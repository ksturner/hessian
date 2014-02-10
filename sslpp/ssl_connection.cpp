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
#include "ssl++.h"
#include <openssl/err.h>
#include <iostream>

using namespace std;
using namespace sslpp;

namespace sslpp {

	bool ssl_connection::_openssl_initialised = false;
	SSL_CTX* ssl_connection::ctx = NULL;

	void ssl_connection::openssl_init() {
		if (_openssl_initialised) {
			// already initialized
			return ;
		}
		SSL_load_error_strings();
		SSL_library_init();
		ctx = SSL_CTX_new(SSLv3_method());
		SSL_CTX_set_options(ctx, SSL_OP_ALL);
		_openssl_initialised = true;
	}

	string ssl_connection::errors() {
	#define __ERR_SIZE 1024
		unsigned long i;
		string trace;
		char buf[__ERR_SIZE];
		buf[__ERR_SIZE - 1] = 0;
		// get all errors in this tread
		while ((i = ERR_get_error())) {
			ERR_error_string_n(i, buf, __ERR_SIZE - 1);
			trace.append(buf);
			trace.append(1, ' ');
		}
		return trace;
	}

	ssl_connection::ssl_connection(const string& endpoint) : _endpoint(endpoint) {
		openssl_init();
		connection = SSL_new(ctx);
		_valid = (bool)connection;
		if (!_valid) {
			cerr << "ssl_connection::ctor(): SSL_new failed" << endl;
			return ;
		}
		if (SSL_set_cipher_list(connection, "ALL") <= 0) {
			_valid = false;
			return ;
		}

		_valid = connect(_endpoint);
	}

	ssl_connection::~ssl_connection() {
		SSL_free(connection);
	}

	bool ssl_connection::connect(const std::string& conn) {
		bio = BIO_new_connect((char*)conn.c_str());
		if (BIO_do_connect(bio) <= 0) {
			cerr << "ssl_connection::connect(): BIO_do_connect failed" << endl;
			return false;
		}
		SSL_set_bio(connection, bio, bio);
		SSL_set_connect_state(connection);
		if (SSL_connect(connection) <= 0) {
			return false;
		}
		return true;
	}


	ssl_connection& ssl_connection::operator<<(const std::string& buf) {
		SSL_write(connection, buf.c_str(), (int)buf.length());
		return *this;
	}

	ssl_connection& ssl_connection::operator>>(std::string& buf) {
	#define __READ_CHUNK_SIZE 1024
		char str[__READ_CHUNK_SIZE];
		int ret;
		buf = "";
		while ((ret = SSL_read(connection, str, sizeof(str) - 1)) > 0) {
			str[ret] = 0;
			buf.append(str, ret);
		}
		int errcode = SSL_get_error(connection, ret);
		if (errcode != SSL_ERROR_ZERO_RETURN) {
			cout << "ssl_connection::operator>>: error reading" << endl;
		}
		return *this;
	}

	int ssl_connection::read(int max_size, char* buf) {
		int ret = SSL_read(connection, buf, max_size - 1);
		return ret;
	}

	int ssl_connection::write(const std::string& what) {
		return SSL_write(connection, what.c_str(), (int)what.length());
	}
}
