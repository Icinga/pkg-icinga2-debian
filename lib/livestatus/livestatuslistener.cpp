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

#include "livestatus/livestatuslistener.hpp"
#include "icinga/perfdatavalue.hpp"
#include "config/configcompilercontext.hpp"
#include "base/utility.hpp"
#include "base/objectlock.hpp"
#include "base/dynamictype.hpp"
#include "base/logger.hpp"
#include "base/exception.hpp"
#include "base/tcpsocket.hpp"
#include "base/unixsocket.hpp"
#include "base/networkstream.hpp"
#include "base/application.hpp"
#include "base/scriptfunction.hpp"
#include "base/statsfunction.hpp"
#include "base/convert.hpp"

using namespace icinga;

REGISTER_TYPE(LivestatusListener);
REGISTER_SCRIPTFUNCTION(ValidateSocketType, &LivestatusListener::ValidateSocketType);

static int l_ClientsConnected = 0;
static int l_Connections = 0;
static boost::mutex l_ComponentMutex;

REGISTER_STATSFUNCTION(LivestatusListenerStats, &LivestatusListener::StatsFunc);

Value LivestatusListener::StatsFunc(const Dictionary::Ptr& status, const Array::Ptr& perfdata)
{
	Dictionary::Ptr nodes = new Dictionary();

	BOOST_FOREACH(const LivestatusListener::Ptr& livestatuslistener, DynamicType::GetObjectsByType<LivestatusListener>()) {
		Dictionary::Ptr stats = new Dictionary();
		stats->Set("connections", l_Connections);

		nodes->Set(livestatuslistener->GetName(), stats);

		perfdata->Add(new PerfdataValue("livestatuslistener_" + livestatuslistener->GetName() + "_connections", l_Connections));
	}

	status->Set("livestatuslistener", nodes);

	return 0;
}

/**
 * Starts the component.
 */
void LivestatusListener::Start(void)
{
	DynamicObject::Start();

	if (GetSocketType() == "tcp") {
		TcpSocket::Ptr socket = new TcpSocket();

		try {
			socket->Bind(GetBindHost(), GetBindPort(), AF_UNSPEC);
		} catch (std::exception&) {
			Log(LogCritical, "LivestatusListener")
			    << "Cannot bind TCP socket on host '" << GetBindHost() << "' port '" << GetBindPort() << "'.";
			return;
		}

		m_Listener = socket;

		boost::thread thread(boost::bind(&LivestatusListener::ServerThreadProc, this, socket));
		thread.detach();
		Log(LogInformation, "LivestatusListener")
		    << "Created TCP socket listening on host '" << GetBindHost() << "' port '" << GetBindPort() << "'.";
	}
	else if (GetSocketType() == "unix") {
#ifndef _WIN32
		UnixSocket::Ptr socket = new UnixSocket();

		try {
			socket->Bind(GetSocketPath());
		} catch (std::exception&) {
			Log(LogCritical, "LivestatusListener")
			    << "Cannot bind UNIX socket to '" << GetSocketPath() << "'.";
			return;
		}

		/* group must be able to write */
		mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;

		if (chmod(GetSocketPath().CStr(), mode) < 0) {
			Log(LogCritical, "LivestatusListener")
			    << "chmod() on unix socket '" << GetSocketPath() << "' failed with error code " << errno << ", \"" << Utility::FormatErrorNumber(errno) << "\"";
			return;
		}

		m_Listener = socket;

		boost::thread thread(boost::bind(&LivestatusListener::ServerThreadProc, this, socket));
		thread.detach();
		Log(LogInformation, "LivestatusListener")
		    << "Created UNIX socket in '" << GetSocketPath() << "'.";
#else
		/* no UNIX sockets on windows */
		Log(LogCritical, "LivestatusListener", "Unix sockets are not supported on Windows.");
		return;
#endif
	}
}

void LivestatusListener::Stop(void)
{
	DynamicObject::Stop();

	m_Listener->Close();
}

int LivestatusListener::GetClientsConnected(void)
{
	boost::mutex::scoped_lock lock(l_ComponentMutex);

	return l_ClientsConnected;
}

int LivestatusListener::GetConnections(void)
{
	boost::mutex::scoped_lock lock(l_ComponentMutex);

	return l_Connections;
}

void LivestatusListener::ServerThreadProc(const Socket::Ptr& server)
{
	server->Listen();

	try {
		for (;;) {
			timeval tv = { 0, 500000 };

			if (m_Listener->Poll(true, false, &tv)) {
				Socket::Ptr client = m_Listener->Accept();
				Log(LogNotice, "LivestatusListener", "Client connected");
				Utility::QueueAsyncCallback(boost::bind(&LivestatusListener::ClientHandler, this, client), LowLatencyScheduler);
			}

			if (!IsActive())
				break;
		}
	} catch (std::exception&) {
		Log(LogCritical, "ListenerListener", "Cannot accept new connection.");
	}

	m_Listener->Close();
}

void LivestatusListener::ClientHandler(const Socket::Ptr& client)
{
	{
		boost::mutex::scoped_lock lock(l_ComponentMutex);
		l_ClientsConnected++;
		l_Connections++;
	}

	Stream::Ptr stream = new NetworkStream(client);

	for (;;) {
		String line;
		ReadLineContext context;

		std::vector<String> lines;

		while (stream->ReadLine(&line, context)) {
			if (line.GetLength() > 0)
				lines.push_back(line);
			else
				break;
		}

		if (lines.empty())
			break;

		LivestatusQuery::Ptr query = new LivestatusQuery(lines, GetCompatLogPath());
		if (!query->Execute(stream))
			break;
	}

	{
		boost::mutex::scoped_lock lock(l_ComponentMutex);
		l_ClientsConnected--;
	}
}


void LivestatusListener::ValidateSocketType(const String& location, const Dictionary::Ptr& attrs)
{
	Value socket_type = attrs->Get("socket_type");

	if (!socket_type.IsEmpty() && socket_type != "unix" && socket_type != "tcp") {
		ConfigCompilerContext::GetInstance()->AddMessage(true, "Validation failed for " +
		    location + ": Socket type '" + socket_type + "' is invalid.");
	}
}
