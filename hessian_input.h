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

#ifndef HESSIAN_HESSIAN_INPUT_H
#define HESSIAN_HESSIAN_INPUT_H

#include <inttypes.h>
#include "wrappers.h"
#include "exceptions.h"
#include "input_stream.h"
#include <string>
#include <list>
#include <memory>

// using namespace hessian::wrappers;

namespace std {}

namespace hessian {
	using namespace hessian::exceptions;
	using namespace std;

	/// Process input stream for Hessian responses.
	class hessian_input {
	public:
		/**
		 * Creates a new Hessian input stream, initialized with an
		 * underlying input stream.
		 *
		 * @param is the underlying input stream.
		 */
		hessian_input(auto_ptr<input_stream> is, int debug_level = 0) :
			_is(is), _peek(-2), _debug_level(debug_level) {
		}

		/// Destructor.
		virtual ~hessian_input() {
		}

		/**
		 * Starts reading the reply.
		 * <p>Hessian:
		 * <pre>
		 * r x01 x00
		 * </pre>
		 * @exception io_exception
		 */
		void start_reply() throw(io_exception);

		/**
		 * Completes reading the call.
		 * <p>Hessian:
		 * <pre>
		 * z
		 * </pre>
		 * @exception io_exception
		 */
		void complete_reply() throw(io_exception);

		///Retrieve the call result.
		wrappers::Object* get_result() throw(io_exception);

	protected:
		/// Reads an object from the stream
		/**
		 * @exception io_exception when IO error or expected class type not found
		 */
		wrappers::Object* read_object() throw(io_exception);

		/// Reads an object from the stream with object tag already read
		/**
		 * @param tag the object tag from the stream
		 * @exception io_exception when IO error or expected class type not found
		 */
		wrappers::Object* read_object(int tag) throw(io_exception);

		/**
		 * Reads a bool.
		 * <p>Hessian:
		 * <pre>
		 * T
		 * F
		 * </pre>
		 * @exception io_exception
		 */
		bool read_boolean(int tag) throw(io_exception);

		/**
		 * Reads an int.
		 * <p>Hessian:
		 * <pre>
		 * I b32 b24 b16 b8
		 * </pre>
		 * @exception io_exception
		 */
		int read_int(int tag) throw(io_exception);

		/**
		 * Reads a double.
		 * <p>Hessian:
		 * <pre>
		 * D b64 b56 b48 b40 b32 b24 b16 b8
		 * </pre>
		 * @exception io_exception
		 */
		double read_double(int tag) throw(io_exception);

		/**
		 * Reads a long.
		 * <pre>
		 * L b64 b56 b48 b40 b32 b24 b16 b8
		 * </pre>
		 * @exception io_exception
		 */
		int64_t read_long(int tag) throw(io_exception);

		/**
		 * Reads an UTC date.
		 * <p>Hessian:
		 * <pre>
		 * T b64 b56 b48 b40 b32 b24 b16 b8
		 * </pre>
		 * @exception io_exception
		 */
		int64_t read_date(int tag) throw(io_exception);

		/**
		 * Reads a string.
		 * <p>Hessian:
		 * <pre>
		 * (s b16 b8 utf8_string)* S b16 b8 utf8_string
		 * </pre>
		 * @exception io_exception
		 */
		wstring read_string(int tag) throw(io_exception);

		/**
		 * Reads a byte array.
		 * <p>Hessian:
		 * <pre>
		 * (b b16 b8 data)* B b16 b8 data
		 * </pre>
		 * @exception io_exception
		 */
		string read_bytes(int tag) throw(io_exception);

		/**
		 * Reads a list.
		 * <p>Hessian:
		 * <pre>
		 * V (t b16 b8 type-string)?
		 * length?
		 * object*
		 * z
		 * </pre>
		 * @exception io_exception
		 */
		list<wrappers::Object*> read_list(int tag) throw(io_exception);

