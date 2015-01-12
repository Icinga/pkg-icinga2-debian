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

#ifndef TLSSTREAM_H
#define TLSSTREAM_H

#include "base/i2-base.hpp"
#include "base/socket.hpp"
#include "base/stream.hpp"
#include "base/tlsutility.hpp"

namespace icinga
{

/**
 * A TLS stream.
 *
 * @ingroup base
 */
class I2_BASE_API TlsStream : public Stream
{
public:
	DECLARE_PTR_TYPEDEFS(TlsStream);

	TlsStream(const Socket::Ptr& socket, ConnectionRole role, const boost::shared_ptr<SSL_CTX>& sslContext);

	boost::shared_ptr<X509> GetClientCertificate(void) const;
	boost::shared_ptr<X509> GetPeerCertificate(void) const;

	void Handshake(void);

	virtual void Close(void);

	virtual size_t Read(void *buffer, size_t count);
	virtual void Write(const void *buffer, size_t count);

	virtual bool IsEof(void) const;

	bool IsVerifyOK(void) const;

private:
	boost::shared_ptr<SSL> m_SSL;
	bool m_Eof;
	mutable boost::mutex m_SSLLock;
	mutable boost::mutex m_IOActionLock;
	bool m_VerifyOK;

	Socket::Ptr m_Socket;
	ConnectionRole m_Role;

	static int m_SSLIndex;
	static bool m_SSLIndexInitialized;

	void CloseUnlocked(void);

	static int ValidateCertificate(int preverify_ok, X509_STORE_CTX *ctx);
	static void NullCertificateDeleter(X509 *certificate);
};

}

#endif /* TLSSTREAM_H */
