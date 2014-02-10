#include "http_exception.h"

using namespace sslpp::exceptions;

http_exception::http_exception(const string& what) : _message(what) {}

http_exception::~http_exception() throw() {}

const char* http_exception::what() const throw() {
	return _message.c_str();
}

void http_exception::raise() const {
	throw *this;
}

