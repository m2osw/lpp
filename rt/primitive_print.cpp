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




void primitive_print(lpp::lpp__context::pointer_t context)
{
    lpp::lpp__value::pointer_t thing(context->get_thing("thing")->get_value());
    lpp::lpp__write_file(context, std::string(), thing);
    lpp::lpp__thing::pointer_t rest(context->find_thing("rest"));
    if(rest != nullptr)
    {
        auto list(rest->get_value()->get_list());
        size_t const max(list.size());
        for(size_t i(0); i < max; ++i)
        {
            lpp::lpp__write_file(context, std::string(), " ");
            lpp::lpp__write_file(context, std::string(), list[i]);
        }
    }
    lpp::lpp__write_file(context, std::string(), "\n");
}


// vim: ts=4 sw=4 et nocindent
