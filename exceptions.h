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

#ifndef HESSIAN_EXCEPTIONS_H
#define HESSIAN_EXCEPTIONS_H

#include <exception>
#include <string>

#include "wrappers.h"

#ifdef WIN32
	#pragma warning(disable:4290)
#endif

// using namespace hessian::wrappers;

namespace std {}

namespace hessian {
	namespace exceptions {
		using namespace std;

		///io_exception class.
		class io_exception : public exception {
		public:
			///Constructor with error message.
			explicit io_exception(const string& what): _message(what) {
			}
			///Destructor.
			virtual ~io_exception() throw() {
			}
			///Get error message.
			const char* what() const throw() {
				return _message.c_str();
			}
			///Re-throw.
			virtual void raise() const {
				throw *this;
			}
		protected:
			string _message;
		};

		///fault_exception class representing a Hessian Fault.
		class fault_exception : public exception {
		public:
			///Constructor with code and message.
			fault_exception(const wstring& code,
				 const wstring& message)
			: _code(code), _message(message)
			{
				string c=wrappers::String::to_string(_code);
				string m=wrappers::String::to_string(_message);
				_what = string("code ").append(c)
				.append(", message ")
				.append(m);
			}
			fault_exception(const string& code,
				const string& message
				) :
				_code(wrappers::String::to_wstring(code)),
				_message(wrappers::String::to_wstring(message))
			{
				_what = string("code ").append(code)
				.append(", message ")
				.append(message);
			}
			///Constructor from Fault.
			fault_exception(const wrappers::Fault& fault) :
			_code(fault.wcode()), _message(fault.wmessage())
			{
				string c=wrappers::String::to_string(_code);
				string m=wrappers::String::to_string(_message);
				_what = string("code ").append(c)
				.append(", message ")
				.append(m);
			}
			///Destructor.
			virtual ~fault_exception() throw() {
			}
			///Get error message with code and message.
			const char* what() const throw() {
				return _what.c_str(); 
			}
			///Re-throw.
			virtual void raise() const {
				throw *this;
			}
			///Get the Fault code.
			wstring wcode() const throw() {
				return _code;
			}
			wstring wmessage() const throw() {
				return _message;
			}
			string code() const throw() {
				return wrappers::String::to_string(_code);
			}
			///Get the Fault message.
			string message() const throw() {
				return wrappers::String::to_string(_message);
			}
		protected:
			wstring _code;
			wstring _message;
			string _what;
		};
	}
}

#endif
