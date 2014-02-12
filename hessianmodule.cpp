#include <iostream>
#include <vector>
#include <string>
#include <list>

using namespace std;

void say_hello(const char* name) {
    /* simple example function to test python module is wired up correctly */
    cout << "Hello, " << name << "!\n";
}


#include <boost/shared_ptr.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/class.hpp>
#include <boost/python/list.hpp>
#include <boost/python/stl_iterator.hpp>

#include "zlibdec.h"
#include "hessian_proxy.h"

using namespace boost;
using namespace boost::python;
using namespace hessian;
using namespace hessian::wrappers;
namespace py = boost::python;

void dump(Object* o, py::list& pyl);


void dumplist(const std::list<Object*>& l, py::list& pyl) {
    for (std::list<Object*>::const_iterator i = l.begin(); i != l.end(); i++) {
        dump(*i, pyl);
    }
}


void dumpmap(Map::basic_type l, py::list& pyl) {
    /* XXX: this needs to convert to data, then append to pyl */
    for (Map::basic_type::iterator i = l.begin(); i != l.end(); i++) {
        cout << "{" << endl << "key ";
        cout << (*i).first << endl;
        cout << "value ";
        dump((*i).second, pyl);
        cout << "}" << endl;
    }
}


void dump(Object* o, py::list& pyl) {
    if (Binary* _obj = dynamic_cast<Binary*>(o)) {
        cout << "binary length " << _obj->value().length() << endl;
        return ;
    }
    if (Boolean* _obj = dynamic_cast<Boolean*>(o)) {
        pyl.append(_obj->value());
        return ;
    }
    if (Date* _obj = dynamic_cast<Date*>(o)) {
        pyl.append(_obj->value());
        return ;
    }
    if (Double* _obj = dynamic_cast<Double*>(o)) {
        pyl.append(_obj->value());
        return ;
    }
    if (Integer* _obj = dynamic_cast<Integer*>(o)) {
        pyl.append(_obj->value());
        return ;
    }
    if (List* _obj = dynamic_cast<List*>(o)) {
        dumplist(_obj->value(), pyl);
        return ;
    }
    if (Long* _obj = dynamic_cast<Long*>(o)) {
        pyl.append(_obj->value());
        return ;
    }
    if (Map* _obj = dynamic_cast<Map*>(o)) {
        //dumpmap(_obj->value(), pyl); // XXX: todo
        return ;
    }
    if (Null* _obj = dynamic_cast<Null*>(o)) {
        // dump(_obj); // XXX: todo
        return ;
    }
    if (Ref* _obj = dynamic_cast<Ref*>(o)) {
        // dump(_obj); // XXX: todo
        return ;
    }
    if (Remote* _obj = dynamic_cast<Remote*>(o)) {
        // dump(_obj); // XXX: todo
        return ;
    }
    if (String* _obj = dynamic_cast<String*>(o)) {
        // dump(_obj); // XXX: todo
        return ;
    }
    if (Xml* _obj = dynamic_cast<Xml*>(o)) {
        // dump(_obj); // XXX: todo
        return ;
    }
    if (Fault* _obj = dynamic_cast<Fault*>(o)) {
        // dump(_obj); // XXX: todo
        return ;
    }
    cout << "Warning: dump does not handle " << o->classname() << endl;
}


/*
template<class T>
py::list std_vector_to_py_list(const std::vector<T>& v)
{
    py::object get_iter = py::iterator<std::vector<T> >();
    py::object iter = get_iter(v);
    py::list l(iter);
    return l;
}
*/

template<typename T>
void python_to_vector(boost::python::object o, vector<T>* v) {
    stl_input_iterator<T> begin(o);
    stl_input_iterator<T> end;
    v->clear();
    v->insert(v->end(), begin, end);
}

/*
 * proxy_call instantiates a hessian_proxy object for every call that is made,
 * unfortunately, but this is a safer wrapper around the C++ for the Python to
 * interface with at this point. In the future, it's certainly very possible
 * to instantiate the hessian_proxy object, and pass that in to this call
 * method when the call needs to be performed.
 */
