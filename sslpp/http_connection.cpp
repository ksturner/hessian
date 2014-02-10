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
#include "ssl++.h"

#include <iostream>
#include <string>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace sslpp;
using namespace sslpp::exceptions;
using namespace boost;

http_connection::http_connection(const string& hostspec, bool use_ssl): _hostspec(hostspec), _use_ssl(use_ssl), _conn(NULL) {
	if (_use_ssl) {
		_conn = new ssl_connection(hostspec);
	}
	else {
		_conn = new socket_connection(hostspec);
	}
}

http_connection::~http_connection() {
	if (_conn) {
		delete _conn;
	}
}

string http_connection::parse_reply(const string& reply) const throw (http_exception) {
	string length;
	bool found = false;
	int len;
	typedef boost::tokenizer<boost::char_separator<char> > char_tokenizer;
	char_separator<char> sep(" \n\r");
	char_tokenizer tok(reply, sep);
	string content_length = "Content-Length:";
	for (char_tokenizer::iterator token = tok.begin(); token != tok.end(); ++token) {
		if ((*token).compare(content_length) == 0) {
			token++;
			length = *token;
			found = true;
			break;
		}
	}
	if (!found) {
		// Content-Length: spec not found, search for the double newline (\r\n\r\n)
		string double_newline("\r\n\r\n");
		string::size_type reply_starts_at = reply.find(double_newline);
		if (reply_starts_at == string::npos) {
			throw http_exception("http_connection::parse_reply(): unable to determine reply start");
		}
		return reply.substr(reply_starts_at + double_newline.length());
	}
	try {
		len = lexical_cast<int>(length);
	}
	catch (boost::bad_lexical_cast) {
		throw http_exception("http_connection::parse_reply(): exception converting Content-Length to integer");
	}
	if (len == 0) {
		cout << "http_connection::parse_reply(): warning " << content_length << " is " << len << endl;
		return "";
	}
	return reply.substr(reply.length() - len, reply.length());
}

string http_connection::POST(const string& url, const string& data, const string& content_type, const string& user_agent) throw (http_exception) {
	if (!_conn->valid()) {
		throw http_exception(string("http_connection::POST(): connection not valid (").append(_conn->errors()).append(")"));
	}
	string reply;
	string request("POST ");
	string content_length = lexical_cast<std::string>((unsigned int)data.length());
	request.append(url);
	request.append(" HTTP/1.0");
	request.append("\nContent-Type: ");
	request.append(content_type);
	request.append("\nUser-Agent: ");
	request.append(user_agent);
	request.append("\nContent-Length: ");
	request.append(content_length);
	request.append("\nConnection: close\n\n");
	request.append(data);
	// write request
	int ret = _conn->write(request);
	if (ret != (int)(request.length())) {
		throw http_exception(string("http_connection::POST(): error writting request\n").append(_conn->errors()));
	}
	// read reply
	*_conn >> reply;
	return reply;
}

string http_connection::GET(const string& url, const string& data) throw (http_exception) {
	if (!_conn->valid()) {
		throw http_exception("http_connection::POST(): SSL connection not valid");
	}
	throw http_exception("http_connection::GET(): not implemented");
}

string http_connection::toString() const {
	string name(_hostspec);
	if (_use_ssl)
		name.append(" SSL");
	return name;
}
