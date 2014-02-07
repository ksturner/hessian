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
/*
	Code in this file is partially based on code
	from the Micro-Hessian Java implementation.
*/

#include "hessian_input.h"

using namespace hessian::wrappers;

namespace hessian {

	void hessian_input::start_reply() throw(io_exception) {
		int tag = read();

		if (tag != 'r')
			throw expect("hessian reply", tag);

		int major = read();
		int minor = read();
		if (major & minor);	// avoid warning
	}

	int hessian_input::start_reply_with_version() throw(io_exception)
	{
		int tag = read();
		if (tag != 'r')
			throw expect("hessian reply", tag);
		int major = read() & 0xff;
		int minor = read() & 0xff;
		return (major << 8) + minor;
	}

	int hessian_input::start_call_with_version() throw(io_exception)
	{
		int tag = read();
		if (tag != 'c')
			throw expect("hessian call", tag);
		int major = read() & 0xff;
		int minor = read() & 0xff;
		return (major << 8) + minor;
	}

	void hessian_input::complete_call() throw(io_exception)
	{
		int tag = read();
		if (tag != 'z')
			throw expect("end of call", tag);
	}

	Object* hessian_input::read_header(int tag, wstring& key)
		throw(io_exception)
	{
		if (tag != 'H') throw expect("header", tag);
		key.assign(read_string('S')); 
		return read_object();
	}

	wstring hessian_input::read_method(int tag)
		throw(io_exception)
	{
		if (tag != 'm') throw expect("method", tag);
		return read_string('S');
	}

	bool hessian_input::is_eof() throw(io_exception)
	{
		int p = _peek;
		if (p != -2) return false; // having a saved byte means no eof
		return _is->eof();
	}

        int hessian_input::read() throw(io_exception)
	{
		int p = _peek;
		if (p == -2) // no saved byte
			return _is->read();
		else
		{
			char ch = (char) p;
			_peek = -2; // consume the saved byte
			return (int) ch;
		}
	}

	int hessian_input::peek() throw(io_exception)
	{
		int p = _peek;
		char ch;
		if (p != -2) // there is a saved byte
		{
			ch = (char) p;
			return (int) ch; // return the saved byte
		}
		else // no saved byte
		{
			p = _is->read();
			ch = (char) p;
			_peek = p & 0xff; // save the byte just read
			return (int) ch;
		}
	}

	void hessian_input::complete_reply() throw(io_exception) {
		int tag = read();

		if (tag != 'z')
			throw expect("end of reply", tag);
	}

	Object* hessian_input::get_result() throw(io_exception) {
		return read_object();
	}

	Object* hessian_input::read_object() throw(io_exception) {
		int tag = read();
		return read_object(tag);
	}

	Object* hessian_input::read_object(int tag) throw(io_exception) {
		switch (tag) {
		case 'b':
		case 'B': return new Binary(read_bytes(tag));
		case 'T':
		case 'F': return new Boolean(read_boolean(tag));
		case 'd': return new Date(read_date(tag));
		case 'D': return new Double(read_double(tag));
		case 'f': return new Fault(read_fault(tag));
		case 'I': return new Integer(read_int(tag));
		case 'V': return new List(read_list(tag));
		case 'L': return new Long(read_long(tag));
		case 'M': return new Map(read_map(tag));
		case 'N': return new Null();
		case 'R': return new Ref(read_ref(tag));
		case 's':
		case 'S': return new String(read_string(tag));
		case 'x':
		case 'X': return new Xml(read_xml(tag));
		default:
			throw io_exception(string("hessian_input::readObject(): tag ").append(1, (char) tag).append(" cannot be handled"));
		}
	}


	bool hessian_input::read_boolean(int tag) throw(io_exception) {
		switch (tag) {
		case 'T': return true;
		case 'F': return false;
		default:
			throw expect("bool", tag);
		}
	}

	int hessian_input::read_int(int tag) throw(io_exception) {
		if (tag != 'I')
			throw expect("integer", tag);

		int b32 = read() & 0xFF;
		int b24 = read() & 0xFF;
		int b16 = read() & 0xFF;
		int b8  = read() & 0xFF;

		return((b32 << 24) + (b24 << 16) + (b16 << 8) + b8);
	}

