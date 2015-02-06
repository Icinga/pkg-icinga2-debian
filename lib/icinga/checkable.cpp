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

#include "icinga/checkable.hpp"
#include "config/configcompilercontext.hpp"
#include "base/objectlock.hpp"
#include "base/utility.hpp"
#include "base/scriptfunction.hpp"
#include <boost/foreach.hpp>
#include <boost/bind/apply.hpp>

using namespace icinga;

REGISTER_TYPE(Checkable);
REGISTER_SCRIPTFUNCTION(ValidateCheckableCheckInterval, &Checkable::ValidateCheckInterval);

boost::signals2::signal<void (const Checkable::Ptr&, bool, const MessageOrigin&)> Checkable::OnEnablePerfdataChanged;
boost::signals2::signal<void (const Checkable::Ptr&, const String&, const String&, AcknowledgementType, double, const MessageOrigin&)> Checkable::OnAcknowledgementSet;
boost::signals2::signal<void (const Checkable::Ptr&, const MessageOrigin&)> Checkable::OnAcknowledgementCleared;

Checkable::Checkable(void)
	: m_CheckRunning(false)
{ }

void Checkable::Start(void)
{
	double now = Utility::GetTime();

	if (GetNextCheck() < now + 300)
		UpdateNextCheck();

	DynamicObject::Start();
}

void Checkable::OnConfigLoaded(void)
{
	DynamicObject::OnConfigLoaded();

	SetSchedulingOffset(Utility::Random());
}

void Checkable::OnStateLoaded(void)
{
	AddDowntimesToCache();
	AddCommentsToCache();

	std::vector<String> ids;
	Dictionary::Ptr downtimes = GetDowntimes();

	{
		ObjectLock dlock(downtimes);
		BOOST_FOREACH(const Dictionary::Pair& kv, downtimes) {
			Downtime::Ptr downtime = kv.second;

			if (downtime->GetScheduledBy().IsEmpty())
				continue;

			ids.push_back(kv.first);
		}
	}

	BOOST_FOREACH(const String& id, ids) {
		/* override config owner to clear downtimes once */
		Downtime::Ptr downtime = GetDowntimeByID(id);
		downtime->SetConfigOwner(Empty);
		RemoveDowntime(id, true);
	}
}

void Checkable::AddGroup(const String& name)
{
	boost::mutex::scoped_lock lock(m_CheckableMutex);

	Array::Ptr groups = GetGroups();

	if (groups && groups->Contains(name))
		return;

	if (!groups)
		groups = new Array();

	groups->Add(name);
}

AcknowledgementType Checkable::GetAcknowledgement(void)
{
	ASSERT(OwnsLock());

	AcknowledgementType avalue = static_cast<AcknowledgementType>(GetAcknowledgementRaw());

	if (avalue != AcknowledgementNone) {
		double expiry = GetAcknowledgementExpiry();

		if (expiry != 0 && expiry < Utility::GetTime()) {
			avalue = AcknowledgementNone;
			ClearAcknowledgement();
		}
	}

	return avalue;
}

bool Checkable::IsAcknowledged(void)
{
	return GetAcknowledgement() != AcknowledgementNone;
}

void Checkable::AcknowledgeProblem(const String& author, const String& comment, AcknowledgementType type, double expiry, const MessageOrigin& origin)
{
	{
		ObjectLock olock(this);

		SetAcknowledgementRaw(type);
		SetAcknowledgementExpiry(expiry);
	}

	OnNotificationsRequested(this, NotificationAcknowledgement, GetLastCheckResult(), author, comment);

	OnAcknowledgementSet(this, author, comment, type, expiry, origin);
}

void Checkable::ClearAcknowledgement(const MessageOrigin& origin)
{
	ASSERT(OwnsLock());

	SetAcknowledgementRaw(AcknowledgementNone);
	SetAcknowledgementExpiry(0);

	OnAcknowledgementCleared(this, origin);
}

bool Checkable::GetEnablePerfdata(void) const
{
	if (!GetOverrideEnablePerfdata().IsEmpty())
		return GetOverrideEnablePerfdata();
	else
		return GetEnablePerfdataRaw();
}

void Checkable::SetEnablePerfdata(bool enabled, const MessageOrigin& origin)
{
	SetOverrideEnablePerfdata(enabled);

	OnEnablePerfdataChanged(this, enabled, origin);
}

