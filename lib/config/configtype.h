/******************************************************************************
 * Icinga 2                                                                   *
 * Copyright (C) 2012-2013 Icinga Development Team (http://www.icinga.org/)   *
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

#ifndef CONFIGTYPE_H
#define CONFIGTYPE_H

#include "config/i2-config.h"
#include "config/typerule.h"
#include "config/typerulelist.h"
#include "config/configitem.h"
#include "base/array.h"
#include "base/registry.h"

namespace icinga
{

/**
 * A configuration type. Used to validate config objects.
 *
 * @ingroup config
 */
class I2_CONFIG_API ConfigType : public Object {
public:
	DECLARE_PTR_TYPEDEFS(ConfigType);

	ConfigType(const String& name, const DebugInfo& debuginfo);

	String GetName(void) const;

	String GetParent(void) const;
	void SetParent(const String& parent);

	TypeRuleList::Ptr GetRuleList(void) const;

	DebugInfo GetDebugInfo(void) const;

	void ValidateItem(const ConfigItem::Ptr& object);

	void Register(void);
	static ConfigType::Ptr GetByName(const String& name);
	static Registry<ConfigType, ConfigType::Ptr>::ItemMap GetTypes(void);
	static void DiscardTypes(void);

private:
	String m_Name; /**< The type name. */
	String m_Parent; /**< The parent type. */

	TypeRuleList::Ptr m_RuleList;
	DebugInfo m_DebugInfo; /**< Debug information. */

	static void ValidateDictionary(const Dictionary::Ptr& dictionary,
	    const std::vector<TypeRuleList::Ptr>& ruleLists, std::vector<String>& locations);
	static void ValidateArray(const Array::Ptr& array,
	    const std::vector<TypeRuleList::Ptr>& ruleLists, std::vector<String>& locations);

	static String LocationToString(const std::vector<String>& locations);

	static void AddParentRules(std::vector<TypeRuleList::Ptr>& ruleLists, const ConfigType::Ptr& item);
};

}

#endif /* CONFIGTYPE_H */
