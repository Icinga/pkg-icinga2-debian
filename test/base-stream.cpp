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

#include "base/stdiostream.hpp"
#include "base/string.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/foreach.hpp>
#include <sstream>

using namespace icinga;

BOOST_AUTO_TEST_SUITE(base_stream)

BOOST_AUTO_TEST_CASE(readline_stdio)
{
	std::stringstream msgbuf;
	msgbuf << "Hello\nWorld\n\n";

	StdioStream::Ptr stdstream = new StdioStream(&msgbuf, false);

	ReadLineContext rlc;

	String line;
	BOOST_CHECK(stdstream->ReadLine(&line, rlc));
	BOOST_CHECK(line == "Hello");

	BOOST_CHECK(stdstream->ReadLine(&line, rlc));
	BOOST_CHECK(line == "World");

	BOOST_CHECK(stdstream->ReadLine(&line, rlc));
	BOOST_CHECK(line == "");

	BOOST_CHECK(stdstream->ReadLine(&line, rlc));
	BOOST_CHECK(line == "");

	stdstream->Close();
}

BOOST_AUTO_TEST_SUITE_END()
