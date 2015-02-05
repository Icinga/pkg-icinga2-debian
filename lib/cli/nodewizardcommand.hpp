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

#ifndef AGENTWIZARDCOMMAND_H
#define AGENTWIZARDCOMMAND_H

#include "cli/clicommand.hpp"

namespace icinga
{

/**
 * The "agent wizard" command.
 *
 * @ingroup cli
 */
class NodeWizardCommand : public CLICommand
{
public:
	DECLARE_PTR_TYPEDEFS(NodeWizardCommand);

	virtual String GetDescription(void) const;
	virtual String GetShortDescription(void) const;
	virtual int GetMaxArguments(void) const;
	virtual int Run(const boost::program_options::variables_map& vm, const std::vector<std::string>& ap) const;
	virtual ImpersonationLevel GetImpersonationLevel(void) const;
};

}

#endif /* AGENTWIZARDCOMMAND_H */
