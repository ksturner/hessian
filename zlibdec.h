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
#ifndef ZLIBDEC_H
#define ZLIBDEC_H

#include <string>
#include <exception>

#ifdef WIN32
	#pragma warning(disable:4290)
#endif

using namespace std;

///ZLib_exception class.
class zlib_exception : public exception {
public:
	///Constructor with error message.
	explicit zlib_exception(const string& what): _message(what) {
	}
	///Destructor.
	virtual ~zlib_exception() throw() {
	}
	///Get error message.
	virtual const char* what() const throw() {
		return _message.c_str();
	}
	///Re-throw.
	virtual void raise() const {
		throw *this;
	}
private:
	string _message;
};

///ZLib decompression of gzipped data.
class zlibdec {
public:
	///Constructor.
	/**
	 * @param buf_size Internal decompression temporary buffer size.
	 */
	zlibdec(unsigned int buf_size = 512);
	/// Decompress gzip data.
	/**
	 * @param buffer Input gzip data buffer.
	 * @param len Buffer length.
	 * @return Decompressed data.
	 * @throws zlib_exception
	 */
	string decompress(const char* buffer, unsigned int len) throw (zlib_exception);
	/// Decompress gzip data.
	/**
	 * @param buffer Input gzip data buffer.
	 * @return Decompressed data.
	 * @throws zlib_exception
	 */
	string decompress(const string& buffer) throw (zlib_exception);
private:
	const unsigned _buf_size;	/// Internal decompression temporary buffer size.
};

#endif

