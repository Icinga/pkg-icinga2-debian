/******************************************************************************
 * Icinga 2                                                                   *
 * Copyright (C) 2012-2015 Icinga Development Team (http://www.icinga.org)    *
 *                                                                            *
 * This program is free software; you can redistribute it and/or              *
 * modify it under the terms of the GNU General Public License                *
 * as published by the Free Software Foundation; either version 2             *
 * of the License, or (at your option) any later version.                     *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program; if not, write to the Free Software Foundation     *
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.             *
 ******************************************************************************/

#include "base/timer.hpp"
#include "base/debug.hpp"
#include "base/utility.hpp"
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/key_extractors.hpp>

using namespace icinga;

typedef boost::multi_index_container<
	Timer::Holder,
	boost::multi_index::indexed_by<
		boost::multi_index::ordered_unique<boost::multi_index::const_mem_fun<Timer::Holder, Timer *, &Timer::Holder::GetObject> >,
		boost::multi_index::ordered_non_unique<boost::multi_index::const_mem_fun<Timer::Holder, double, &Timer::Holder::GetNextUnlocked> >
	>
> TimerSet;

static boost::mutex l_Mutex;
static boost::condition_variable l_CV;
static boost::thread l_Thread;
static bool l_StopThread;
static TimerSet l_Timers;

/**
 * Constructor for the Timer class.
 */
Timer::Timer(void)
	: m_Interval(0), m_Next(0)
{ }

/**
 * Destructor for the Timer class.
 */
Timer::~Timer(void)
{
	Stop();
}

/**
 * Initializes the timer sub-system.
 */
void Timer::Initialize(void)
{
	boost::mutex::scoped_lock lock(l_Mutex);
	l_StopThread = false;
	l_Thread = boost::thread(&Timer::TimerThreadProc);
}

/**
 * Disables the timer sub-system.
 */
void Timer::Uninitialize(void)
{
	{
		boost::mutex::scoped_lock lock(l_Mutex);
		l_StopThread = true;
		l_CV.notify_all();
	}

	if (l_Thread.joinable())
		l_Thread.join();
}

/**
 * Calls this timer.
 */
void Timer::Call(void)
{
	ASSERT(!OwnsLock());

	try {
		OnTimerExpired(Timer::Ptr(this));
	} catch (...) {
		Reschedule();

		throw;
	}

	Reschedule();
}

/**
 * Sets the interval for this timer.
 *
 * @param interval The new interval.
 */
void Timer::SetInterval(double interval)
{
	ASSERT(!OwnsLock());

	boost::mutex::scoped_lock lock(l_Mutex);
	m_Interval = interval;
}

/**
 * Retrieves the interval for this timer.
 *
 * @returns The interval.
 */
double Timer::GetInterval(void) const
{
	ASSERT(!OwnsLock());

	boost::mutex::scoped_lock lock(l_Mutex);
	return m_Interval;
}

/**
 * Registers the timer and starts processing events for it.
 */
void Timer::Start(void)
{
	ASSERT(!OwnsLock());

	{
		boost::mutex::scoped_lock lock(l_Mutex);
		m_Started = true;
	}

	Reschedule();
}

/**
 * Unregisters the timer and stops processing events for it.
 */
void Timer::Stop(void)
{
	ASSERT(!OwnsLock());

	if (l_StopThread)
		return;

	boost::mutex::scoped_lock lock(l_Mutex);

	m_Started = false;
	l_Timers.erase(this);

	/* Notify the worker thread that we've disabled a timer. */
	l_CV.notify_all();
}

/**
 * Reschedules this timer.
 *
 * @param next The time when this timer should be called again. Use -1 to let
 * 	       the timer figure out a suitable time based on the interval.
 */
void Timer::Reschedule(double next)
{
	ASSERT(!OwnsLock());

	boost::mutex::scoped_lock lock(l_Mutex);

	if (next < 0) {
		/* Don't schedule the next call if this is not a periodic timer. */
		if (m_Interval <= 0)
			return;

		next = Utility::GetTime() + m_Interval;
	}

	m_Next = next;

	if (m_Started) {
		/* Remove and re-add the timer to update the index. */
		l_Timers.erase(this);
		l_Timers.insert(this);

		/* Notify the worker that we've rescheduled a timer. */
		l_CV.notify_all();
	}
}

/**
 * Retrieves when the timer is next due.
 *
 * @returns The timestamp.
 */
double Timer::GetNext(void) const
{
	ASSERT(!OwnsLock());

	boost::mutex::scoped_lock lock(l_Mutex);
	return m_Next;
}

/**
 * Adjusts all timers by adding the specified amount of time to their
 * next scheduled timestamp.
 *
 * @param adjustment The adjustment.
 */
void Timer::AdjustTimers(double adjustment)
{
	boost::mutex::scoped_lock lock(l_Mutex);

	double now = Utility::GetTime();

	typedef boost::multi_index::nth_index<TimerSet, 1>::type TimerView;
	TimerView& idx = boost::get<1>(l_Timers);

	std::vector<Timer *> timers;

	BOOST_FOREACH(Timer *timer, idx) {
		if (abs(now - (timer->m_Next + adjustment)) <
		    abs(now - timer->m_Next)) {
			timer->m_Next += adjustment;
			timers.push_back(timer);
		}
	}

	BOOST_FOREACH(Timer *timer, timers) {
		l_Timers.erase(timer);
		l_Timers.insert(timer);
	}

	/* Notify the worker that we've rescheduled some timers. */
	l_CV.notify_all();
}

/**
 * Worker thread proc for Timer objects.
 */
void Timer::TimerThreadProc(void)
{
	Utility::SetThreadName("Timer Thread");

	for (;;) {
		boost::mutex::scoped_lock lock(l_Mutex);

		typedef boost::multi_index::nth_index<TimerSet, 1>::type NextTimerView;
		NextTimerView& idx = boost::get<1>(l_Timers);

		/* Wait until there is at least one timer. */
		while (idx.empty() && !l_StopThread)
			l_CV.wait(lock);

		if (l_StopThread)
			break;

		NextTimerView::iterator it = idx.begin();
		Timer *timer = *it;

		double wait = timer->m_Next - Utility::GetTime();

		if (wait > 0.01) {
			/* Wait for the next timer. */
			l_CV.timed_wait(lock, boost::posix_time::milliseconds(wait * 1000));

			continue;
		}

		Timer::Ptr ptimer = timer;

		/* Remove the timer from the list so it doesn't get called again
		 * until the current call is completed. */
		l_Timers.erase(timer);

		lock.unlock();

		/* Asynchronously call the timer. */
		Utility::QueueAsyncCallback(boost::bind(&Timer::Call, ptimer));
	}
}
