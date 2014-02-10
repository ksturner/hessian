// Copyright (C) 2004 Aldratech Ltd. See the LICENSE file for licensing information.
// Copyright (C) 2002 Ilya Volynets, Total Knowledge.
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
/*
	This file is not an original work, but contains work developed by
	Ilya Volynets and sponsored by Total Knowledge.
	Please see the file ORIGINAL.COPYRIGHT for the original license
	under which the original code was distributed.
*/
#ifndef SSL_CONNECTION_H
#define SSL_CONNECTION_H

#include "abstract_connection.h"
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <string>
using namespace std;

namespace sslpp {

	///SSL socket connection.
	class ssl_connection : public abstract_connection {
	public:
		/// Constructor.
		/**
		 * @param endpoint The connection string (host:port).
		 */
		ssl_connection(const string& endpoint);
		/// Destructor.
		virtual ~ssl_connection();

		/// Output operator.
		ssl_connection& operator<<(const string& buf);

		/// Input operator.
		ssl_connection& operator>>(string& buf);

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
		private:
		string _endpoint;
		SSL* connection;
		BIO* bio;
		bool _valid;
		static bool _openssl_initialised;///< Track OpenSSL init.
		static SSL_CTX* ctx;
		/// Initialise OpenSSL (once!).
		void openssl_init();
		/// Establish a client SSL connection.
		bool connect(const string& conn);
	};
};

#endif
