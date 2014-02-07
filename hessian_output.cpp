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

#include "hessian_output.h"
#include <cstring>
#include <iostream>

using namespace hessian::exceptions;
using namespace hessian::wrappers;

namespace hessian {

	hessian_output::hessian_output() {
	}

	hessian_output::~hessian_output() {
	}

	string hessian_output::start_call(const string& method_name) {
		string mc("c");
		mc.append(1, (char)1);
		mc.append(1, (char)0);
		return write_ascii_string(mc, method_name, 'm');
	}

	string& hessian_output::complete_call(string& call) {
		return call.append(1, 'z');
	}

	string& hessian_output::set_parameter(string& call, Object* object) {
		return write_object(call, object);
	}

	string& hessian_output::write_binary(string& call, Binary* object) {
		return write_binary_image(call, object->value());
	}

	string& hessian_output::write_boolean(string& call, Boolean* object) {
		return call.append(1, object->value() ? 'T' : 'F');
	}

	string& hessian_output::write_date(string& call, Date* object) {
		return write_date_milli(call, object->value());
	}

	string& hessian_output::write_double(string& call, Double* object) {
		return write_double(call, object->value());
	}

	string& hessian_output::write_integer(string& call, Integer* object) {
		return write_integer32(call, object->value());
	}

	string& hessian_output::write_list(string& call, List* object) {
		if (object == NULL) {
			return write_null(call, NULL);
		}
		call.append(1, 'V');
		list<Object*> l = object->value();
		for (list<Object*>::iterator i = l.begin(); i != l.end(); i++) {
			write_object(call, *i);
		}
		call.append(1, 'z');
		return call;
	}

	string& hessian_output::write_long(string& call, Long* object) {
		call.append(1, 'L');
		return write_long64(call, object->value());
	}

	string& hessian_output::write_map(string& call, Map* object) {
		call.append(1, 'M');
		Map::basic_type m = object->value();
		for (Map::basic_type::iterator i = m.begin(); i != m.end(); i++) {
			const wstring& wkey =
			String::utf8_to_wstring((*i).first);
			//writeObject(call, const_cast<String*>(&((*i).first)));
			write_string(call, wkey, 's', 'S');
			write_object(call, (*i).second);
		}
		call.append(1, 'z');
		return call;
	}

	string& hessian_output::write_null(string& call, Null* object) {
		call.append(1, 'N');
		return call;
	}

	string& hessian_output::write_long64(string& call, int64_t value) {
		char b64 = (char) (value >> 56);
		char b56 = (char) (value >> 48);
		char b48 = (char) (value >> 40);
		char b40 = (char) (value >> 32);
		char b32 = (char) (value >> 24);
		char b24 = (char) (value >> 16);
		char b16 = (char) (value >> 8);
		char b8  = (char) value;
		call.append(1, b64);
		call.append(1, b56);
		call.append(1, b48);
		call.append(1, b40);
		call.append(1, b32);
		call.append(1, b24);
		call.append(1, b16);
		call.append(1, b8);
		return call;
	}

