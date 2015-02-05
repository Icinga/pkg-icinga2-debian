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

#include "livestatus/servicegroupstable.hpp"
#include "icinga/servicegroup.hpp"
#include "base/dynamictype.hpp"
#include <boost/foreach.hpp>

using namespace icinga;

ServiceGroupsTable::ServiceGroupsTable(void)
{
	AddColumns(this);
}

void ServiceGroupsTable::AddColumns(Table *table, const String& prefix,
    const Column::ObjectAccessor& objectAccessor)
{
	table->AddColumn(prefix + "name", Column(&ServiceGroupsTable::NameAccessor, objectAccessor));
	table->AddColumn(prefix + "alias", Column(&ServiceGroupsTable::AliasAccessor, objectAccessor));
	table->AddColumn(prefix + "notes", Column(&ServiceGroupsTable::NotesAccessor, objectAccessor));
	table->AddColumn(prefix + "notes_url", Column(&ServiceGroupsTable::NotesUrlAccessor, objectAccessor));
	table->AddColumn(prefix + "action_url", Column(&ServiceGroupsTable::ActionUrlAccessor, objectAccessor));
	table->AddColumn(prefix + "members", Column(&ServiceGroupsTable::MembersAccessor, objectAccessor));
	table->AddColumn(prefix + "members_with_state", Column(&ServiceGroupsTable::MembersWithStateAccessor, objectAccessor));
	table->AddColumn(prefix + "worst_service_state", Column(&ServiceGroupsTable::WorstServiceStateAccessor, objectAccessor));
	table->AddColumn(prefix + "num_services", Column(&ServiceGroupsTable::NumServicesAccessor, objectAccessor));
	table->AddColumn(prefix + "num_services_ok", Column(&ServiceGroupsTable::NumServicesOkAccessor, objectAccessor));
	table->AddColumn(prefix + "num_services_warn", Column(&ServiceGroupsTable::NumServicesWarnAccessor, objectAccessor));
	table->AddColumn(prefix + "num_services_crit", Column(&ServiceGroupsTable::NumServicesCritAccessor, objectAccessor));
	table->AddColumn(prefix + "num_services_unknown", Column(&ServiceGroupsTable::NumServicesUnknownAccessor, objectAccessor));
	table->AddColumn(prefix + "num_services_pending", Column(&ServiceGroupsTable::NumServicesPendingAccessor, objectAccessor));
	table->AddColumn(prefix + "num_services_hard_ok", Column(&ServiceGroupsTable::NumServicesHardOkAccessor, objectAccessor));
	table->AddColumn(prefix + "num_services_hard_warn", Column(&ServiceGroupsTable::NumServicesHardWarnAccessor, objectAccessor));
	table->AddColumn(prefix + "num_services_hard_crit", Column(&ServiceGroupsTable::NumServicesHardCritAccessor, objectAccessor));
	table->AddColumn(prefix + "num_services_hard_unknown", Column(&ServiceGroupsTable::NumServicesHardUnknownAccessor, objectAccessor));
}

String ServiceGroupsTable::GetName(void) const
{
	return "servicegroups";
}

String ServiceGroupsTable::GetPrefix(void) const
{
	return "servicegroup";
}

void ServiceGroupsTable::FetchRows(const AddRowFunction& addRowFn)
{
	BOOST_FOREACH(const ServiceGroup::Ptr& sg, DynamicType::GetObjectsByType<ServiceGroup>()) {
		addRowFn(sg);
	}
}

Value ServiceGroupsTable::NameAccessor(const Value& row)
{
	return static_cast<ServiceGroup::Ptr>(row)->GetName();
}

Value ServiceGroupsTable::AliasAccessor(const Value& row)
{
	return static_cast<ServiceGroup::Ptr>(row)->GetDisplayName();
}

Value ServiceGroupsTable::NotesAccessor(const Value& row)
{
	return static_cast<ServiceGroup::Ptr>(row)->GetNotes();
}

Value ServiceGroupsTable::NotesUrlAccessor(const Value& row)
{
	return static_cast<ServiceGroup::Ptr>(row)->GetNotesUrl();
}

Value ServiceGroupsTable::ActionUrlAccessor(const Value& row)
{
	return static_cast<ServiceGroup::Ptr>(row)->GetActionUrl();
}

Value ServiceGroupsTable::MembersAccessor(const Value& row)
{
	Array::Ptr members = new Array();

	BOOST_FOREACH(const Service::Ptr& service, static_cast<ServiceGroup::Ptr>(row)->GetMembers()) {
		Array::Ptr host_svc = new Array();
		host_svc->Add(service->GetHost()->GetName());
		host_svc->Add(service->GetShortName());
		members->Add(host_svc);
	}

	return members;
}

