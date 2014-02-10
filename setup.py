#!/usr/bin/env python

from distutils.core import setup
from distutils.extension import Extension


setup(name="ForeFlight",
    ext_modules = [
        Extension("hessian", ["hessianmodule.cpp",
                              'hessian_proxy.cpp',
                              'hessian_input.cpp',
                              'hessian_output.cpp',
                              'string_input_stream.cpp',
                              'wrappers.cpp',
                              'zlibdec.cpp',
                              'sslpp/client.cpp',
                              'sslpp/http_connection.cpp',
                              'sslpp/http_exception.cpp',
                              'sslpp/socket_connection.cpp',
                              'sslpp/ssl_connection.cpp'],
                  libraries = ["boost_python","ssl"])
    ])


