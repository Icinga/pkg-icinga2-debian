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

#ifndef SCRIPTFUNCTIONWRAPPER_H
#define SCRIPTFUNCTIONWRAPPER_H

#include "base/i2-base.hpp"
#include "base/value.hpp"
#include <vector>
#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace icinga
{

Value ScriptFunctionWrapperVV(void (*function)(void), const std::vector<Value>& arguments);
Value ScriptFunctionWrapperVA(void (*function)(const std::vector<Value>&), const std::vector<Value>& arguments);

boost::function<Value (const std::vector<Value>& arguments)> I2_BASE_API WrapScriptFunction(void (*function)(void));

template<typename TR>
Value ScriptFunctionWrapperR(TR (*function)(void), const std::vector<Value>&)
{
	return function();
}

template<typename TR>
boost::function<Value (const std::vector<Value>& arguments)> WrapScriptFunction(TR (*function)(void))
{
	return boost::bind(&ScriptFunctionWrapperR<TR>, function, _1);
}

template<typename T0>
Value ScriptFunctionWrapperV(void (*function)(T0), const std::vector<Value>& arguments)
{
	if (arguments.size() < 1)
		BOOST_THROW_EXCEPTION(std::invalid_argument("Too few arguments for function."));

	function(static_cast<T0>(arguments[0]));

	return Empty;
}

template<typename T0>
boost::function<Value (const std::vector<Value>& arguments)> WrapScriptFunction(void (*function)(T0))
{
	return boost::bind(&ScriptFunctionWrapperV<T0>, function, _1);
}

template<typename TR, typename T0>
Value ScriptFunctionWrapperR(TR (*function)(T0), const std::vector<Value>& arguments)
{
	if (arguments.size() < 1)
		BOOST_THROW_EXCEPTION(std::invalid_argument("Too few arguments for function."));

	return function(static_cast<T0>(arguments[0]));
}

template<typename TR, typename T0>
boost::function<Value (const std::vector<Value>& arguments)> WrapScriptFunction(TR (*function)(T0))
{
	return boost::bind(&ScriptFunctionWrapperR<TR, T0>, function, _1);
}

template<typename T0, typename T1>
Value ScriptFunctionWrapperV(void (*function)(T0, T1), const std::vector<Value>& arguments)
{
	if (arguments.size() < 2)
		BOOST_THROW_EXCEPTION(std::invalid_argument("Too few arguments for function."));

	function(static_cast<T0>(arguments[0]),
	    static_cast<T1>(arguments[1]));

	return Empty;
}

template<typename T0, typename T1>
boost::function<Value (const std::vector<Value>& arguments)> WrapScriptFunction(void (*function)(T0, T1))
{
	return boost::bind(&ScriptFunctionWrapperV<T0, T1>, function, _1);
}

template<typename TR, typename T0, typename T1>
Value ScriptFunctionWrapperR(TR (*function)(T0, T1), const std::vector<Value>& arguments)
{
	if (arguments.size() < 2)
		BOOST_THROW_EXCEPTION(std::invalid_argument("Too few arguments for function."));

	return function(static_cast<T0>(arguments[0]),
	    static_cast<T1>(arguments[1]));
}

template<typename TR, typename T0, typename T1>
boost::function<Value (const std::vector<Value>& arguments)> WrapScriptFunction(TR (*function)(T0, T1))
{
	return boost::bind(&ScriptFunctionWrapperR<TR, T0, T1>, function, _1);
}

template<typename T0, typename T1, typename T2>
Value ScriptFunctionWrapperV(void (*function)(T0, T1, T2), const std::vector<Value>& arguments)
{
	if (arguments.size() < 3)
		BOOST_THROW_EXCEPTION(std::invalid_argument("Too few arguments for function."));

	function(static_cast<T0>(arguments[0]),
	    static_cast<T1>(arguments[1]),
	    static_cast<T2>(arguments[2]));

	return Empty;
}

template<typename T0, typename T1, typename T2>
boost::function<Value (const std::vector<Value>& arguments)> WrapScriptFunction(void (*function)(T0, T1, T2))
{
	return boost::bind(&ScriptFunctionWrapperV<T0, T1, T2>, function, _1);
}

template<typename TR, typename T0, typename T1, typename T2>
Value ScriptFunctionWrapperR(TR (*function)(T0, T1, T2), const std::vector<Value>& arguments)
{
	if (arguments.size() < 3)
		BOOST_THROW_EXCEPTION(std::invalid_argument("Too few arguments for function."));

	return function(static_cast<T0>(arguments[0]),
	    static_cast<T1>(arguments[1]),
	    static_cast<T2>(arguments[2]));
}

template<typename TR, typename T0, typename T1, typename T2>
boost::function<Value (const std::vector<Value>& arguments)> WrapScriptFunction(TR (*function)(T0, T1, T2))
{
	return boost::bind(&ScriptFunctionWrapperR<TR, T0, T1, T2>, function, _1);
}

template<typename T0, typename T1, typename T2, typename T3>
Value ScriptFunctionWrapperV(void (*function)(T0, T1, T2, T3), const std::vector<Value>& arguments)
{
	if (arguments.size() < 4)
		BOOST_THROW_EXCEPTION(std::invalid_argument("Too few arguments for function."));

	function(static_cast<T0>(arguments[0]),
	    static_cast<T1>(arguments[1]),
	    static_cast<T2>(arguments[2]),
	    static_cast<T3>(arguments[3]));

	return Empty;
}

template<typename T0, typename T1, typename T2, typename T3>
boost::function<Value (const std::vector<Value>& arguments)> WrapScriptFunction(void (*function)(T0, T1, T2, T3))
{
	return boost::bind(&ScriptFunctionWrapperV<T0, T1, T2, T3>, function, _1);
}

template<typename TR, typename T0, typename T1, typename T2, typename T3>
Value ScriptFunctionWrapperR(TR (*function)(T0, T1, T2, T3), const std::vector<Value>& arguments)
{
	if (arguments.size() < 4)
		BOOST_THROW_EXCEPTION(std::invalid_argument("Too few arguments for function."));

	return function(static_cast<T0>(arguments[0]),
	    static_cast<T1>(arguments[1]),
	    static_cast<T2>(arguments[2]),
	    static_cast<T3>(arguments[3]));
}

template<typename TR, typename T0, typename T1, typename T2, typename T3>
boost::function<Value (const std::vector<Value>& arguments)> WrapScriptFunction(TR (*function)(T0, T1, T2, T3))
{
	return boost::bind(&ScriptFunctionWrapperR<TR, T0, T1, T2, T3>, function, _1);
}

template<typename T0, typename T1, typename T2, typename T3, typename T4>
Value ScriptFunctionWrapperV(void (*function)(T0, T1, T2, T3, T4), const std::vector<Value>& arguments)
{
	if (arguments.size() < 5)
		BOOST_THROW_EXCEPTION(std::invalid_argument("Too few arguments for function."));

	function(static_cast<T0>(arguments[0]),
	    static_cast<T1>(arguments[1]),
	    static_cast<T2>(arguments[2]),
	    static_cast<T3>(arguments[3]),
	    static_cast<T4>(arguments[4]));

	return Empty;
}

template<typename T0, typename T1, typename T2, typename T3, typename T4>
boost::function<Value (const std::vector<Value>& arguments)> WrapScriptFunction(void (*function)(T0, T1, T2, T3, T4))
{
	return boost::bind(&ScriptFunctionWrapperV<T0, T1, T2, T3, T4>, function, _1);
}

template<typename TR, typename T0, typename T1, typename T2, typename T3, typename T4>
Value ScriptFunctionWrapperR(TR (*function)(T0, T1, T2, T3, T4), const std::vector<Value>& arguments)
{
	if (arguments.size() < 5)
		BOOST_THROW_EXCEPTION(std::invalid_argument("Too few arguments for function."));

	return function(static_cast<T0>(arguments[0]),
	    static_cast<T1>(arguments[1]),
	    static_cast<T2>(arguments[2]),
	    static_cast<T3>(arguments[3]),
	    static_cast<T4>(arguments[4]));
}

template<typename TR, typename T0, typename T1, typename T2, typename T3, typename T4>
boost::function<Value (const std::vector<Value>& arguments)> WrapScriptFunction(TR (*function)(T0, T1, T2, T3, T4))
{
	return boost::bind(&ScriptFunctionWrapperR<TR, T0, T1, T2, T3, T4>, function, _1);
}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
Value ScriptFunctionWrapperV(void (*function)(T0, T1, T2, T3, T4, T5), const std::vector<Value>& arguments)
{
	if (arguments.size() < 6)
		BOOST_THROW_EXCEPTION(std::invalid_argument("Too few arguments for function."));

	function(static_cast<T0>(arguments[0]),
	    static_cast<T1>(arguments[1]),
	    static_cast<T2>(arguments[2]),
	    static_cast<T3>(arguments[3]),
	    static_cast<T4>(arguments[4]),
	    static_cast<T5>(arguments[5]));

	return Empty;
}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
boost::function<Value (const std::vector<Value>& arguments)> WrapScriptFunction(void (*function)(T0, T1, T2, T3, T4, T5))
{
	return boost::bind(&ScriptFunctionWrapperV<T0, T1, T2, T3, T4, T5>, function, _1);
}

template<typename TR, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
Value ScriptFunctionWrapperR(TR (*function)(T0, T1, T2, T3, T4, T5), const std::vector<Value>& arguments)
{
	if (arguments.size() < 6)
		BOOST_THROW_EXCEPTION(std::invalid_argument("Too few arguments for function."));

	return function(static_cast<T0>(arguments[0]),
	    static_cast<T1>(arguments[1]),
	    static_cast<T2>(arguments[2]),
	    static_cast<T3>(arguments[3]),
	    static_cast<T4>(arguments[4]),
	    static_cast<T5>(arguments[5]));
}

template<typename TR, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
boost::function<Value (const std::vector<Value>& arguments)> WrapScriptFunction(TR (*function)(T0, T1, T2, T3, T4, T5))
{
	return boost::bind(&ScriptFunctionWrapperR<TR, T0, T1, T2, T3, T4, T5>, function, _1);
}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
Value ScriptFunctionWrapperV(void (*function)(T0, T1, T2, T3, T4, T5, T6), const std::vector<Value>& arguments)
{
	if (arguments.size() < 7)
		BOOST_THROW_EXCEPTION(std::invalid_argument("Too few arguments for function."));

	function(static_cast<T0>(arguments[0]),
	    static_cast<T1>(arguments[1]),
	    static_cast<T2>(arguments[2]),
	    static_cast<T3>(arguments[3]),
	    static_cast<T4>(arguments[4]),
	    static_cast<T5>(arguments[5]),
	    static_cast<T6>(arguments[6]));

	return Empty;
}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
boost::function<Value (const std::vector<Value>& arguments)> WrapScriptFunction(void (*function)(T0, T1, T2, T3, T4, T5, T6))
{
	return boost::bind(&ScriptFunctionWrapperV<T0, T1, T2, T3, T4, T5, T6>, function, _1);
}

template<typename TR, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
Value ScriptFunctionWrapperR(TR (*function)(T0, T1, T2, T3, T4, T5, T6), const std::vector<Value>& arguments)
{
	if (arguments.size() < 7)
		BOOST_THROW_EXCEPTION(std::invalid_argument("Too few arguments for function."));

	return function(static_cast<T0>(arguments[0]),
	    static_cast<T1>(arguments[1]),
	    static_cast<T2>(arguments[2]),
	    static_cast<T3>(arguments[3]),
	    static_cast<T4>(arguments[4]),
	    static_cast<T5>(arguments[5]),
	    static_cast<T6>(arguments[6]));
}

template<typename TR, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
boost::function<Value (const std::vector<Value>& arguments)> WrapScriptFunction(TR (*function)(T0, T1, T2, T3, T4, T5, T6))
{
	return boost::bind(&ScriptFunctionWrapperR<TR, T0, T1, T2, T3, T4, T5, T6>, function, _1);
}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
Value ScriptFunctionWrapperV(void (*function)(T0, T1, T2, T3, T4, T5, T6, T7), const std::vector<Value>& arguments)
{
	if (arguments.size() < 8)
		BOOST_THROW_EXCEPTION(std::invalid_argument("Too few arguments for function."));

	function(static_cast<T0>(arguments[0]),
	    static_cast<T1>(arguments[1]),
	    static_cast<T2>(arguments[2]),
	    static_cast<T3>(arguments[3]),
	    static_cast<T4>(arguments[4]),
	    static_cast<T5>(arguments[5]),
	    static_cast<T6>(arguments[6]),
	    static_cast<T7>(arguments[7]));

	return Empty;
}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
boost::function<Value (const std::vector<Value>& arguments)> WrapScriptFunction(void (*function)(T0, T1, T2, T3, T4, T5, T6, T7))
{
	return boost::bind(&ScriptFunctionWrapperV<T0, T1, T2, T3, T4, T5, T6, T7>, function, _1);
}

template<typename TR, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
Value ScriptFunctionWrapperR(TR (*function)(T0, T1, T2, T3, T4, T5, T6, T7), const std::vector<Value>& arguments)
{
	if (arguments.size() < 8)
		BOOST_THROW_EXCEPTION(std::invalid_argument("Too few arguments for function."));

	return function(static_cast<T0>(arguments[0]),
	    static_cast<T1>(arguments[1]),
	    static_cast<T2>(arguments[2]),
	    static_cast<T3>(arguments[3]),
	    static_cast<T4>(arguments[4]),
	    static_cast<T5>(arguments[5]),
	    static_cast<T6>(arguments[6]),
	    static_cast<T7>(arguments[7]));
}

template<typename TR, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
boost::function<Value (const std::vector<Value>& arguments)> WrapScriptFunction(TR (*function)(T0, T1, T2, T3, T4, T5, T6, T7))
{
	return boost::bind(&ScriptFunctionWrapperR<TR, T0, T1, T2, T3, T4, T5, T6, T7>, function, _1);
}

template<typename TR>
boost::function<TR (const std::vector<Value>& arguments)> WrapScriptFunction(TR (*function)(const std::vector<Value>&))
{
	return boost::bind<TR>(function, _1);
}

inline boost::function<Value (const std::vector<Value>& arguments)> WrapScriptFunction(void (*function)(const std::vector<Value>&))
{
	return boost::bind(&ScriptFunctionWrapperVA, function, _1);
}

}

#endif /* SCRIPTFUNCTION_H */
