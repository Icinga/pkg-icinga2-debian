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

#ifndef OBJECT_H
#define OBJECT_H

#include "base/i2-base.hpp"
#include "base/debug.hpp"
#include "base/thinmutex.hpp"
#include <boost/thread/thread.hpp>

#ifndef _DEBUG
#include <boost/thread/mutex.hpp>
#else /* _DEBUG */
#include <boost/thread/recursive_mutex.hpp>
#endif /* _DEBUG */

#include <boost/smart_ptr/intrusive_ptr.hpp>

using boost::intrusive_ptr;
using boost::dynamic_pointer_cast;
using boost::static_pointer_cast;

#include <boost/tuple/tuple.hpp>
using boost::tie;

namespace icinga
{

class Value;
class Object;
class Type;

#define DECLARE_PTR_TYPEDEFS(klass) \
	typedef intrusive_ptr<klass> Ptr

#define IMPL_TYPE_LOOKUP(klass) 					\
	static intrusive_ptr<Type> TypeInstance;			\
	virtual intrusive_ptr<Type> GetReflectionType(void) const	\
	{								\
		return TypeInstance;					\
	}

#define DECLARE_OBJECT(klass) \
	DECLARE_PTR_TYPEDEFS(klass); \
	IMPL_TYPE_LOOKUP(klass);

template<typename T>
intrusive_ptr<Object> DefaultObjectFactory(void)
{
	return new T();
}

typedef intrusive_ptr<Object> (*ObjectFactory)(void);

template<typename T>
struct TypeHelper
{
	static ObjectFactory GetFactory(void)
	{
		return DefaultObjectFactory<T>;
	}
};

/**
 * Base class for all heap-allocated objects. At least one of its methods
 * has to be virtual for RTTI to work.
 *
 * @ingroup base
 */
class I2_BASE_API Object
{
public:
	DECLARE_OBJECT(Object);

	Object(void);
	virtual ~Object(void);

	virtual void SetField(int id, const Value& value);
	virtual Value GetField(int id) const;

#ifdef _DEBUG
	bool OwnsLock(void) const;
#endif /* _DEBUG */

	void InflateMutex(void);

private:
	Object(const Object& other);
	Object& operator=(const Object& rhs);

	uintptr_t m_References;
	mutable ThinMutex m_Mutex;

#ifdef _DEBUG
#	ifndef _WIN32
	mutable pthread_t m_LockOwner;
#	else /* _WIN32 */
	mutable DWORD m_LockOwner;
#	endif /* _WIN32 */
#endif /* _DEBUG */

	friend struct ObjectLock;

	friend void intrusive_ptr_add_ref(Object *object);
	friend void intrusive_ptr_release(Object *object);
};

inline void intrusive_ptr_add_ref(Object *object)
{
#ifdef _WIN32
	InterlockedIncrement(&object->m_References);
#else /* _WIN32 */
	__sync_add_and_fetch(&object->m_References, 1);
#endif /* _WIN32 */
}

inline void intrusive_ptr_release(Object *object)
{
	uintptr_t refs;
#ifdef _WIN32
	refs = InterlockedDecrement(&object->m_References);
#else /* _WIN32 */
	refs = __sync_sub_and_fetch(&object->m_References, 1);
#endif /* _WIN32 */

	if (refs == 0)
		delete object;
}

template<typename T>
class ObjectImpl
{
};

}

#endif /* OBJECT_H */

#include "base/type.hpp"
