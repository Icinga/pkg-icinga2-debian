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

#include "livestatus/endpointstable.hpp"
#include "icinga/host.hpp"
#include "icinga/service.hpp"
#include "icinga/icingaapplication.hpp"
#include "remote/endpoint.hpp"
#include "base/dynamictype.hpp"
#include "base/objectlock.hpp"
#include "base/convert.hpp"
#include "base/utility.hpp"
#include <boost/algorithm/string/classification.hpp>
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>

using namespace icinga;

EndpointsTable::EndpointsTable(void)
{
	AddColumns(this);
}

void EndpointsTable::AddColumns(Table *table, const String& prefix,
    const Column::ObjectAccessor& objectAccessor)
{
	table->AddColumn(prefix + "name", Column(&EndpointsTable::NameAccessor, objectAccessor));
	table->AddColumn(prefix + "identity", Column(&EndpointsTable::IdentityAccessor, objectAccessor));
	table->AddColumn(prefix + "node", Column(&EndpointsTable::NodeAccessor, objectAccessor));
	table->AddColumn(prefix + "is_connected", Column(&EndpointsTable::IsConnectedAccessor, objectAccessor));
}

String EndpointsTable::GetName(void) const
{
	return "endpoints";
}

String EndpointsTable::GetPrefix(void) const
{
	return "endpoint";
}

void EndpointsTable::FetchRows(const AddRowFunction& addRowFn)
{
	BOOST_FOREACH(const Endpoint::Ptr& endpoint, DynamicType::GetObjectsByType<Endpoint>()) {
		addRowFn(endpoint);
	}
}

Value EndpointsTable::NameAccessor(const Value& row)
{
	Endpoint::Ptr endpoint = static_cast<Endpoint::Ptr>(row);

	if (!endpoint)
		return Empty;

	return endpoint->GetName();
}

Value EndpointsTable::IdentityAccessor(const Value& row)
{
	Endpoint::Ptr endpoint = static_cast<Endpoint::Ptr>(row);

	if (!endpoint)
		return Empty;

	return endpoint->GetName();
}

Value EndpointsTable::NodeAccessor(const Value& row)
{
	Endpoint::Ptr endpoint = static_cast<Endpoint::Ptr>(row);

	if (!endpoint)
		return Empty;

	return IcingaApplication::GetInstance()->GetNodeName();
}

Value EndpointsTable::IsConnectedAccessor(const Value& row)
{
	Endpoint::Ptr endpoint = static_cast<Endpoint::Ptr>(row);

	if (!endpoint)
		return Empty;

	unsigned int is_connected = endpoint->IsConnected() ? 1 : 0;

	/* if identity is equal to node, fake is_connected */
	if (endpoint->GetName() == IcingaApplication::GetInstance()->GetNodeName())
		is_connected = 1;

	return is_connected;
}



