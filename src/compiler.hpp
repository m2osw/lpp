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

#pragma once


#include <memory>
#include <vector>



namespace lpp
{


typedef std::vector<std::string>        string_list_t;


class Compiler
{
public:
    typedef std::shared_ptr<Compiler>     pointer_t;

                            Compiler();

    void                    set_output_object(bool status);
    void                    set_has_program(bool has_program);
    void                    set_verbosity(bool status);
    void                    set_include_debug(bool status);
    void                    set_main_cpp(std::string const & path);
    void                    set_output(std::string const & output);
    void                    add_include_path(std::string const & path);
    void                    add_rpath(std::string const & path);
    void                    add_library_path(std::string const & path);
    void                    add_library(std::string const & path);

    int                     compile();

private:
    bool                    f_output_object = false;
    bool                    f_has_program = false;
    bool                    f_verbose = false;
    bool                    f_include_debug = false;
    string_list_t           f_include_paths = string_list_t();
    std::string             f_main_cpp = std::string("/usr/lib/lpp/main.cpp");
    std::string             f_output = std::string();
    string_list_t           f_rpaths = string_list_t();
    string_list_t           f_library_paths = string_list_t();
    string_list_t           f_libraries = string_list_t();
};



} // lpp namespace

// vim: ts=4 sw=4 et nocindent