	string& hessian_output::write_object(string& call, Object* object) {
		const char* cls = object->classname2();
		if (strcmp(cls, "Binary") == 0) {
			return write_binary(call, dynamic_cast<Binary*>(object));
		}
		if (strcmp(cls, "Boolean") == 0) {
			return write_boolean(call, dynamic_cast<Boolean*>(object));
		}
		if (strcmp(cls, "Date") == 0) {
			return write_date(call, dynamic_cast<Date*>(object));
		}
		if (strcmp(cls, "Double") == 0) {
			return write_double(call, dynamic_cast<Double*>(object));
		}
		if (strcmp(cls, "Fault") == 0) {
			return write_fault(call, dynamic_cast<Fault*>(object));
		}
		if (strcmp(cls, "Integer") == 0) {
			return write_integer(call, dynamic_cast<Integer*>(object));
		}
		if (strcmp(cls, "List") == 0) {
			return write_list(call, dynamic_cast<List*>(object));
		}
		if (strcmp(cls, "Long") == 0) {
			return write_long(call, dynamic_cast<Long*>(object));
		}
		if (strcmp(cls, "Map") == 0) {
			return write_map(call, dynamic_cast<Map*>(object));
		}
		if (strcmp(cls, "Null") == 0) {
			return write_null(call, NULL);
		}
		if (strcmp(cls, "Ref") == 0) {
			return write_ref(call, dynamic_cast<Ref*>(object));
		}
		if (strcmp(cls, "Remote") == 0) {
			return write_remote(call, dynamic_cast<Remote*>(object));
		}
		if (strcmp(cls, "String") == 0) {
			return write_string(call, dynamic_cast<String*>(object));
		}
		if (strcmp(cls, "Xml") == 0) {
			return write_xml(call, dynamic_cast<Xml*>(object));
		}
		// throw exception, should not get here, really
		throw io_exception(string("hessian_output::write_object(): unknown object class ").append(object->classname()));
	}

	string& hessian_output::write_ref(string& call, Ref* object) {
		cerr << "write_ref not implemented properly" << endl;
		return write_null(call, NULL);
	}


	string& hessian_output::write_remote(string& call, Remote* object) {
		cerr << "write_ref not implemented properly" << endl;
		return write_null(call, NULL);
	}

	string& hessian_output::write_string(string& call, String* object) {
		return write_string(call, object->wvalue(), 's', 'S'); //
	}

	string& hessian_output::write_xml(string& call, Xml* object) {
		return write_string(call, object->wvalue(), 'x', 'X');
	}

	string& hessian_output::write_call_with_version(string& call,
		int version) {
		char b16 = (char) (version >> 8);
		char b8 = (char) version;
		call.append(1, 'c');
		call.append(1, b16);
		call.append(1, b8);
		return call;
	}

	string& hessian_output::write_reply_with_version(string& call,
		int version) {
		char b16 = (char) (version >> 8);
		char b8 = (char) version;
		call.append(1, 'r');
		call.append(1, b16);
		call.append(1, b8);
		return call;
	}

	string& hessian_output::write_header(string& call, const string& hd,
		Object* obj) {
		write_ascii_string(call, hd, 'H');
		return write_object(call, obj);
	}

	string& hessian_output::write_method_name(string& call,
		const string& name) {
		return write_ascii_string(call, name, 'm');
	}

	string& hessian_output::complete_reply(string& call) {
		return call.append(1, 'z');
	}

  int hessian_output::write_utf16_char(string& call, uint16_t wch)
  {
    const uint16_t replacement_marker = 0xfffd;
    const uint16_t word_joiner = 0x2060;
    const uint16_t bom = 0xfeef;
    if ((wch == 0xfffe) || (wch == 0xffff)) wch=replacement_marker;
    else if (wch == bom) wch = word_joiner;
    // emit CESU-8 chars
    if (wch < 0x80)
    {
      call.append(1, (char) wch);
      return 1; // 1 UTF-16 char, 1 CESU-8 char
    }
    else if (wch < 0x800)
    {
      call.append(1, (char) ((wch >> 6) | 0xc0));
      call.append(1, (char) ((wch & 0x3f) | 0x80));
      return 1; // 1 UTF-16 char, 2 CESU-8 chars
    }
    else
    {
      call.append(1, (char) ((wch >> 12) | 0xe0));
      call.append(1, (char) (((wch >> 6) & 0x3f) | 0x80));
      call.append(1, (char) ((wch & 0x3f) | 0x80));
      return 1; // 1 UTF-16 char, 3 CESU-8 chars
    }
  }

