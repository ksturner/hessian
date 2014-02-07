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
#include <iostream>
#include <string>
#include <list>
#include <map>
#include <memory>

#include "zlibdec.h"

#include "hessian_proxy.h"


using namespace std;

using namespace hessian;
using namespace hessian::wrappers;

template <typename T> void dump(T t) {
	cout << t->classname() << "\t: " << t->value() << endl;
}

void dump(Object* o);

void dumplist(const list<Object*>& l) {
	cout << "List {" << endl;
	for (list<Object*>::const_iterator i = l.begin(); i != l.end(); i++) {
		cout << "\t"; dump(*i);
	}
	cout << "}" << endl;
}

void dumpmap(Map::basic_type l) {
	for (Map::basic_type::iterator i = l.begin(); i != l.end(); i++) {
		cout << "{" << endl << "key ";
		cout << (*i).first << endl;
		cout << "value ";
		dump((*i).second);
		cout << "}" << endl;
	}
}

void dump(Object* o) {
	if (Binary* _obj = dynamic_cast<Binary*>(o)) {
		cout << "binary length " << _obj->value().length() << endl;
		return ;
	}
	if (Boolean* _obj = dynamic_cast<Boolean*>(o)) {
		dump(_obj);
		return ;
	}
	if (Date* _obj = dynamic_cast<Date*>(o)) {
		dump(_obj);
		return ;
	}
	if (Double* _obj = dynamic_cast<Double*>(o)) {
		dump(_obj);
		return ;
	}
	if (Integer* _obj = dynamic_cast<Integer*>(o)) {
		dump(_obj);
		return ;
	}
	if (List* _obj = dynamic_cast<List*>(o)) {
		dumplist(_obj->value());
		return ;
	}
	if (Long* _obj = dynamic_cast<Long*>(o)) {
		dump(_obj);
		return ;
	}
	if (Map* _obj = dynamic_cast<Map*>(o)) {
		dumpmap(_obj->value());
		return ;
	}
	if (Null* _obj = dynamic_cast<Null*>(o)) {
		dump(_obj);
		return ;
	}
	if (Ref* _obj = dynamic_cast<Ref*>(o)) {
		dump(_obj);
		return ;
	}
	if (Remote* _obj = dynamic_cast<Remote*>(o)) {
		dump(_obj);
		return ;
	}
	if (String* _obj = dynamic_cast<String*>(o)) {
		dump(_obj);
		return ;
	}
	if (Xml* _obj = dynamic_cast<Xml*>(o)) {
		dump(_obj);
		return ;
	}
	if (Fault* _obj = dynamic_cast<Fault*>(o)) {
		dump(_obj);
		return ;
	}
	cout << "Warning: dump does not handle " << o->classname() << endl;
}

/*
void testnoncopy() {
	// list
	List::basic_type _list;
	List l(_list);
	List l1 = l;
	List l2(_list);
	l2 = l;
	// map
	Map::basic_type _map;
	Map m(_map);
	Map m1 = m;
	Map m2(_map);
	m2 = m;
}
*/

void debug_memory() {
	// debug object creation/destruction
	cout << "created " << Object::created() << " Objects" << endl;
	cout << "deleted " << Object::deleted() << " Objects" << endl;
	cout << "copied  " << Object::copied() << " Objects" << endl;
	cout << Object::created() + Object::copied() - Object::deleted() << " leaks" << endl;
}

int main(int argc, char* argv[]) {
	cout << "starting" << endl;

	// start a scope to ensure destructors are called on all objects
	{
		Integer p_int1(32890);
		String p_bin("cine a mancat sarmalele lu Doru ?");

		// input parameter
		String p_str1("input string"); // allocate on stack
		std::list<Object*> the_list; // allocate list on stack
		for (int i = 0; i < 10; i++) {
			String* p_str2 = new String("string in list");// list deletes objects held
			the_list.push_back(p_str2);
		}
		List p_list(the_list);
		// hessian proxy, with SSL
		hessian_proxy proxy("www.aldratech.com:82", "/hello", true);
		try {
			// sendInt(int):int
			Object* res1 = proxy.call("sendInt", 1, &p_int1);
			auto_ptr<Object> auto_release_res1(res1);
			
			// receiveBinary(byte[]):byte[]
			Object* res2 = proxy.call("receiveBinary", 0);
			auto_ptr<Object> auto_release_res2(res2);
			
			// sendList(list, string):list
			Object* result = proxy.call("sendList", 2, &p_list, &p_str1);
			// auto-release result
			auto_ptr<Object> auto_release(result);
			// test for a Fault and throw an exception
			// get the list as expected
			if (List* l = dynamic_cast<List*>(result)) {
				// print list
				dumplist(*l);
			}
			else {
				if (Fault* f = dynamic_cast<Fault*>(result)) {
					throw hessian::exceptions::fault_exception(f->code(), f->message());
				}
				else {
					cout << "unknown object received: " << result->classname() << endl;
				}
			}
		}
		catch (sslpp::exceptions::http_exception& e) {
			cout << "http exception: " << e.what() << endl;
		}
		catch (hessian::exceptions::io_exception& e) {
			cout << "io exception: " << e.what() << endl;
		}
		catch (hessian::exceptions::fault_exception& e) {
			cout << "hessian fault: " << e.what() << endl;
		}
		catch (std::exception& e) {
			cout << "other exception occured: " << e.what() << endl;
		}
	}
	// object scope ends

	// traffic stats
	unsigned long long bin,bout,ncalls;
	bin = hessian_proxy::bytes_in();
	bout = hessian_proxy::bytes_out();
	ncalls = hessian_proxy::num_calls();
	cout << "STATS" << endl;
	cout << "\tin\t: " << bin << endl << "\tout\t: " << bout << endl;
	cout << "\tb/c\t: " << (bin + bout)/ncalls << endl << endl;
	map<string, unsigned int> csm = hessian_proxy::call_size_map();
	cout << "call size map: " << endl;
	for (map<string, unsigned int>::iterator i = csm.begin(); i != csm.end(); i++) {
		cout << "\t" << (*i).first << " " << (*i).second << "b" << endl;
	}
	cout << endl;

	debug_memory();

	cout << "done" << endl;
	return 0;
}

