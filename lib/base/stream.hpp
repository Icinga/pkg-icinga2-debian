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

#ifndef STREAM_H
#define STREAM_H

#include "base/i2-base.hpp"
#include "base/object.hpp"
#include <boost/signals2.hpp>

namespace icinga
{

class String;
class Stream;

enum ConnectionRole
{
	RoleClient,
	RoleServer
};

struct StreamReadContext
{
	StreamReadContext(bool wait = true)
		: Buffer(NULL), Size(0), MustRead(true), Eof(false), Wait(wait)
	{ }

	~StreamReadContext(void)
	{
		free(Buffer);
	}

	bool FillFromStream(const intrusive_ptr<Stream>& stream);
	void DropData(size_t count);

	char *Buffer;
	size_t Size;
	bool MustRead;
	bool Eof;
	bool Wait;
};

enum StreamReadStatus
{
	StatusNewItem,
	StatusNeedData,
	StatusEof
};

/**
 * A stream.
 *
 * @ingroup base
 */
class I2_BASE_API Stream : public Object
{
public:
	DECLARE_PTR_TYPEDEFS(Stream);

	/**
	 * Reads data from the stream.
	 *
	 * @param buffer The buffer where data should be stored. May be NULL if you're
	 *		 not actually interested in the data.
	 * @param count The number of bytes to read from the queue.
	 * @param allow_partial Whether to allow partial reads.
	 * @returns The number of bytes actually read.
	 */
	virtual size_t Read(void *buffer, size_t count, bool allow_partial = false) = 0;

	/**
	 * Writes data to the stream.
	 *
	 * @param buffer The data that is to be written.
	 * @param count The number of bytes to write.
	 * @returns The number of bytes written
	 */
	virtual void Write(const void *buffer, size_t count) = 0;

	/**
	 * Closes the stream and releases resources.
	 */
	virtual void Close(void) = 0;

	/**
	 * Checks whether we've reached the end-of-file condition.
	 *
	 * @returns true if EOF.
	 */
	virtual bool IsEof(void) const = 0;

	/**
	 * Waits until data can be read from the stream.
	 */
	void WaitForData(void);

	virtual bool SupportsWaiting(void) const;

	virtual bool IsDataAvailable(void) const;

	void RegisterDataHandler(const boost::function<void(void)>& handler);

	StreamReadStatus ReadLine(String *line, StreamReadContext& context);

protected:
	void SignalDataAvailable(void);

private:
	boost::signals2::signal<void(void)> OnDataAvailable;

	boost::mutex m_Mutex;
	boost::condition_variable m_CV;
};

}

#endif /* STREAM_H */
