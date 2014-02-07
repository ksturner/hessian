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
#include "hessian_proxy.h"

#include <memory>
#include <sstream>
#include <cassert>
using namespace std;

#include "string_input_stream.h"
#include "zlibdec.h"
using namespace hessian;

namespace hessian {
#define GZIP_MAGICK 0x8b1f

	unsigned long long hessian_proxy::_bytes_in = 0;
	unsigned long long hessian_proxy::_bytes_out = 0;
	unsigned int hessian_proxy::_num_calls = 0;
	map<string, unsigned int> hessian_proxy::_call_size_map;
	const string HESSIAN_HTTP_CONTENT_TYPE("application/binary");
	const string HESSIAN_HTTP_USER_AGENT("hessian cpp");

	hessian_proxy::hessian_proxy(const string& hostspec, const string& url, bool use_ssl): _hostspec(hostspec), _url(url), _use_ssl(use_ssl) {
		;
	}
	hessian_proxy::~hessian_proxy() {
		;
	}

	Object* hessian_proxy::call(const string& method, int argc, ...) throw(io_exception, http_exception) {
		va_list ap;
		int narg = 0;

		// result, connection, hessian output
		Object* result = NULL;
		sslpp::http_connection con(_hostspec, _use_ssl);
		hessian_output hout;

		// create method call
		string mc = hout.start_call(method);

		// add parameters
		va_start(ap, argc);
		while (narg++ < argc) {
			Object* param = va_arg(ap, Object*);
			hout.set_parameter(mc, param);
		}
		// clean up vararg
		va_end(ap);
		// finish method call
		hout.complete_call(mc);

		// call
		_num_calls++;
		_bytes_out += mc.length();
		string raw_reply = con.POST(_url, mc, HESSIAN_HTTP_CONTENT_TYPE, HESSIAN_HTTP_USER_AGENT);
		string hessian_reply = con.parse_reply(raw_reply);
		_bytes_in += hessian_reply.length();
		_call_size_map.insert(std::make_pair(method, hessian_reply.length()));
		// test for compressed answer
		unsigned short header = ((unsigned short)hessian_reply[1]) << 8;
		header += ((unsigned short)hessian_reply[0]);
		if (header == GZIP_MAGICK) {
			// decompress
			zlibdec zdec;
			try {
				hessian_reply = zdec.decompress(hessian_reply);
			}
			catch (zlib_exception& e) {
				throw io_exception(e.what());
			}
		}
		// create a string_input_stream around the reply; note use of auto_ptr
		auto_ptr<input_stream> sis(new string_input_stream(hessian_reply));
		// read reply
		hessian_input hin(sis);
		hin.start_reply();
		result = hin.get_result();
		hin.complete_reply();
		return result;
	}

	string hessian_proxy::hostspec() const {
		return _hostspec;
	}

	string hessian_proxy::url() const {
		return _url;
	}

	bool hessian_proxy::use_ssl() const {
		return _use_ssl;
	}

	string hessian_proxy::to_string() const {
		stringstream ss;
		ss << (_use_ssl ? "https" : "http") << "://" << _hostspec << _url << endl;
		return ss.str();
	}

	unsigned long long hessian_proxy::bytes_in() {
		return _bytes_in;
	}

	unsigned long long hessian_proxy::bytes_out() {
		return _bytes_out;
	}

	unsigned int hessian_proxy::num_calls() {
		return _num_calls;
	}

	map<string, unsigned int> hessian_proxy::call_size_map() {
		return _call_size_map;
	}
}
