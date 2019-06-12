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
#include "lpp.hpp"

// C++ lib
//
#include <iostream>


// program is always defined in the lpp_startup() function
//
void lpp__startup(lpp::lpp__context::pointer_t context);


int main(int argc, char * argv[])
{
    std::cout << "-- setup context & `arguments`... --\n";

    lpp::lpp__context::pointer_t context(std::make_shared<lpp::lpp__context>());
    context->set_global(context);

    lpp__startup(context);

    lpp::lpp__value::pointer_t exit_code(context->get_returned_value());
    if(exit_code != nullptr)
    {
        if(exit_code->type() != lpp::lpp__value_type_t::LPP__VALUE_TYPE_INTEGER)
        {
            std::cerr << "error: exit code is expected to be an integer.\n";
            return 1;
        }
        else
        {
std::cout << "-- got an exit code! --\n";
            return exit_code->get_integer();
        }
    }

std::cout << "-- no exit code, return 0 by default --\n";
    return 0;
}


// vim: ts=4 sw=4 et nocindent
