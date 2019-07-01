
<p align="center">
<img alt="lpp" title="A Logo Compiler to transform Logo scripts to binary."
src="https://raw.githubusercontent.com/m2osw/lpp/master/doc/lpp-logo.png" width="180" height="180"/>
</p>

# About lpp

lpp is a compiler that transforms Logo procedures in binary executables
using a C++ compiler as an intermediate step to the compilation process.

The following describes the basic functionality. See the documentation
for additional and detailed information about each instruction and
how to use your shell commands.

# Programs

A program is a _new concept_ in Logo. It means that the instructions
are going to be added to your `int main()` C++ function. We also call
it the `startup()` (i.e. the `main()` calls the program `startup()`
function.)

For example, a `hello-world.logo` program that prints `"Hello world!"`:

    Program
      Print [Hello world!]
    End Program

To compile this program, you use the `lpp` compiler as so:

    lpp hello-world.logo

By default, the output goes to `a.out`. Now e can run our program with:

    ./a.out

And we get the following output:

    Hello world!

Note that I decided to use a PROGRAM keyword to make it clear that a
.logo file includes a program or not. Otherwise you could write lose
commands and that would not work well at all. It also simplifies the
EBNF which I think is great.

# Procedures/Functions

Logo supports procedure definitions. The order within a .logo file is not
important. The procedures have arguments and can output a value.

When a procedure outputs a value it is viewed as a function. Otherwise it
is an actual _procedure_. To declare a procedure you can use either one
of three keywords: TO, PROCEDURE, and FUNCTION. When using TO, the compiler
tries to determine the type. When using PROCEDURE or FUNCTION, you clearly
state the type of procedure you are creating.

    ; The following two definitions are equivalent

    TO SQUARE :N
      OUTPUT :N * :N
    END SQUARE

    FUNCTION SQUARE :N
      OUTPUT :N * :N
    END SQUARE

    ; A procedure can't use the OUTPUT primitive

    PROCEDURE HELLO
      PRINT [Hello world!]
    END HELLO

Your program can then call your procedures or functions just like it can
call a primitive.

# Libraries

_To be worked on still... this is not actually available yet._

The compiler can be used to create libraries. A library is a .logo file
which has no PROGRAM in it. This means you have declarations of functions,
such as presented above, which get compiled in a library (.a or .so file).

The compiler also outputs a declaration header for logo so other .logo
files can import them and a header of functions so other l.cpp files can
compile (i.e. so we can do a #include.)

# Bugs

Please report problems and bugs in this
[github issue queue](https://github.com/m2osw/lpp/issues).


# License

This software is licensed under the GPL version 3.

[Made to Order Software Corp.](https://www.m2osw.com/) offers commercial
licensing if you are interested in using our software in closed source.


vim: ts=4 sw=4 et
-- From the [lpp](https://www.m2osw.com/lpp) project.
