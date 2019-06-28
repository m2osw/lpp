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




void primitive_standout(lpp::lpp__context::pointer_t context)
{
    class standout
    {
    public:
        standout()
        {
            lpp::lpp__tty_set_standout(true);
        }

        ~standout()
        {
            lpp::lpp__tty_set_standout(false);
        }
    };
    standout so;

    lpp::lpp__value::pointer_t thing(context->get_thing("thing")->get_value());
    std::cout << thing->to_string();
    lpp::lpp__thing::pointer_t rest(context->find_thing("rest"));
    if(rest != nullptr)
    {
        // reimplement the list because we need to not have spaces between
        // each element
        //
        auto list(rest->get_value()->get_list());
        size_t const max(list.size());
        for(size_t i(0); i < max; ++i)
        {
            std::cout << list[i]->to_string();
        }
    }
}


// vim: ts=4 sw=4 et nocindent
