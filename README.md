
Fast Python Hessian Module
==========================

![The Hessian](http://animationreview.files.wordpress.com/2013/03/bunker-hill-bunny-c2a9-warner-brothers.jpg)


I don't know what this image says about a Hessian api, but at any rate, if you
need to interface with a Hessian api (They) and the regular Python version is
just not performing fast enough, then this module is what you're looking for.
It simply wraps the existing C++ version of the Hessian protocol as a Python
module for your computing pleasure. Now you can get all the speed of the
low-level compiled version from within your Python code without feeling like
the api looks/feels like the picture above.

WARNING
-------

*THIS MODULE IS NOT FULLY COMPLETED!* While it wraps the C++ code, the
conversion of the objects into Python objects was not fully completed. The
reason for this was the discovery that the C++ code parse for v1.0 of the
Hessian spec which was not the version I needed to connect to. Rather than
invest the significant amount of energy that was required to update a library I
didn't write to v2.0, I decided to abort and find a non-Hessian solution to the
original project that was needing a faster Hessian implementation. My apologies
for the half-implemented dump() function in the code below, but feel free to
submit patches if you work on it.


References
----------

 + [Existing Python Hessian module](https://github.com/bgilmore/mustaine)
 + [Hessian Protocol](http://hessian.caucho.com)
 + [C++ version of Protocol](http://sourceforge.net/projects/hessiancpp/)