py::list proxy_call(const char* hostspec,
                const char* url,
                const char* call_func,
                py::list& call_func_args,
                bool use_ssl)
{
    py::list pyl;
    ssize_t len_args = py::len(call_func_args);

    /* WARNING - NOTE - BAD CODING STYLE AHEAD
     *
     * Okay, I'm in a rush and this approach will technically work but only up
     * to a point, which is bad. So here's the deal. variadic functions are
     * really awesome at taking a bunch of junk when you don't know what the
     * junk coming in is. However, in some cases you don't even know what the
     * junk is that your going to want to send to a variadic function is. This
     * is when they, well suck. You'd think that you'd just pass in a vector or
     * something like that, but truth is, in a statically typed world, that
     * doesn't really solve every problem. You have to be able to pass in any
     * type, of anything, mixed along other things. This means that you need a
     * vector of pointers that are data-type aware pointers as well.
     *
     * I don't have the kind of time to rewrite hessian_proxy to do this
     * properly. So... now that you have the background on the problem, you'll
     * appreciate this suck code below..
     */
    hessian_proxy proxy(hostspec, url, use_ssl);
    Object* result = NULL;
    try {
        vector<char*> v_args; // XXX: Also bad.. assumes string arguments for simplicity sake
        python_to_vector(call_func_args, &v_args);

        switch (len_args) {
            case 1:
                result = proxy.call(call_func, len_args, &v_args[0]);
                break;
            case 2:
                result = proxy.call(call_func, len_args, &v_args[0],
                                    &v_args[1]);
                break;
            case 3:
                result = proxy.call(call_func, len_args, &v_args[0],
                                    &v_args[1], &v_args[2]);
                break;
            case 4:
                result = proxy.call(call_func, len_args, &v_args[0],
                                    &v_args[1], &v_args[2],
                                    &v_args[3]);
                break;
            case 5:
                result = proxy.call(call_func, len_args, &v_args[0],
                                    &v_args[1], &v_args[2],
                                    &v_args[3], &v_args[4]);
                break;
            case 6:
                result = proxy.call(call_func, len_args, &v_args[0],
                                    &v_args[1], &v_args[2],
                                    &v_args[3], &v_args[4],
                                    &v_args[5]);
                break;
            case 7:
                result = proxy.call(call_func, len_args, &v_args[0],
                                    &v_args[1], &v_args[2],
                                    &v_args[3], &v_args[4],
                                    &v_args[5], &v_args[6]);
                break;
            case 8:
                result = proxy.call(call_func, len_args, &v_args[0],
                                    &v_args[1], &v_args[2],
                                    &v_args[3], &v_args[4],
                                    &v_args[5], &v_args[6],
                                    &v_args[7]);
                break;
            case 9:
                result = proxy.call(call_func, len_args, &v_args[0],
                                    &v_args[1], &v_args[2],
                                    &v_args[3], &v_args[4],
                                    &v_args[5], &v_args[6],
                                    &v_args[7], &v_args[8]);
                break;
            default:
                cout << "too many args (" << len_args << ") passed in" << endl;
            return pyl;
        }

        /* At this point, we have a result, which comes back to us as a list of
         * objects that we need to deserialize.
         */
        auto_ptr<Object> auto_release(result);
        if (List* l = dynamic_cast<List*>(result)) {
            dumplist(*l, pyl);
            return pyl; // return python list of python objects
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
    return pyl; // None object; don't be fooled
}



BOOST_PYTHON_MODULE(hessian) {
    def("say_hello", say_hello);
    def("proxy_call", proxy_call);
    /*
     * Example of how you might attempt to create map directly to hessian_proxy
     * object but if you do, you must figure out how to map the call() method,
     * which was well beyond my boost mastery.
     */
    /*
    class_<hessian_proxy>("hessian_proxy",init<std::string,std::string,bool>())
        .def("url", &hessian_proxy::url)    // example
    ;
    */
}