		/**
		 * Reads a map.
		 * <p>Hessian:
		 * <pre>
		 * M (t b16 b8 type-string)?
		 * (object, object)*
		 * z
		 * </pre>
		 * @exception io_exception
		 */
		map<string,wrappers::Object*> read_map(int tag) throw(io_exception);

		/**
		 * Reads a ref.
		 * <p>Hessian:
		 * <pre>
		 * R b32 b24 b16 b8
		 * </pre>
		 * @exception io_exception
		 */
		int read_ref(int tag) throw(io_exception);

		/**
		 * Reads an Xml.
		 * <p>Hessian:
		 * <pre>
		 * (x b16 b8 utf8_string)* X b16 b8 utf8_string
		 * </pre>
		 * @exception io_exception
		 */
		wstring read_xml(int tag) throw(io_exception);

		/**
		 * Reads a Fault.
		 * @exception io_exception
		 */
		wrappers::Fault read_fault(int tag) throw(io_exception);
		// Reads a binary chunk from the underlying stream.
		void read_byte_chunk(string& buf)
			throw(io_exception);
		///Reads a string from the underlying stream.
		void read_string_impl(wstring& buf)
			 throw(io_exception);
		///Reads a 64-bit long from the underlying stream, no tags.
		int64_t read_long64() throw (io_exception);
		///Creates exception message.
		io_exception expect(const string& expect, int ch);
		///Auto-Ptr to input stream.
		auto_ptr<input_stream> _is;
		int _peek; // one byte look-ahead: -2 no byte, 0-255 byte
		int _debug_level;
	public:
		/**
		 * Starts reading the reply, and returns the version number.
		 * <p>Hessian:
		 * <pre>
		 * r x01 x00
		 * </pre>
		 * @returns the version number
		 * @exception io_exception
		 */
		int start_reply_with_version() throw(io_exception);

		/**
		 * Starts reading the call, and returns the version number.
		 * <p>Hessian:
		 * <pre>
		 * c x01 x00
		 * </pre>
		 * @returns the version number
		 * @exception io_exception
		 */
		int start_call_with_version() throw(io_exception);

		/**
		 * Complete reading the call.
		 * <p>Hessian:
		 * <pre>
		 * z
		 * </pre>
		 * @exception io_exception
		 */
		void complete_call() throw(io_exception);

		/**
		 * Starts reading the header, and returns the associated
		 * object.
		 * <p>Hessian:
		 * <pre>
		 * H b16 b8 header-string object
		 * </pre>
		 * @param tag  the tag value, which should be 'H'
		 * @param key  the header-string is written to this variable
		 * @returns the associated object
		 * @exception io_exception
		 */
		wrappers::Object* read_header(int tag, wstring& key)
			throw(io_exception);

		/**
		 * Starts reading the method name, and returns the method
		 * name.
		 * <p>Hessian:
		 * <pre>
		 * m b16 b8 method-name
		 * </pre>
		 * @param tag  the tag value, which should be 'm'
		 * @returns the method name
		 * @exception io_exception
		 */
		wstring read_method(int tag) throw(io_exception);

		/**
		 * Returns true if the underlying stream has reached
		 * end of file condition.
		 */
		bool is_eof() throw(io_exception);

		/**
		 * Returns the next byte.
		 */
		int read() throw(io_exception);

		/**
		 * Return the next byte without consuming it.
		 */
		int peek() throw(io_exception);

		/**
		 * Parse and return a string.
		 */
		wstring read_string() throw(io_exception);

		/**
		 * Parse and return an integer.
		 */
		int read_int() throw(io_exception)
		{
			int ch = read();
			return read_int(ch);
		}

		/**
		 * Parse and return a 64 bit integer.
		 */
		int64_t read_long() throw(io_exception)
		{
			int ch = read();
			return read_long(ch);
		}

		/**
		 * Parse and return a string.
		 */
		bool read_boolean() throw(io_exception)
		{
			int ch = read();
			return read_boolean(ch);
		}
	};
}

#endif

