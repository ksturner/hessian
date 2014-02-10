#include <iostream>

using namespace std;

void say_hello(const char* name) {
    cout << "Hello, " << name << "!\n";
}


#include <boost/shared_ptr.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/class.hpp>

#include "zlibdec.h"
#include "hessian_proxy.h"

using namespace boost;
using namespace boost::python;
using namespace hessian;
using namespace hessian::wrappers;

/*

from mustaine import HessianProxy

store_client = HessianProxy('http://stage.foreflight.com/store/hessian/StoreServicePrivate')
account = store_client.findAccountCompositeByUuid('eea4ecc0-b0e9-11e1-a954-003048cf2bcf')

 */


BOOST_PYTHON_MODULE(hessian) {
    def("say_hello", say_hello);

    class_<hessian_proxy>("hessian_proxy",init<std::string,std::string,bool>())
        .def("url", &hessian_proxy::url)
    ;

    /*
    class_<hessian_proxy, shared_ptr<hessian_proxy>, noncopyable>("hessian_proxy",init<std::string,std::string,bool>())
        .def("__call__", &hessian_proxy::call)
    ;
    */
    /* .def("__call__", &Signal::operator()) // if above doesn't work. */
}