	int64_t hessian_input::read_long(int tag) throw(io_exception) {
		if (tag != 'L')
			throw expect("long", tag);

		return read_long64();
	}

	double hessian_input::read_double(int tag) throw(io_exception) {
		if (tag != 'D')
			throw expect("double", tag);
		int64_t lval = read_long64();
		double* dval = (double*) & lval;
		return *dval;
	}

	int64_t hessian_input::read_date(int tag) throw(io_exception) {
		if (tag != 'd')
			throw expect("date", tag);
		return read_long64();
	}

	int64_t hessian_input::read_long64() throw(io_exception) {
		int64_t b64 = read() & 0xFF;
		int64_t b56 = read() & 0xFF;
		int64_t b48 = read() & 0xFF;
		int64_t b40 = read() & 0xFF;
		int64_t b32 = read() & 0xFF;
		int64_t b24 = read() & 0xFF;
		int64_t b16 = read() & 0xFF;
		int64_t b8  = read() & 0xFF;
		int64_t value = (b64 << 56) + (b56 << 48) + (b48 << 40) +
			(b40 << 32) + (b32 << 24) + (b24 << 16) + (b16 << 8) +
			b8;
		return value;
	}

	wstring hessian_input::read_string(int tag) throw(io_exception) {
		wstring result;
		while (tag == 's')
		{
			read_string_impl(result);
			tag = read();
		}
		if (tag != 'S')
			throw expect("string", tag);
		read_string_impl(result);
		return result;
	}

	void hessian_input::read_byte_chunk(string& bytes)
		throw(io_exception)
	{
		int b16 = read() & 0xFF;
		int b8 = read() & 0xFF;
		int len = (b16 << 8) + b8;
		bytes.reserve(bytes.size() + len);
		for (int i = 0; i < len; i++)
			bytes.append(1, (char) read() );
	}

	string hessian_input::read_bytes(int tag) throw(io_exception) {
		string bos;
		while (tag == 'b') {
			read_byte_chunk(bos);
			tag = read();
		}

		if (tag != 'B')
			throw expect("bytes", tag);
		read_byte_chunk(bos);

		return bos;
	}

	wstring hessian_input::read_xml(int tag) throw(io_exception) {
		wstring result;
		while (tag == 'x')
		{
			read_string_impl(result);
			tag = read();
		}
		if (tag != 'X')
			throw expect("xml", tag);
		read_string_impl(result);
		return result;
	}

	list<Object*> hessian_input::read_list(int tag) throw(io_exception) {
		if (tag == 'N') {
			list<Object*> l;
			return l;
		}
		if (tag != 'V') {
			throw expect("list", tag);
		}
		int list_length = 0;
		int meta = read();
		// parse optional (meta) type and/or length
		while (meta == 't' || meta == 'l') {
			if (meta == 't') {
				// type
				wstring list_type = read_string('S');
			}
			if (meta == 'l') {
				// length
				int b32 = read();
				int b24 = read();
				int b16 = read();
				int b8  = read();
				list_length = (b32 << 24) + (b24 << 16) + (b16 << 8) + b8;
			}
			meta = read();
		}
		// read in the list data
		list<Object*> l;
		while (meta != 'z') {	// list ends with 'z'
			// read object
			Object* obj = read_object(meta);
			// add to list
			l.push_back(obj);
			// next please
			meta = read();
		}
		return l;
	}

	map<string,Object*> hessian_input::read_map(int tag) throw(io_exception) {
		if (tag == 'N') {
			Map::basic_type l;
			return l;
		}
		if (tag != 'M') {
			throw expect("map", tag);
		}
		int meta = read();
		// parse optional (meta) type
		while (meta == 't') {
			if (meta == 't') {
				// type
				wstring map_type = read_string('S');
			}
			meta = read();
		}
		// read in the map data
		Map::basic_type m;
		while (meta != 'z') {	// map ends with 'z'
			// read key object FIXME (non-string key)
			const string key(
				String::to_utf8_string(read_string(meta)));
			// read value object
			meta = read();
			Object* val = read_object(meta);
			// add to list
			Map::basic_type::value_type pair =
			Map::basic_type::value_type(key,val);
			m.insert(pair);
			// next please
			meta = read();
		}
		return m;
	}

