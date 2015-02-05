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

#ifndef SYSLOGLOGGER_H
#define SYSLOGLOGGER_H

#ifndef _WIN32
#include "base/i2-base.hpp"
#include "base/sysloglogger.thpp"

namespace icinga
{

/**
 * A logger that logs to syslog.
 *
 * @ingroup base
 */
class I2_BASE_API SyslogLogger : public ObjectImpl<SyslogLogger>
{
public:
	DECLARE_OBJECT(SyslogLogger);
	DECLARE_OBJECTNAME(SyslogLogger);

	static Value StatsFunc(Dictionary::Ptr& status, Array::Ptr& perfdata);

protected:
	virtual void ProcessLogEntry(const LogEntry& entry);
	virtual void Flush(void);
};

}
#endif /* _WIN32 */

#endif /* SYSLOGLOGGER_H */
