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

#ifndef SERVICEGROUP_H
#define SERVICEGROUP_H

#include "icinga/i2-icinga.hpp"
#include "icinga/servicegroup.thpp"
#include "icinga/service.hpp"

namespace icinga
{

class ObjectRule;

/**
 * An Icinga service group.
 *
 * @ingroup icinga
 */
class I2_ICINGA_API ServiceGroup : public ObjectImpl<ServiceGroup>
{
public:
	DECLARE_OBJECT(ServiceGroup);
	DECLARE_OBJECTNAME(ServiceGroup);

	std::set<Service::Ptr> GetMembers(void) const;
	void AddMember(const Service::Ptr& service);
	void RemoveMember(const Service::Ptr& service);

	bool ResolveGroupMembership(const Service::Ptr& service, bool add = true, int rstack = 0);

	static void RegisterObjectRuleHandler(void);

private:
	mutable boost::mutex m_ServiceGroupMutex;
	std::set<Service::Ptr> m_Members;

	static bool EvaluateObjectRuleOne(const Service::Ptr& service, const ObjectRule& rule);
	static void EvaluateObjectRule(const ObjectRule& rule);
	static void EvaluateObjectRules(const std::vector<ObjectRule>& rules);
};

}

#endif /* SERVICEGROUP_H */
