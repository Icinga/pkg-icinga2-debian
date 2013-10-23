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

#ifndef OBJECTLOCK_H
#define OBJECTLOCK_H

#include "base/object.h"

namespace icinga
{

/**
 * A scoped lock for Objects.
 */
struct I2_BASE_API ObjectLock {
public:
	ObjectLock(void);
	ObjectLock(const Object::Ptr& object);
	ObjectLock(const Object *object);
	~ObjectLock(void);

	void Lock(void);
	void Unlock(void);

private:
	const Object *m_Object;
	Object::MutexType::scoped_lock m_Lock;
};

}

#endif /* OBJECTLOCK_H */
