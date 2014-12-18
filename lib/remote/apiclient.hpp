/******************************************************************************
 * Icinga 2                                                                   *
 * Copyright (C) 2012-2014 Icinga Development Team (http://www.icinga.org)    *
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

#ifndef APICLIENT_H
#define APICLIENT_H

#include "remote/endpoint.hpp"
#include "base/tlsstream.hpp"
#include "base/timer.hpp"
#include "base/workqueue.hpp"
#include "remote/i2-remote.hpp"

namespace icinga
{

enum ClientRole
{
	ClientInbound,
	ClientOutbound
};

struct MessageOrigin;

/**
 * An API client connection.
 *
 * @ingroup remote
 */
class I2_REMOTE_API ApiClient : public Object
{
public:
	DECLARE_PTR_TYPEDEFS(ApiClient);

	ApiClient(const String& identity, bool authenticated, const TlsStream::Ptr& stream, ConnectionRole role);

	void Start(void);

	String GetIdentity(void) const;
	bool IsAuthenticated(void) const;
	Endpoint::Ptr GetEndpoint(void) const;
	TlsStream::Ptr GetStream(void) const;
	ConnectionRole GetRole(void) const;

	void Disconnect(void);
	void DisconnectSync(void);

	void SendMessage(const Dictionary::Ptr& request);

	static void HeartbeatTimerHandler(void);
	static Value HeartbeatAPIHandler(const MessageOrigin& origin, const Dictionary::Ptr& params);

private:
	String m_Identity;
	bool m_Authenticated;
	Endpoint::Ptr m_Endpoint;
	TlsStream::Ptr m_Stream;
	ConnectionRole m_Role;
	double m_Seen;
	double m_NextHeartbeat;

	WorkQueue m_WriteQueue;

	bool ProcessMessage(void);
	void MessageThreadProc(void);
	void SendMessageSync(const Dictionary::Ptr& request);
};

}

#endif /* APICLIENT_H */