int Checkable::GetModifiedAttributes(void) const
{
	int attrs = 0;

	if (!GetOverrideEnableNotifications().IsEmpty())
		attrs |= ModAttrNotificationsEnabled;

	if (!GetOverrideEnableActiveChecks().IsEmpty())
		attrs |= ModAttrActiveChecksEnabled;

	if (!GetOverrideEnablePassiveChecks().IsEmpty())
		attrs |= ModAttrPassiveChecksEnabled;

	if (!GetOverrideEnableFlapping().IsEmpty())
		attrs |= ModAttrFlapDetectionEnabled;

	if (!GetOverrideEnableEventHandler().IsEmpty())
		attrs |= ModAttrEventHandlerEnabled;

	if (!GetOverrideEnablePerfdata().IsEmpty())
		attrs |= ModAttrPerformanceDataEnabled;

	if (!GetOverrideCheckInterval().IsEmpty())
		attrs |= ModAttrNormalCheckInterval;

	if (!GetOverrideRetryInterval().IsEmpty())
		attrs |= ModAttrRetryCheckInterval;

	if (!GetOverrideEventCommand().IsEmpty())
		attrs |= ModAttrEventHandlerCommand;

	if (!GetOverrideCheckCommand().IsEmpty())
		attrs |= ModAttrCheckCommand;

	if (!GetOverrideMaxCheckAttempts().IsEmpty())
		attrs |= ModAttrMaxCheckAttempts;

	if (!GetOverrideCheckPeriod().IsEmpty())
		attrs |= ModAttrCheckTimeperiod;

	if (GetOverrideVars())
		attrs |= ModAttrCustomVariable;

	// TODO: finish

	return attrs;
}

void Checkable::SetModifiedAttributes(int flags, const MessageOrigin& origin)
{
	if ((flags & ModAttrNotificationsEnabled) == 0) {
		SetOverrideEnableNotifications(Empty);
		OnEnableNotificationsChanged(this, GetEnableNotifications(), origin);
	}

	if ((flags & ModAttrActiveChecksEnabled) == 0) {
		SetOverrideEnableActiveChecks(Empty);
		OnEnableActiveChecksChanged(this, GetEnableActiveChecks(), origin);
	}

	if ((flags & ModAttrPassiveChecksEnabled) == 0) {
		SetOverrideEnablePassiveChecks(Empty);
		OnEnablePassiveChecksChanged(this, GetEnablePassiveChecks(), origin);
	}

	if ((flags & ModAttrFlapDetectionEnabled) == 0) {
		SetOverrideEnableFlapping(Empty);
		OnEnableFlappingChanged(this, GetEnableFlapping(), origin);
	}

	if ((flags & ModAttrEventHandlerEnabled) == 0)
		SetOverrideEnableEventHandler(Empty);

	if ((flags & ModAttrPerformanceDataEnabled) == 0) {
		SetOverrideEnablePerfdata(Empty);
		OnEnablePerfdataChanged(this, GetEnablePerfdata(), origin);
	}

	if ((flags & ModAttrNormalCheckInterval) == 0)
		SetOverrideCheckInterval(Empty);

	if ((flags & ModAttrRetryCheckInterval) == 0)
		SetOverrideRetryInterval(Empty);

	if ((flags & ModAttrEventHandlerCommand) == 0)
		SetOverrideEventCommand(Empty);

	if ((flags & ModAttrCheckCommand) == 0)
		SetOverrideCheckCommand(Empty);

	if ((flags & ModAttrMaxCheckAttempts) == 0)
		SetOverrideMaxCheckAttempts(Empty);

	if ((flags & ModAttrCheckTimeperiod) == 0)
		SetOverrideCheckPeriod(Empty);

	if ((flags & ModAttrCustomVariable) == 0) {
		SetOverrideVars(Empty);
		OnVarsChanged(this, GetVars(), origin);
	}
}

Endpoint::Ptr Checkable::GetCommandEndpoint(void) const
{
	return Endpoint::GetByName(GetCommandEndpointRaw());
}

void Checkable::ValidateCheckInterval(const String& location, const Dictionary::Ptr& attrs)
{
	if (attrs->Contains("check_interval") && attrs->Get("check_interval") <= 0) {
		ConfigCompilerContext::GetInstance()->AddMessage(true, "Validation failed for " +
		    location + ": check_interval must be greater than 0.");
	}
}