  int hessian_output::write_wchar(string& call, wchar_t wch)
  {
    const wchar_t replacement_marker = 0xfffd;
    const wchar_t word_joiner = 0x2060;
    const wchar_t bom = 0xfeef;
    if (wch >= 0xd800)
    {
      if (wch > 0x10ffff) wch = replacement_marker; // too large
      else if (wch <= 0xdfff) wch = replacement_marker; // bad surrogate
      else if ((wch == 0xfffe) || (wch == 0xffff)) wch=replacement_marker;
      else if (wch == bom) wch = word_joiner;
    }
    // emit CESU-8 chars
    if (wch < 0x80)
    {
      call.append(1, (char) wch);
      return 1; // 1 UTF-16 char, 1 CESU-8 char
    }
    else if (wch < 0x800)
    {
      call.append(1, (char) ((wch >> 6) | 0xc0));
      call.append(1, (char) ((wch & 0x3f) | 0x80));
      return 1; // 1 UTF-16 char, 2 CESU-8 chars
    }
    else if (wch < 0xffff)
    {
      call.append(1, (char) ((wch >> 12) | 0xe0));
      call.append(1, (char) (((wch >> 6) & 0x3f) | 0x80));
      call.append(1, (char) ((wch & 0x3f) | 0x80));
      return 1; // 1 UTF-16 char, 3 CESU-8 chars
    }
    else // need 2 UTF-16 surrogates
    {
      wchar_t lead = 0xd7c0 + (wch >> 10);
      wchar_t trailing = 0xdc00 + (wch & 0x3ff);
      call.append(1, (char) ((lead >> 12) | 0xe0));
      call.append(1, (char) (((lead >> 6) & 0x3f) | 0x80));
      call.append(1, (char) ((lead & 0x3f) | 0x80));
      call.append(1, (char) ((trailing >> 12) | 0xe0));
      call.append(1, (char) (((trailing >> 6) & 0x3f) | 0x80));
      call.append(1, (char) ((trailing & 0x3f) | 0x80));
      return 2; // 2 UTF-16 chars, 6 CESU-8 chars
    }
  }

	string& hessian_output::write_string(string& call,
		const wstring& value, char more, char last) {
		const int chunk_max = 0x7fff;
		int left = value.length();
		int start = 0;
		int chunk_actual = 0;
		size_t len_ptr = 0;
		while (left > chunk_max) {
			call.append(1, more);
			len_ptr = call.length();
			call.append(1, (char) (chunk_max >> 8));
			call.append(1, (char) (chunk_max & 0xff));
			chunk_actual = 0;
			for (int i = 0; i < chunk_max; ++i)
			{
				chunk_actual+=write_wchar(call,value[start+i]);
			}
			if (chunk_actual != chunk_max)
			{
				// patch length
				call[len_ptr]=(char) (chunk_actual >> 8);
				call[len_ptr+1]=(char)(chunk_actual & 0xff);
			}
			left -= chunk_max;
			start += chunk_max;
		}
		char b16 = (char) (left >> 8);
		char b8 = (char) (left & 0xff);
		call.append(1, last);
		len_ptr = call.length();
		call.append(1, b16);
		call.append(1, b8);
		chunk_actual = 0;
		for (int i = 0; i < left; ++i)
		{
			chunk_actual += write_wchar(call, value[start+i]);
		}
		if (chunk_actual != left)
		{
			// patch length
			call[len_ptr]= (char) (chunk_actual >> 8);
			call[len_ptr+1]= (char) (chunk_actual & 0xff);
		}
		return call;
	}

	string& hessian_output::write_string16(string& call,
		const uint16_t buf[], int len, char more, char last) {
		const int chunk_max = 0x7fff;
		int left = len;
		int start = 0;
		if (left < 0) left = 0;
		while (left > chunk_max) {
			call.append(1, more);
			call.append(1, (char) (chunk_max >> 8));
			call.append(1, (char) (chunk_max & 0xff));
			for (int i = 0; i < chunk_max; ++i)
			{
				write_utf16_char(call, buf[start+i]);
			}
			left -= chunk_max;
			start += chunk_max;
		}
		char b16 = (char) (left >> 8);
		char b8 = (char) (left & 0xff);
		call.append(1, last);
		call.append(1, b16);
		call.append(1, b8);
		for (int i = 0; i < left; ++i)
		{
			write_utf16_char(call, buf[start+i]);
		}
		return call;
	}

