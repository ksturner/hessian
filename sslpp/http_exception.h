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
#ifndef SSLPP_EXCEPTION_H
#define SSLPP_EXCEPTION_H

#include <string>
#include <exception>

using namespace std;

namespace sslpp {
	namespace exceptions {
		/// http_exception class.
		class http_exception : public std::exception {
		public:
			/// Constructor.
			explicit http_exception(const string& what);
			/// Destructor.
			virtual ~http_exception() throw();
			/// Returns a C-style character string describing the general cause of the current error.
			virtual const char* what() const throw();
			virtual void raise() const;
		private:
			string _message;
		};
	}
}

#endif

