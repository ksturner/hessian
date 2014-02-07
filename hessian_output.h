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

#ifndef HESSIAN_HESSIAN_OUTPUT_H
#define HESSIAN_HESSIAN_OUTPUT_H

#include "exceptions.h"
#include "wrappers.h"

#include <string>

namespace std {}

namespace hessian {
	using namespace hessian::exceptions;
	using namespace std;

	///Creates output serialisation for Hessian requests.
	class hessian_output {
	public:
		///Constructor.
		hessian_output();
		///Destructor.
		~hessian_output();

		///Start a Hessian method call.
		string start_call(const string& method_name);

		///Finish a Hessian method call.
		string& complete_call(string& call);

		///Set a new parameter for the method call; calls writeObject().
		string& set_parameter(string& call, wrappers::Object* object);
	protected:
		///Write a Binary to the call.
		string& write_binary(string& call, wrappers::Binary* object);

		///Write a Boolean to the call.
		string& write_boolean(string& call, wrappers::Boolean* object);

		///Write a Date to the call.
		string& write_date(string& call, wrappers::Date* object);

		///Write a Double to the call.
		string& write_double(string& call, wrappers::Double* object);

		///Write a 32 bit Integer to the call.
		string& write_integer(string& call, wrappers::Integer* object);

		///Write a List to the call.
		string& write_list(string& call, wrappers::List* object);

		///Write a 64 bit Long to the call.
		string& write_long(string& call, wrappers::Long* object);

		///Write a Map to the call.
		string& write_map(string& call, wrappers::Map* object);

		///Write a Null to the call.
		string& write_null(string& call, wrappers::Null* object);

		///Write an Object to the call.
		string& write_object(string& call, wrappers::Object* object);

		///Write a Ref to the call.
		string& write_ref(string& call, wrappers::Ref* object);

		///Write a Remote to the call.
		string& write_remote(string& call, wrappers::Remote* object);

		///Write a String to the call.
		string& write_string(string& call, wrappers::String* object);

		///Write an Xml to the call.
		string& write_xml(string& call, wrappers::Xml* object);

	protected:
		string& write_long64(string& call, int64_t value);
		string& write_ascii_string(string& call,
			const string& ascii_str, char last);
		string& write_string(string& call, const wstring& value,
			char more, char last);
		string& write_string16(string& call, const uint16_t buf[],
			int len, char more, char last);
		int write_wchar(string& call, wchar_t wch);
		int write_utf16_char(string& call, uint16_t u16);
		string& write_fault(string& call, wrappers::Fault* f_obj);
	public:
		string& write_call_with_version(string& call, int version);
		string& write_reply_with_version(string& call, int version);
		string& write_header(string& call, const string& hd,
			wrappers::Object* obj);
		string& complete_reply(string& call);
		string& write_method_name(string& call, const string& name);
                string& write_length16(string& call, int value);

		string& write_null(string& call)
		{
			return call.append(1, 'N');
		}

		string& write_bool(string& call, const bool b)
		{
			return call.append(1, b ? 'T' : 'F');
		}

		string& write_date_milli(string& call, int64_t milli_epoch)
		{
			return write_long64(call.append(1,'d'), milli_epoch);
		}

		string& write_double(string& call, double d_val)
		{
			int64_t* addr = (int64_t*) (&d_val);
			return write_long64(call.append(1,'D'), *addr);
		}

		string& write_integer32(string& call, int32_t value);
		string& write_integer64(string& call, int64_t value);
		string& write_binary_image(string& call,const string& bin_img); 

		string& write_native_string(string& call,const string& mb_str);
		string& write_wide_string(string& call, const wstring& w_str);
		string& write_utf8_string(string& call,const string& utf8_str);
		string& write_utf16_string(string& call, const uint16_t buf[],
			int len);

		string& write_native_xml(string& call,const string& mb_xml);
		string& write_wide_xml(string& call, const wstring& w_xml);
		string& write_utf8_xml(string& call,const string& utf8_xml);
		string& write_utf16_xml(string& call, const uint16_t buf[],
			int len);
	};
}

#endif
