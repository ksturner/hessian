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

#undef _GNU_SOURCE

#ifdef WIN32
	#include <winsock2.h>
#else
	#include <string.h>
	#include <errno.h>
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
#endif

#include <iostream>
using namespace std;

namespace sslpp {

	bool socket_connection::_winsock_initialised = false;

	socket_connection::socket_connection(const string& endpoint): _endpoint(endpoint), _valid(false), _socket(-1), _host_resolve_error(false) {
		// parse endpoint
		string::size_type pos = _endpoint.find(':');
		if (pos == string::npos) {
			cout << "socket_connection::ctor(): invalid endpoint, ':' missing (use hostname:port)" << endl;
			return ;
		}
		string hostname = _endpoint.substr(0, pos);
		string port_ = _endpoint.substr(pos + 1);
		unsigned short port = atoi(port_.c_str());
		// initialise winsock
		if (!winsock_init()) {
			return;
		}
		// create the socket
		_socket = socket(PF_INET, SOCK_STREAM, 0);
		if (_socket == -1) {
			cout << "socket_connection::ctor(): invalid socket" << endl;
			return ;
		}
		// prepare address
		struct sockaddr_in srv;
		srv.sin_family = AF_INET;
		srv.sin_port = htons(port);
		hostent* host_ = gethostbyname(hostname.c_str());
		if (host_ == NULL) {
			_host_resolve_error = true;
			return ;
		}
		memcpy(&(srv.sin_addr), host_->h_addr, host_->h_length);
		// connect
		int conn_status = connect(_socket, (struct sockaddr*) & srv, sizeof(struct sockaddr_in));
		if (conn_status != 0) {
			return ;
		}
		_valid = true;
	}

	socket_connection::~socket_connection() {
#ifdef WIN32
		closesocket(_socket);
#else
		close(_socket);
#endif
	}

	socket_connection& socket_connection::operator<<(const std::string& buf) {
		int len = send(_socket, buf.c_str(), (int)buf.length(), 0);
		if (len < (int)(buf.length())) {
			cout << "warn: socket_connection::operator<<: sent " << len << " of " << (int)buf.length() << endl;
		}
		return *this;
	}

	socket_connection& socket_connection::operator>>(std::string& buf) {
	#define __READ_CHUNK_SIZE 1024
		char str[__READ_CHUNK_SIZE];
		int ret;
		buf = "";
		while ((ret = recv(_socket, str, sizeof(str) - 1, 0)) > 0) {
			str[ret] = 0;
			buf.append(str, ret);
		}
		return *this;
	}

	int socket_connection::read(int max_size, char* buf) {
		return recv(_socket, buf, max_size - 1, 0);
	}

	int socket_connection::write(const std::string& what) {
		//return send(_socket, what.c_str(), what.length(), 0);
		return write(what.c_str(), what.length());
	}

	int socket_connection::write(const char* data, unsigned int length) {
		unsigned int offset = 0;
		unsigned int bsize = 1024;
		if(bsize > length) bsize = length;
		while(true) {
			int wrote = send(_socket, data+offset, bsize, 0);
			// check for write error
			if(wrote < 0) {
				int _errno = errno;
				char* buf = strerror(_errno);
				cout << "error: socket_connection::write(): sent " << offset << " of " << length << endl;
				cout << "detail: " << buf << endl;
				return offset;
			}
			// advance the data buffer offset
			offset += wrote;
			// check for incomplete write
			if((unsigned int)wrote < bsize) {
				cout << "warn: socket_connection::write(): incomplete write: wrote " << wrote << " of " << bsize << endl;
				cout << "warn: socket_connection::write(): sent " << offset << " of " << length << endl;
			}
			unsigned int remaining = length - offset;
			if(bsize > remaining) bsize = remaining;
			//cout << "wrote " << wrote << ", next " << bsize << ", @ " << offset << " / " << length << endl << flush;
			if (remaining == 0) {
				//cout << "done" << endl;
				break;
			}
		}
		return offset;
	}

	string socket_connection::errors() {
		if (_host_resolve_error) {
			return "Error resolving host";
		}
		if (!_winsock_initialised) {
			return "Error initialising WinSock";
		}
		char* buf = strerror(errno);
		string trace(buf);
		return trace;
	}

	bool socket_connection::winsock_init() {
		if (_winsock_initialised) {
			return true;
		}
		// do this only once
		_winsock_initialised = true;
#ifdef WIN32
		WSADATA wsadata;
		if (WSAStartup(MAKEWORD(2,2), &wsadata) != NO_ERROR) {
			cout << "socket_connection::winsock_init(): error initialising winsock" << endl;
			return false;
		}
		return true;
#else
		return true;
#endif
	}
}