Value ServiceGroupsTable::MembersWithStateAccessor(const Value& row)
{
	Array::Ptr members = new Array();

	BOOST_FOREACH(const Service::Ptr& service, static_cast<ServiceGroup::Ptr>(row)->GetMembers()) {
		Array::Ptr host_svc = new Array();
		host_svc->Add(service->GetHost()->GetName());
		host_svc->Add(service->GetShortName());
		host_svc->Add(service->GetHost()->GetState());
		host_svc->Add(service->GetState());
		members->Add(host_svc);
	}

	return members;
}

Value ServiceGroupsTable::WorstServiceStateAccessor(const Value& row)
{
	Value worst_service = ServiceOK;

	BOOST_FOREACH(const Service::Ptr& service, static_cast<ServiceGroup::Ptr>(row)->GetMembers()) {
		if (service->GetState() > worst_service)
			worst_service = service->GetState();
	}

	return worst_service;
}

Value ServiceGroupsTable::NumServicesAccessor(const Value& row)
{
	return static_cast<long>(static_cast<ServiceGroup::Ptr>(row)->GetMembers().size());
}

Value ServiceGroupsTable::NumServicesOkAccessor(const Value& row)
{
	int num_services = 0;

	BOOST_FOREACH(const Service::Ptr& service, static_cast<ServiceGroup::Ptr>(row)->GetMembers()) {
		if (service->GetState() == ServiceOK)
			num_services++;
	}

	return num_services;
}

Value ServiceGroupsTable::NumServicesWarnAccessor(const Value& row)
{
	int num_services = 0;

	BOOST_FOREACH(const Service::Ptr& service, static_cast<ServiceGroup::Ptr>(row)->GetMembers()) {
		if (service->GetState() == ServiceWarning)
			num_services++;
	}

	return num_services;
}

Value ServiceGroupsTable::NumServicesCritAccessor(const Value& row)
{
	int num_services = 0;

	BOOST_FOREACH(const Service::Ptr& service, static_cast<ServiceGroup::Ptr>(row)->GetMembers()) {
		if (service->GetState() == ServiceCritical)
			num_services++;
	}

	return num_services;
}

Value ServiceGroupsTable::NumServicesUnknownAccessor(const Value& row)
{
	int num_services = 0;

	BOOST_FOREACH(const Service::Ptr& service, static_cast<ServiceGroup::Ptr>(row)->GetMembers()) {
		if (service->GetState() == ServiceUnknown)
			num_services++;
	}

	return num_services;
}

Value ServiceGroupsTable::NumServicesPendingAccessor(const Value& row)
{
	int num_services = 0;

	BOOST_FOREACH(const Service::Ptr& service, static_cast<ServiceGroup::Ptr>(row)->GetMembers()) {
		if (!service->GetLastCheckResult())
			num_services++;
	}

	return num_services;
}

Value ServiceGroupsTable::NumServicesHardOkAccessor(const Value& row)
{
	int num_services = 0;

	BOOST_FOREACH(const Service::Ptr& service, static_cast<ServiceGroup::Ptr>(row)->GetMembers()) {
		if (service->GetStateType() == StateTypeHard && service->GetState() == ServiceOK)
			num_services++;
	}

	return num_services;
}

Value ServiceGroupsTable::NumServicesHardWarnAccessor(const Value& row)
{
	int num_services = 0;

	BOOST_FOREACH(const Service::Ptr& service, static_cast<ServiceGroup::Ptr>(row)->GetMembers()) {
		if (service->GetStateType() == StateTypeHard && service->GetState() == ServiceWarning)
			num_services++;
	}

	return num_services;
}

Value ServiceGroupsTable::NumServicesHardCritAccessor(const Value& row)
{
	int num_services = 0;

	BOOST_FOREACH(const Service::Ptr& service, static_cast<ServiceGroup::Ptr>(row)->GetMembers()) {
		if (service->GetStateType() == StateTypeHard && service->GetState() == ServiceCritical)
			num_services++;
	}

	return num_services;
}

Value ServiceGroupsTable::NumServicesHardUnknownAccessor(const Value& row)
{
	int num_services = 0;

	BOOST_FOREACH(const Service::Ptr& service, static_cast<ServiceGroup::Ptr>(row)->GetMembers()) {
		if (service->GetStateType() == StateTypeHard && service->GetState() == ServiceUnknown)
			num_services++;
	}

	return num_services;
}
