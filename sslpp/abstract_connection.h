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
#ifndef SSLPP_ABSTRACT_CONNECTION_H
#define SSLPP_ABSTRACT_CONNECTION_H

#include <string>
using namespace std;

namespace sslpp {
	///Abstract connection.
	class abstract_connection {
	public:
		/// Destructor.
		virtual ~abstract_connection() {
			;
		}

		/// Output operator.
		virtual abstract_connection& operator<<(const string& buf) = 0;

		/// Input operator.
		virtual abstract_connection& operator>>(string& buf) = 0;

		/// Read max_size bytes into buf.
		/**
		 * @param max_size Number of bytes to read into buf.
		 * @param buf Pre-allocated buffer to write into.
		 * @return >0 success, number of bytes read; <=0 error
		 */
		virtual int read(int max_size, char* buf) = 0;

		/// Write a string to the current connection.
		/**
		 * @param what The string to write.
		 * @return >0 success, number of bytes read; <=0 error
		 */
		virtual int write(const string& what) = 0;

		/// Test status.
		virtual bool valid() const = 0;

		/// Return an error trace.
		virtual string errors() = 0;

		/// Return the endpoint.
		virtual string endpoint() const = 0;
	};
}

#endif

