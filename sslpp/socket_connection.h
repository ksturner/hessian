// Copyright (C) 2004 Aldratech Ltd. See the LICENSE file for licensing information.
/*
 This file is part of sslpp.

		sslpp is free software; you can redistribute it and/or modify
		it under the terms of the GNU Lesser General Public License as published by
		the Free Software Foundation; either version 2.1 of the License, or
		(at your option) any later version.

		sslpp is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
		GNU Lesser General Public License for more details.

		You should have received a copy of the GNU Lesser General Public License
		along with sslpp; if not, write to the Free Software
		Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef SSLPP_SOCKET_CONNECTION
#define SSLPP_SOCKET_CONNECTION

#include "ssl++.h"
#ifdef WIN32
	#include <winsock2.h>
#endif

using namespace sslpp;

namespace sslpp {

	///Plain socket connection.
	class socket_connection : public abstract_connection {
	public:
		/// Constructor.
		/**
		 * @param endpoint The connection string (host:port).
		 */
		socket_connection(const string& endpoint);
		/// Destructor.
		virtual ~socket_connection();

		/// Output operator.
		socket_connection& operator<<(const string& buf);

		/// Input operator.
		socket_connection& operator>>(string& buf);

		/// Read max_size bytes into buf.
		/**
		 * @param max_size Number of bytes to read into buf.
		 * @param buf Pre-allocated buffer to write into.
		 * @return >0 success, number of bytes read; <=0 error
		 */
		int read(int max_size, char* buf);

		/// Write a string to the current connection.
		/**
		 * @param what The string to write.
		 * @return >0 success, number of bytes read; <=0 error
		 */
		int write(const string& what);

		/// Test status.
		bool valid() const {
			return _valid;
		}

		/// Return an error trace.
		string errors();

		/// Return the endpoint.
		string endpoint() const {
			return _endpoint;
		}

	private:
		string _endpoint;
		bool _valid;
#ifdef WIN32
		SOCKET _socket;
#else
		int _socket;
#endif
		bool _host_resolve_error;
		static bool _winsock_initialised;///< Track WinSock init on WIN32.
		///Initialise WinSock on WIN32.
		bool winsock_init();
		int write(const char* data, unsigned int length);
	};
}

#endif