	int hessian_input::read_ref(int tag) throw(io_exception) {
		if (tag != 'R') {
			throw expect("ref", tag);
		}
		int b32 = read() & 0xFF;
		int b24 = read() & 0xFF;
		int b16 = read() & 0xFF;
		int b8  = read() & 0xFF;

		return((b32 << 24) + (b24 << 16) + (b16 << 8) + b8) & 0x00000000FFFFFFFF;
	}

  void hessian_input::read_string_impl(wstring& sb)
    throw(io_exception)
  {
    int b16 = read() & 0xFF;
    int b8  = read() & 0xFF;
    int len = (b16 << 8) + b8;
    sb.reserve(sb.size() + len);
    for (int i = 0; i < len; i++)
    {
      int ch1 = read() & 0xff;
      if (ch1 < 0x80)
      {
        sb.append(1, (wchar_t) ch1);
        continue;
      }
      else if ((ch1 & 0xe0) == 0xc0)
      {
        int ch2 = read();
        wchar_t v = ((ch1 & 0x1f) << 6) + (ch2 & 0x3f);
        // 2 byte sequence from 0x80 to 0x7ff
        if ((v >= 0x80) && ((ch2 & 0xc0) == 0x80))
        {
          sb.append(1, v);
          continue;
        }
      }
      else if ((ch1 & 0xf0) == 0xe0)
      {
        int ch2 = read();
        int ch3 = read();
        wchar_t v = ((ch1 & 0x0f) << 12) + ((ch2 & 0x3f) << 6) + (ch3 & 0x3f);
        // 3 byte sequence from 0x800 to 0xfffd
        // d800-dbff first surrogate
        // dc00-dfff second surrogate
        if ((v >= 0x800) && (v < 0xfffe) &&
          ((ch2 & 0xc0) == 0x80) && ((ch3 & 0xc0) == 0x80))
        {
          if ((v < 0xd800) || (v > 0xdfff)) // not a surrogate
          {
            sb.append(1, v);
            continue;
          }
          else if (v < 0xdc00) // first surrogate
          {
            int ch4 = read();
            int ch5 = read();
            int ch6 = read();
            if (((ch4 & 0xf0) == 0xe0) && ((ch5 & 0xc0) == 0x80) &&
              ((ch6 & 0xc0) == 0x80))
            {
              wchar_t v2 = ((ch4 & 0x0f) << 12) + ((ch5 & 0x3f) << 6) +
                (ch6 & 0x3f);
              if ((v2 >= 0xdc00) && (v2 <= 0xdfff)) // second surrogate
              {
                wchar_t codepoint = (v << 10) - 0x35fdc00 + v2;
                sb.append(1, codepoint);
                continue;
              }
            }
          }
        }
      }
      throw io_exception("bad CESU-8 encoding");
    }
  }

	wstring hessian_input::read_string()
		throw(io_exception)
	{
		wstring result;
		int tag = read();
		if ((tag == 's') || (tag == 'S'))
		{
			return read_string(tag);
		}
		throw expect("string", tag);
	}

	Fault hessian_input::read_fault(int tag) throw(io_exception) {
		if (tag != 'f')
			throw expect("fault", tag);
		String code_key("code");
		String message_key("message");
		String detail_key("detail");
		String code_val;
		String message_val;
		int _tag = read();
		while (_tag != 'z')
		{
			String key(read_string(_tag));
			if (code_key == key)
			{
				_tag = read();
				code_val = read_string(_tag);
			}
			else if (message_key == key)
			{
				_tag = read();
				message_val = read_string(_tag);
			}
			else if (detail_key == key)
			{
				_tag = read();
				delete read_object(_tag); // skip over detail
			}
			else
			{
				_tag = read();
				delete read_object(_tag); // skip unknown key
			}
			_tag = read();
		}
		return Fault(code_val.wvalue(), message_val.wvalue());
	}

	io_exception hessian_input::expect(const string& expect, int ch) {
		if (ch < 0)
			return io_exception("expected " + expect + " at end of file");
		else
			return io_exception("expected " + expect + " at " + (char) ch);
	}
}

