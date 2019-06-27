// lpp -- Logo to binary compiler
// Copyright (C) 2019  Made to Order Software Corporation
// https://www.m2osw.com/lpp
// contact@m2osw.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// self
//
#include "compiler.hpp"

// lpp compiler
//
#include "exception.hpp"

// boost lib
//
#include <boost/algorithm/string.hpp>

// C++ lib
//
#include <iostream>
#include <limits>
#include <sstream>




namespace lpp
{


Compiler::Compiler()
{
}


void Compiler::set_output_object(bool status)
{
    f_output_object = status;
}


void Compiler::set_has_program(bool has_program)
{
    f_has_program = has_program;
}


void Compiler::set_main_cpp(std::string const & path)
{
    f_main_cpp = path;
}


void Compiler::set_output(std::string const & output)
{
    f_output = output;
}


void Compiler::set_verbosity(bool status)
{
    f_verbose = status;
}


void Compiler::add_include_path(std::string const & path)
{
    f_include_paths.push_back(path);
}


void Compiler::add_rpath(std::string const & path)
{
    f_rpaths.push_back(path);
}


void Compiler::add_library_path(std::string const & path)
{
    f_library_paths.push_back(path);
}


void Compiler::add_library(std::string const & path)
{
    f_libraries.push_back(path);
}


int Compiler::compile()
{
    std::vector<std::string> option;

    // compiler
    //
    option.push_back("g++");

    // standard
    //
    option.push_back("-std=c++14");

    // debug and optimizations
    //
    option.push_back("-g");
    option.push_back("-O0");

    // include paths
    //
    for(auto inc : f_include_paths)
    {
        option.push_back("-I");
        option.push_back(inc);
    }

    // the C++ input file
    //
    option.push_back("l.cpp");

    // the main if necessary (i.e. "program ... end program;" block present)
    //
    if(f_has_program)
    {
        option.push_back(f_main_cpp);
    }

    // include paths
    //
    for(auto rp : f_rpaths)
    {
        option.push_back("-Xlinker");
        option.push_back("-rpath");
        option.push_back("-Xlinker");
        option.push_back(rp);
    }

    // the main if necessary (i.e. "program ... end program;" block present)
    //
    for(auto lp : f_library_paths)
    {
        option.push_back("-L");
        option.push_back(lp);
    }

    // libraries
    //
    for(auto lib : f_libraries)
    {
        option.push_back("-l");
        option.push_back(lib);
    }
    option.push_back("-l");
    option.push_back("lpprt");

    // output filename
    //
    if(!f_output.empty())
    {
        option.push_back("-o");
        option.push_back(f_output);
    }

    // build the command
    //
    std::string cmd;
    for(auto o : option)
    {
        if(!cmd.empty())
        {
            cmd += " ";
        }
        cmd += o;
    }

    if(f_verbose)
    {
        std::cerr << cmd << std::endl;
    }

    // run the command
    //
    return system(cmd.c_str());
}




} // lpp namespace

// vim: ts=4 sw=4 et nocindent