	string& hessian_output::write_length16(string& call, int value) {
		char b16 = (char) (value >> 8);
		char b8  = (char) value;
		call.append(1, b16);
		return call.append(1, b8);
	}

	string& hessian_output::write_integer32(string& call, int32_t value) {
		char b32 = (char) (value >> 24);
		char b24 = (char) (value >> 16);
		char b16 = (char) (value >> 8);
		char b8  = (char) value;
		call.append(1, 'I');
		call.append(1, b32);
		call.append(1, b24);
		call.append(1, b16);
		call.append(1, b8);
		return call;
	}

	string& hessian_output::write_integer64(string& call, int64_t value) {
		call.append(1, 'L');
		return write_long64(call, value);
	}

	string& hessian_output::write_ascii_string(string& call,
		const string& ascii_str, char last) {
		int len = ascii_str.length();
		if (len > 0xffff) len = 0xffff;
		call.reserve(call.size() + len + 3);
		call.append(1, last);
		call.append(1, (char) (len >> 8));
		call.append(1, (char) len);
		for (int i = 0; i < len; ++i)
		{
			char ch = ascii_str[i];
			call.append(1, (ch & 0x80) ? '?' : ch);
		}
		return call;
	}

	string& hessian_output::write_fault(string& call, Fault* f_obj) {
		string c("code");
		string m("message");
		string d("detail");
		call.append(1, 'f');
		write_ascii_string(call, c, 'S');
		write_wide_string(call, f_obj->wcode());
		write_ascii_string(call, m, 'S');
		write_wide_string(call, f_obj->wmessage());
		Object* det = f_obj->detail();
		write_ascii_string(call, d, 'S');
		if (det != NULL)
		{
			write_object(call, det);
		}
		else
		{
			write_null(call);
		}
		return call;
	}

	string& hessian_output::write_native_string(string& call,
		const string& mb_str) {
		return write_string(call,String::to_wstring(mb_str),'s','S');
	}

	string& hessian_output::write_wide_string(string& call,
		const wstring& w_str) {
		return write_string(call, w_str, 's', 'S');
	}

	string& hessian_output::write_utf8_string(string& call,
		const string& utf8_str) {
		return write_string(call, String::utf8_to_wstring(utf8_str),
			's', 'S');
	}

	string& hessian_output::write_utf16_string(string& call,
		const uint16_t buf[], int len) {
		return write_string16(call, buf, len, 's', 'S');
	}

	string& hessian_output::write_native_xml(string& call,
		const string& mb_str) {
		return write_string(call,String::to_wstring(mb_str),'x','X');
	}

	string& hessian_output::write_wide_xml(string& call,
		const wstring& w_xml) {
		return write_string(call, w_xml, 'x', 'X');
	}

	string& hessian_output::write_utf8_xml(string& call,
		const string& utf8_str) {
		String tmp(utf8_str);
		return write_string(call, String::utf8_to_wstring(utf8_str),
			'x', 'X');
	}

	string& hessian_output::write_utf16_xml(string& call,
		const uint16_t buf[], int len) {
		return write_string16(call, buf, len, 'x', 'X');
	}

	string& hessian_output::write_binary_image(string& call,
		const string& bin_img) {
		const int chunk_max = 0x8000;
		int left = bin_img.length();
		int start = 0;
		while (left > chunk_max) {
			call.append(1, 'b');
			call.append(1, (char) (chunk_max >> 8));
			call.append(1, (char) chunk_max);
			call.append(bin_img.substr(start, chunk_max));
			left -= chunk_max;
			start += chunk_max;
		}
		call.append(1, 'B');
		call.append(1, (char) (left >> 8));
		call.append(1, (char) left);
		if (start == 0)
			call.append(bin_img);
		else
			call.append(bin_img.substr(start, left));
		return call;
	}
}

