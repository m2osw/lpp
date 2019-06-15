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


lpp::lpp__integer_t rnd()
{
    // TODO: get a better random function
    return (static_cast<lpp::lpp__integer_t>(rand()) <<  0)
         ^ (static_cast<lpp::lpp__integer_t>(rand()) << 16)
         ^ (static_cast<lpp::lpp__integer_t>(rand()) << 32)
         ^ (static_cast<lpp::lpp__integer_t>(rand()) << 48);
}


}



void primitive_random(lpp::lpp__context::pointer_t context)
{
    lpp::lpp__value::pointer_t n1(context->get_thing("number")->get_value());
    lpp::lpp__number start(context, n1);
    if(!start.is_integer())
    {
        throw lpp::lpp__error(context
                            , "error"
                            , "random only accepts integers.");
    }
    lpp::lpp__integer_t s(start.get_integer());

    lpp::lpp__value::pointer_t rest(context->find_thing("rest")->get_value());
    if(rest != nullptr)
    {
        lpp::lpp__value::vector_t const & items(rest->get_list());
        if(!items.empty())
        {
            if(items.size() > 1)
            {
                throw lpp::lpp__error(context
                                    , "error"
                                    , "random accepts 1 or 2 integers only.");
            }
            lpp::lpp__value::pointer_t n2(items[0]);
            lpp::lpp__number end(context, n2);
            if(!end.is_integer())
            {
                throw lpp::lpp__error(context
                                    , "error"
                                    , "random only accepts integers.");
            }
            lpp::lpp__integer_t e(end.get_integer());
            lpp::lpp__integer_t r(rnd() % (e - s + 1) + s);
            lpp::lpp__value::pointer_t result(std::make_shared<lpp::lpp__value>(r));
            context->set_return_value(result);
            return;
        }
    }

    // only one number, use it as the max (non-inclusive)
    //
    {
        lpp::lpp__integer_t r(rnd() % s);
        lpp::lpp__value::pointer_t result(std::make_shared<lpp::lpp__value>(r));
        context->set_return_value(result);
    }
}


// vim: ts=4 sw=4 et nocindent
