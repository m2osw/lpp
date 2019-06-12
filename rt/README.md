
# Run-Time Library

The lpp runtime (rt) library is composed of mainly three things:

1. a header that the lpp generated files include (lpp.hpp)
2. a startup script in case you are writing a program (i.e. main.cpp)
3. primitives

The lpp compiler will know which part to include when. A final installed
version will have the files precompiled (.o) so it can simply link them.
At some point we plan to offer a shared library for the primitives too.
That way running many logo scripts will generate a smaller memory foot
print.

-- From the [lpp](https://www.m2osw.com/lpp) project.
