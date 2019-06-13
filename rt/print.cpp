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


namespace
{


void print_value(lpp::lpp__value::pointer_t value, int depth = 0)
{
    switch(value->type())
    {
    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_SPECIAL:
        std::cout << "special...\n";
        break;

    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_BOOLEAN:
        std::cout << (value->get_boolean() ? "true" : "false");
        break;

    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_INTEGER:
        std::cout << value->get_integer();
        break;

    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_FLOAT:
        std::cout << value->get_float();
        break;

    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_STRING:
        std::cout << value->get_string();
        break;

    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_LIST:
        if(depth > 0)
        {
            std::cout << "[";
        }
        {
            auto list(value->get_list());
            size_t const max(list.size());
            if(max > 0)
            {
                print_value(list[0], depth + 1);
                for(size_t i(1); i < max; ++i)
                {
                    std::cout << " ";
                    print_value(list[i], depth + 1);
                }
            }
        }
        if(depth > 0)
        {
            std::cout << "]";
        }
        break;

    }
}


} // no name namespace


void print(lpp::lpp__context::pointer_t context)
{
    lpp::lpp__value::pointer_t msg(context->get_thing("msg")->get_value());
    print_value(msg);
    lpp::lpp__value::pointer_t rest(context->find_thing("rest")->get_value());
    if(rest != nullptr)
    {
        std::cout << " ";
        print_value(rest);
    }
    std::cout << "\n";
}


// vim: ts=4 sw=4 et nocindent
