/******************************************************************************
 * Icinga 2                                                                   *
 * Copyright (C) 2012-2014 Icinga Development Team (http://www.icinga.org)    *
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

#include "icinga/checkable.hpp"
#include "icinga/icingaapplication.hpp"
#include "base/objectlock.hpp"
#include "base/logger_fwd.hpp"
#include "base/exception.hpp"
#include "base/context.hpp"
#include "base/convert.hpp"
#include <boost/foreach.hpp>

using namespace icinga;

boost::signals2::signal<void (const Notification::Ptr&, const Checkable::Ptr&, const std::set<User::Ptr>&,
    const NotificationType&, const CheckResult::Ptr&, const String&, const String&)> Checkable::OnNotificationSentToAllUsers;
boost::signals2::signal<void (const Notification::Ptr&, const Checkable::Ptr&, const std::set<User::Ptr>&,
    const NotificationType&, const CheckResult::Ptr&, const String&, const String&)> Checkable::OnNotificationSendStart;
boost::signals2::signal<void (const Notification::Ptr&, const Checkable::Ptr&, const User::Ptr&,
    const NotificationType&, const CheckResult::Ptr&, const String&, const String&, const String&)> Checkable::OnNotificationSentToUser;

void Checkable::ResetNotificationNumbers(void)
{
	BOOST_FOREACH(const Notification::Ptr& notification, GetNotifications()) {
		ObjectLock olock(notification);
		notification->ResetNotificationNumber();
	}
}

void Checkable::SendNotifications(NotificationType type, const CheckResult::Ptr& cr, const String& author, const String& text)
{
	CONTEXT("Sending notifications for object '" + GetName() + "'");

	bool force = GetForceNextNotification();

	if (!IcingaApplication::GetInstance()->GetEnableNotifications() || !GetEnableNotifications()) {
		if (!force) {
			Log(LogInformation, "Checkable", "Notifications are disabled for service '" + GetName() + "'.");
			return;
		}

		SetForceNextNotification(false);
	}

	Log(LogInformation, "Checkable", "Checking for configured notifications for object '" + GetName() + "'");

	std::set<Notification::Ptr> notifications = GetNotifications();

	if (notifications.empty())
		Log(LogInformation, "Checkable", "Checkable '" + GetName() + "' does not have any notifications.");

	Log(LogDebug, "Checkable", "Checkable '" + GetName() + "' has " + Convert::ToString(notifications.size()) + " notification(s).");

	BOOST_FOREACH(const Notification::Ptr& notification, notifications) {
		try {
			notification->BeginExecuteNotification(type, cr, force, author, text);
		} catch (const std::exception& ex) {
			std::ostringstream msgbuf;
			msgbuf << "Exception occured during notification for service '"
			       << GetName() << "': " << DiagnosticInformation(ex);
			String message = msgbuf.str();

			Log(LogWarning, "Checkable", message);
		}
	}
}

std::set<Notification::Ptr> Checkable::GetNotifications(void) const
{
	return m_Notifications;
}

void Checkable::AddNotification(const Notification::Ptr& notification)
{
	m_Notifications.insert(notification);
}

void Checkable::RemoveNotification(const Notification::Ptr& notification)
{
	m_Notifications.erase(notification);
}

bool Checkable::GetEnableNotifications(void) const
{
	if (!GetOverrideEnableNotifications().IsEmpty())
		return GetOverrideEnableNotifications();
	else
		return GetEnableNotificationsRaw();
}

void Checkable::SetEnableNotifications(bool enabled, const MessageOrigin& origin)
{
	SetOverrideEnableNotifications(enabled);

	OnEnableNotificationsChanged(GetSelf(), enabled, origin);
}

bool Checkable::GetForceNextNotification(void) const
{
	return GetForceNextNotificationRaw();
}

void Checkable::SetForceNextNotification(bool forced, const MessageOrigin& origin)
{
	SetForceNextNotificationRaw(forced);

	OnForceNextNotificationChanged(GetSelf(), forced, origin);
}
