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

#include "db_ido/dbobject.hpp"
#include "db_ido/dbtype.hpp"
#include "db_ido/dbvalue.hpp"
#include "icinga/customvarobject.hpp"
#include "icinga/service.hpp"
#include "icinga/compatutility.hpp"
#include "remote/endpoint.hpp"
#include "base/dynamicobject.hpp"
#include "base/dynamictype.hpp"
#include "base/json.hpp"
#include "base/convert.hpp"
#include "base/objectlock.hpp"
#include "base/utility.hpp"
#include "base/initialize.hpp"
#include "base/logger.hpp"
#include <boost/foreach.hpp>

using namespace icinga;

boost::signals2::signal<void (const DbQuery&)> DbObject::OnQuery;

INITIALIZE_ONCE(&DbObject::StaticInitialize);

DbObject::DbObject(const intrusive_ptr<DbType>& type, const String& name1, const String& name2)
	: m_Name1(name1), m_Name2(name2), m_Type(type), m_LastConfigUpdate(0), m_LastStatusUpdate(0)
{ }

void DbObject::StaticInitialize(void)
{
	/* triggered in ProcessCheckResult(), requires UpdateNextCheck() to be called before */
	DynamicObject::OnStateChanged.connect(boost::bind(&DbObject::StateChangedHandler, _1));
	CustomVarObject::OnVarsChanged.connect(boost::bind(&DbObject::VarsChangedHandler, _1));
}

void DbObject::SetObject(const DynamicObject::Ptr& object)
{
	m_Object = object;
}

DynamicObject::Ptr DbObject::GetObject(void) const
{
	return m_Object;
}

String DbObject::GetName1(void) const
{
	return m_Name1;
}

String DbObject::GetName2(void) const
{
	return m_Name2;
}

DbType::Ptr DbObject::GetType(void) const
{
	return m_Type;
}

void DbObject::SendConfigUpdate(void)
{
	/* update custom var config for all objects */
	SendVarsConfigUpdate();

	/* config objects */
	Dictionary::Ptr fields = GetConfigFields();

	if (!fields)
		return;

	DbQuery query;
	query.Table = GetType()->GetTable() + "s";
	query.Type = DbQueryInsert | DbQueryUpdate;
	query.Category = DbCatConfig;
	query.Fields = fields;
	query.Fields->Set(GetType()->GetIDColumn(), GetObject());
	query.Fields->Set("instance_id", 0); /* DbConnection class fills in real ID */
	query.Fields->Set("config_type", 1);
	query.WhereCriteria = new Dictionary();
	query.WhereCriteria->Set(GetType()->GetIDColumn(), GetObject());
	query.Object = this;
	query.ConfigUpdate = true;
	OnQuery(query);

	m_LastConfigUpdate = Utility::GetTime();

	OnConfigUpdate();
}

void DbObject::SendStatusUpdate(void)
{
	/* update custom var status for all objects */
	SendVarsStatusUpdate();

	/* status objects */
	Dictionary::Ptr fields = GetStatusFields();

	if (!fields)
		return;

	DbQuery query;
	query.Table = GetType()->GetTable() + "status";
	query.Type = DbQueryInsert | DbQueryUpdate;
	query.Category = DbCatState;
	query.Fields = fields;
	query.Fields->Set(GetType()->GetIDColumn(), GetObject());

	/* do not override our own endpoint dbobject id */
	if (GetType()->GetTable() != "endpoint") {
		String node = IcingaApplication::GetInstance()->GetNodeName();

		Log(LogDebug, "DbObject")
		    << "Endpoint node: '" << node << "' status update for '" << GetObject()->GetName() << "'";

		Endpoint::Ptr endpoint = Endpoint::GetByName(node);
		if (endpoint)
			query.Fields->Set("endpoint_object_id", endpoint);
	}

	query.Fields->Set("instance_id", 0); /* DbConnection class fills in real ID */

	query.Fields->Set("status_update_time", DbValue::FromTimestamp(Utility::GetTime()));
	query.WhereCriteria = new Dictionary();
	query.WhereCriteria->Set(GetType()->GetIDColumn(), GetObject());
	query.Object = this;
	query.StatusUpdate = true;
	OnQuery(query);

	m_LastStatusUpdate = Utility::GetTime();

	OnStatusUpdate();
}

void DbObject::SendVarsConfigUpdate(void)
{
	DynamicObject::Ptr obj = GetObject();

	CustomVarObject::Ptr custom_var_object = dynamic_pointer_cast<CustomVarObject>(obj);

	if (!custom_var_object)
		return;

	Dictionary::Ptr vars = CompatUtility::GetCustomAttributeConfig(custom_var_object);

	if (vars) {
		Log(LogDebug, "DbObject")
		    << "Updating object vars for '" << custom_var_object->GetName() << "'";

		ObjectLock olock (vars);

		BOOST_FOREACH(const Dictionary::Pair& kv, vars) {
			if (kv.first.IsEmpty())
				continue;

			String value;
			int is_json = 0;

			if (kv.second.IsObjectType<Array>() || kv.second.IsObjectType<Dictionary>()) {
				value = JsonEncode(kv.second);
				is_json = 1;
			} else
				value = kv.second;

			int overridden = custom_var_object->IsVarOverridden(kv.first) ? 1 : 0;

			Log(LogDebug, "DbObject")
			    << "object customvar key: '" << kv.first << "' value: '" << kv.second
			    << "' overridden: " << overridden;

			Dictionary::Ptr fields = new Dictionary();
			fields->Set("varname", kv.first);
			fields->Set("varvalue", value);
			fields->Set("is_json", is_json);
			fields->Set("config_type", 1);
			fields->Set("has_been_modified", overridden);
			fields->Set("object_id", obj);
			fields->Set("instance_id", 0); /* DbConnection class fills in real ID */

			DbQuery query;
			query.Table = "customvariables";
			query.Type = DbQueryInsert;
			query.Category = DbCatConfig;
			query.Fields = fields;
			OnQuery(query);
		}
	}
}

