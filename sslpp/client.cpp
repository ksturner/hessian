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
#include <string>
#include <iostream>
#include "ssl++.h"

using namespace std;

void testpost(const string& host_spec, bool use_ssl) {
	try {
		sslpp::http_connection conn(host_spec, use_ssl);
		cout << conn.toString() << endl;
		string reply = conn.POST("/post_test.php", "j_username=admin&j_password=password", "application/x-www-form-urlencoded");
		cout << "[" << conn.parse_reply(reply) << "]" << endl;
	}
	catch (sslpp::exceptions::http_exception& e) {
		cout << "exception: " << e.what() << endl;
	}
	catch (...) {
		cout << "caught unknown exception" << endl;
	}
}

int main(int argc, char* argv[]) {
	if (argc < 3) {
		cout << "usage " << argv[0] << " host:port [0|1]" << endl;
		return 1;
	}
	string host_spec(argv[1]);
	bool use_ssl = atoi(argv[2]);
	for (int i = 0; i < 1; i++) {
		testpost(host_spec, use_ssl);
	}
	return 0;
}
