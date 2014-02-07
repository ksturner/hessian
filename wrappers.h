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
#ifndef HESSIAN_WRAPPERS_H
#define HESSIAN_WRAPPERS_H

#include <string>
#include <list>
#include <map>
#include <inttypes.h>

namespace std {}

namespace hessian {
	namespace wrappers {
		using namespace std;

		///Non-copyable base class for wrapper object classes.
		class Object {
		public:
			Object() : _classname("Object") {
				_created++;
			}
			virtual ~Object() {
				_deleted++;
			}
			string classname() const {
				return string(_classname);
			}
			static int created() {
				return _created;
			}
			static int deleted() {
				return _deleted;
			}
			static int copied() {
				return _copied;
			}
			Object(const Object& other) : _classname(other._classname) {
				_copied++;
			}
			Object operator=(const Object& right) {
				_copied++;
				return * this;
			}
		protected:
			const char*  _classname;
		private:
			static int _created;
			static int _deleted;
			static int _copied;
		public:
			Object(const char* classname) : _classname(classname) {
				_created++;
			}
			const char* classname2() const {
				return _classname;
			}
		};

		/// null wrapper class
		class Null : public Object {
		public:
			typedef void* basic_type;
			Null() : Object("Null")  {
			}
			basic_type value() const {
				return NULL;
			}
		};

		/// ref wrapper class
		class Ref : public Object {
		public:
			typedef int basic_type;
			Ref(int value)
			: Object("Ref"), _value(value) {
			}
			basic_type value() const {
				return _value;
			}
			operator basic_type() const {
				return _value;
			}
		protected:
			basic_type _value;
		};

		/// remote wrapper class
		class Remote : public Null {
		public:
			Remote() {
				_classname = "Remote";
			}
		};


		/// long long (64) wrapper class
		class Long : public Object {
		public:
			typedef int64_t basic_type;
			Long(basic_type value)
			: Object("Long"), _value(value) {
			}
			basic_type value() const {
				return _value;
			}
			operator basic_type() const {
				return _value;
			}
		protected:
			basic_type _value;
		};


		/// int (32) wrapper class
		class Integer : public Object {
		public:
			typedef int basic_type;
			Integer(int value)
			: Object("Integer"), _value(value) {
			}
			basic_type value() const {
				return _value;
			}
			operator basic_type() const {
				return _value;
			}
		protected:
			basic_type _value;
		};


		/// bool wrapper class
		class Boolean : public Object {
		public:
			typedef bool basic_type;
			Boolean(bool value)
			: Object("Boolean"), _value(value) {
			}
			bool value() const {
				return _value;
			}
			operator basic_type() const {
				return _value;
			}
		protected:
			basic_type _value;
		};


		/// date wrapper class, same as Long
		class Date : public Long {
		public:
			Date(Long::basic_type value)
			: Long(value) {
				_classname = "Date";
			}
		};


		/// binary wrapper class
		class Binary : public Object {
		public:
			typedef string basic_type;
			Binary(string value)
			: Object("Binary"), _value(value) {
			}
			basic_type value() const {
				return _value;
			}
			operator basic_type() const {
				return _value;
			}
			operator const char*() const {
				return _value.c_str();
			}
		protected:
			basic_type _value;
		};


		/// double wrapper class
		class Double : public Object {
		public:
			typedef double basic_type;
			Double(double value)
			: Object("Double"), _value(value) {
			}
			basic_type value() const {
				return _value;
			}
			operator basic_type() const {
				return _value;
			}
		protected:
			basic_type _value;
		};


		/// std::list wrapper class.
		/**
		 * Usage: create a std::list<Object*> (basic_type) to
		 * which add _only_ pointers to objects created with new, i.e
		 * <pre>
		 * 	// regular STL list
		 * 	std::list<Object*> lst;
		 * 	// data to put in, notice "new"
		 * 	Integer* integer = new Integer(10);
		 * 	String* str = new String("foo bar");
		 * 	lst.push_back(integer);
		 * 	lst.push_back(str);
		 * 	// wrap list
		 * 	List olist(lst);
		 * 	// use it as parameter to method call
		 * </pre>
		 * When the List goes out of scope, the dtor iterates the wrapped
		 * std::list<Object*> and deletes all Objects.
		 */
		class List : public Object {
		public:
			typedef list<Object*> basic_type;
			List(const basic_type& value)
			: Object("List"), _value(value) {
			}
			/// Deletes all Object in the embedded std::list<Object*>
			virtual ~List() {
				for (basic_type::iterator i = _value.begin(); i != _value.end(); i++) {
					Object* obj = *i;
					delete obj;
				}
			}
			basic_type value() const {
				return _value;
			}
			operator basic_type() const {
				return _value;
			}
		protected:
			basic_type _value;
		private:
			List(const List& other) {
			}
			List operator=(const List& other) {
				return *this;
			}
		};