void DbObject::SendVarsStatusUpdate(void)
{
	DynamicObject::Ptr obj = GetObject();

	CustomVarObject::Ptr custom_var_object = dynamic_pointer_cast<CustomVarObject>(obj);

	if (!custom_var_object)
		return;

	Dictionary::Ptr vars = CompatUtility::GetCustomAttributeConfig(custom_var_object);

	if (vars) {
		Log(LogDebug, "DbObject")
		    << "Updating object vars for '" << custom_var_object->GetName() << "'";

		ObjectLock olock (vars);

		BOOST_FOREACH(const Dictionary::Pair& kv, vars) {
			if (kv.first.IsEmpty())
				continue;

			String value;
			int is_json = 0;

			if (kv.second.IsObjectType<Array>() || kv.second.IsObjectType<Dictionary>()) {
				value = JsonEncode(kv.second);
				is_json = 1;
			} else
				value = kv.second;

			int overridden = custom_var_object->IsVarOverridden(kv.first) ? 1 : 0;

			Log(LogDebug, "DbObject")
			    << "object customvar key: '" << kv.first << "' value: '" << kv.second
			    << "' overridden: " << overridden;

			Dictionary::Ptr fields = new Dictionary();
			fields->Set("varname", kv.first);
			fields->Set("varvalue", value);
			fields->Set("is_json", is_json);
			fields->Set("has_been_modified", overridden);
			fields->Set("status_update_time", DbValue::FromTimestamp(Utility::GetTime()));
			fields->Set("object_id", obj);
			fields->Set("instance_id", 0); /* DbConnection class fills in real ID */

			DbQuery query;
			query.Table = "customvariablestatus";
			query.Type = DbQueryInsert | DbQueryUpdate;
			query.Category = DbCatState;
			query.Fields = fields;

			query.WhereCriteria = new Dictionary();
			query.WhereCriteria->Set("object_id", obj);
			query.WhereCriteria->Set("varname", Convert::ToString(kv.first));
			query.Object = this;

			OnQuery(query);
		}
	}
}

double DbObject::GetLastConfigUpdate(void) const
{
	return m_LastConfigUpdate;
}

double DbObject::GetLastStatusUpdate(void) const
{
	return m_LastStatusUpdate;
}

bool DbObject::IsStatusAttribute(const String&) const
{
	return false;
}

void DbObject::OnConfigUpdate(void)
{
	/* Default handler does nothing. */
}

void DbObject::OnStatusUpdate(void)
{
	/* Default handler does nothing. */
}

DbObject::Ptr DbObject::GetOrCreateByObject(const DynamicObject::Ptr& object)
{
	DbObject::Ptr dbobj = object->GetExtension("DbObject");

	if (dbobj)
		return dbobj;

	DbType::Ptr dbtype = DbType::GetByName(object->GetType()->GetName());

	if (!dbtype)
		return DbObject::Ptr();

	Service::Ptr service;
	String name1, name2;

	service = dynamic_pointer_cast<Service>(object);

	if (service) {
		Host::Ptr host = service->GetHost();

		name1 = service->GetHost()->GetName();
		name2 = service->GetShortName();
	} else {
		if (object->GetType() == DynamicType::GetByName("CheckCommand") ||
		    object->GetType() == DynamicType::GetByName("EventCommand") ||
		    object->GetType() == DynamicType::GetByName("NotificationCommand")) {
			Command::Ptr command = dynamic_pointer_cast<Command>(object);
			name1 = CompatUtility::GetCommandName(command);
		}
		else
			name1 = object->GetName();
	}

	dbobj = dbtype->GetOrCreateObjectByName(name1, name2);

	{
		ObjectLock olock(object);
		dbobj->SetObject(object);
		object->SetExtension("DbObject", dbobj);
	}

	return dbobj;
}

void DbObject::StateChangedHandler(const DynamicObject::Ptr& object)
{
	DbObject::Ptr dbobj = GetOrCreateByObject(object);

	if (!dbobj)
		return;

	dbobj->SendStatusUpdate();
}

void DbObject::VarsChangedHandler(const CustomVarObject::Ptr& object)
{
	DbObject::Ptr dbobj = GetOrCreateByObject(object);

	Log(LogDebug, "DbObject")
	    << "Vars changed for object '" << object->GetName() << "'";

	if (!dbobj)
		return;

	dbobj->SendVarsStatusUpdate();
}
