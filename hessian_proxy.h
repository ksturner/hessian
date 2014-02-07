// Copyright (C) 2004 Aldratech Ltd. See the LICENSE file for licensing information.
/*
 This file is part of hessiancpp.

    hessiancpp is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    hessiancpp is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with hessiancpp; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef HESSIAN_HESSIAN_PROXY_H
#define HESSIAN_HESSIAN_PROXY_H

#include <string>
#include <map>

#include "exceptions.h"
#include "hessian_input.h"
#include "hessian_output.h"
#include "wrappers.h"
#include "input_stream.h"

#include <ssl++.h>

using namespace std;
using namespace hessian::wrappers;
using namespace hessian::exceptions;
using namespace sslpp::exceptions;

namespace hessian {
	///Hessian method invocation proxy.
	/**
	 * The Hessian proxy implements method calls taking
	 * care of parameters and result de-serialisation.
	 * Clients should use this class instead of
	 * hessian_input and hessian_output.
	 */
	class hessian_proxy {
	public:
		///Constructor.
		/**
		 * @param hostspec The host to connect to in HOST:PORT format.SSL only.
		 * @param url The Hessian endpoint URL relative to the root /.
		 * @param use_ssl Whether the connection should use SSL.
		 */
		hessian_proxy(const string& hostspec, const string& url, bool use_ssl);
		///Destructor.
		~hessian_proxy();
		///Call a method on the proxy.
		/**
		 * @param method The method name.
		 * @param argc The number of parameters passed.
		 * @param ... The parameters.
		 * @return An Object* containing the server reply.
		 * @throws io_exception from the Hessian protocol level.
		 * @throws ssl_exception from the HTTPS connection level.
		 */
		Object* call(const string& method, int argc, ...) throw (io_exception, http_exception);
		///Get the hostspec.
		string hostspec() const;
		///Get the hessian endpoint URL.
		string url() const;
		///Get SSL usage.
		bool use_ssl() const;
		///Return a string representation of this proxy.
		string to_string() const;
		///Get the number of bytes received by all instances. 
		static unsigned long long bytes_in();
		///Get the number of bytes sent by all instances.
		static unsigned long long bytes_out();
		///Get the number of calls made by all instances.
		static unsigned int num_calls();
		///Get the call size map with method name and return size.
		static map<string, unsigned int> call_size_map();
	private:
		string _hostspec;
		string _url;
		bool _use_ssl;
		static unsigned long long _bytes_in;
		static unsigned long long _bytes_out;
		static unsigned int _num_calls;
		static map<string, unsigned int> _call_size_map;
	};
}

#endif