		/// std::string wrapper class
		class String : public Object {
		public:
			typedef string basic_type;
			static wstring to_wstring(const string&);
			static wstring utf8_to_wstring(const string&);
			static string to_string(const wstring&);
			static string to_utf8_string(const wstring&);
			String() : Object("String"), _value() {
			}
			String(const wstring& value)
			: Object("String"), _value(value) {
			}
			String(const string& utf8_str);
			basic_type value() const;
			wstring wvalue() const {
				return _value;
			}
			const wstring& read_only_wvalue() const {
				return _value;
			}
			operator basic_type() const {
				return value();
			}
			bool operator<(const String& other) const {
				return _value < other._value;
			}
			bool operator<=(const String& other) const {
				return _value <= other._value;
			}
			bool operator==(const String& other) const {
				return _value == other._value;
			}
		protected:
			wstring _value;
		};

		/// std::map wrapper class
		class Map : public Object {
		public:
			typedef map<string,Object*> basic_type;
			Map(const basic_type& value)
			: Object("Map"), _value(value) {
			}
			virtual ~Map() {
				for (basic_type::iterator i = _value.begin(); i != _value.end(); i++) {
					delete i->second;
				}
			}
			basic_type value() const {
				return _value;
			}
			operator basic_type() const {
				return _value;
			}
		protected:
			basic_type _value;///< Map's internal std::map
		private:
			Map(const Map& other) {
			}
			Map operator=(const Map& other) {
				return *this;
			}
		};

		/// xml wrapper class
		class Xml : public String {
		public:
			typedef string basic_type;
			Xml(const wstring& value)
			: String(value) {
				_classname = "Xml";
			}
			Xml(const string& utf8_str)
			: String(utf8_str) {
				_classname = "Xml";
			}
		};

		/// fault wrapper class
		class Fault : public Object {
		public:
			typedef string basic_type;
			Fault(const wstring& code, const wstring& message) :
			Object("Fault"),
			_code(code), _message(message), _detail(NULL)
			{
			}

			Fault(const wstring& code, const wstring& message,
				Object* detail) :
			Object("Fault"),
			_code(code), _message(message), _detail(detail)
			{
			}

			Fault(const string& code, const string& message
			) :
			Object("Fault"), _code(String::to_wstring(code)),
			_message(String::to_wstring(message)), _detail(NULL)
			{
			}

			Fault(const string& code, const string& message,
				Object* detail) :
			Object("Fault"),
			_code(String::to_wstring(code)),
			_message(String::to_wstring(message)),
			_detail(detail)
			{
			}

			Fault(const Fault& other) :
			Object("Fault"),
			_code(other._code), _message(other._message),
			_detail(NULL)
			{
			}

			~Fault() { delete _detail; }
			basic_type code() const;
			basic_type message() const;
			basic_type value() const {
				return string("HessianFault: {code=").
				append(code()).append(", message=").
				append(message()).append("}");
			}
			wstring wvalue() const {
				return wstring(L"HessianFault: {code=").
				append(_code).append(L", message=").
				append(_message).append(L"}");
			}
			wstring wcode() const { return _code; }
			wstring wmessage() const { return _message; }
			Object* detail() const { return _detail; }
		protected:
			wstring _code;
			wstring _message;
			Object* _detail;
		};
	}
}
#endif 
/*
*binary						A binary value
*boolean					The byte 'F' represents false and the byte 'T' represents true
*date							Date represented by a 64-bits long of milliseconds since the epoch
*double						A 64-bit IEEE floating pointer number
*int							A 32-bit signed integer
*list							An ordered list, like an array
*long							A 64-bit signed integer
*map							Represents serialized objects and Maps
*null							Null represents a null pointer
*ref							An integer referring to a previous list or map instance
*remote						A reference to a remote object
*string						A 16-bit unicode character string encoded in UTF-8
*xml							An XML document encoded as a 16-bit unicode character string encoded in UTF-8
*/
