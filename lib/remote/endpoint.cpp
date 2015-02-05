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

#include "remote/endpoint.hpp"
#include "remote/apilistener.hpp"
#include "remote/apiclient.hpp"
#include "remote/zone.hpp"
#include "base/dynamictype.hpp"
#include "base/utility.hpp"
#include "base/exception.hpp"
#include "base/convert.hpp"
#include <boost/foreach.hpp>

using namespace icinga;

REGISTER_TYPE(Endpoint);

boost::signals2::signal<void(const Endpoint::Ptr&, const ApiClient::Ptr&)> Endpoint::OnConnected;
boost::signals2::signal<void(const Endpoint::Ptr&, const ApiClient::Ptr&)> Endpoint::OnDisconnected;

void Endpoint::OnConfigLoaded(void)
{
	DynamicObject::OnConfigLoaded();

	BOOST_FOREACH(const Zone::Ptr& zone, DynamicType::GetObjectsByType<Zone>()) {
		const std::set<Endpoint::Ptr> members = zone->GetEndpoints();

		if (members.empty())
			continue;

		if (members.find(this) != members.end()) {
			if (m_Zone)
				BOOST_THROW_EXCEPTION(std::runtime_error("Endpoint '" + GetName() + "' is in more than one zone."));

			m_Zone = zone;
		}
	}

	if (!m_Zone)
		BOOST_THROW_EXCEPTION(std::runtime_error("Endpoint '" + GetName() + "' does not belong to a zone."));
}

void Endpoint::AddClient(const ApiClient::Ptr& client)
{
	bool was_master = ApiListener::GetInstance()->IsMaster();

	{
		boost::mutex::scoped_lock lock(m_ClientsLock);
		m_Clients.insert(client);
	}

	bool is_master = ApiListener::GetInstance()->IsMaster();

	if (was_master != is_master)
		ApiListener::OnMasterChanged(is_master);

	OnConnected(this, client);
}

void Endpoint::RemoveClient(const ApiClient::Ptr& client)
{
	bool was_master = ApiListener::GetInstance()->IsMaster();

	{
		boost::mutex::scoped_lock lock(m_ClientsLock);
		m_Clients.erase(client);

		Log(LogWarning, "ApiListener")
		    << "Removing API client for endpoint '" << GetName() << "'. " << m_Clients.size() << " API clients left.";
	}

	bool is_master = ApiListener::GetInstance()->IsMaster();

	if (was_master != is_master)
		ApiListener::OnMasterChanged(is_master);

	OnDisconnected(this, client);
}

std::set<ApiClient::Ptr> Endpoint::GetClients(void) const
{
	boost::mutex::scoped_lock lock(m_ClientsLock);
	return m_Clients;
}

Zone::Ptr Endpoint::GetZone(void) const
{
	return m_Zone;
}

bool Endpoint::IsConnected(void) const
{
	boost::mutex::scoped_lock lock(m_ClientsLock);
	return !m_Clients.empty();
}

Endpoint::Ptr Endpoint::GetLocalEndpoint(void)
{
	ApiListener::Ptr listener = ApiListener::GetInstance();

	if (!listener)
		return Endpoint::Ptr();

	return Endpoint::GetByName(listener->GetIdentity());
}
