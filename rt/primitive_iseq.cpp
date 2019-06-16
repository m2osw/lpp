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




void primitive_iseq(lpp::lpp__context::pointer_t context)
{
    lpp::lpp__value::pointer_t number1(context->get_thing("number1")->get_value());
    lpp::lpp__value::pointer_t number2(context->get_thing("number2")->get_value());

    lpp::lpp__number n1(context, number1);
    lpp::lpp__number n2(context, number2);
    if(!n1.is_integer()
    || !n2.is_integer())
    {
        throw lpp::lpp__error(context
                            , "error"
                            , "iseq only accepts integers.");
    }

    lpp::lpp__integer_t s(number1->get_integer());
    lpp::lpp__integer_t e(number2->get_integer());

    lpp::lpp__value::vector_t list;
    list.reserve(labs(s - e) + 1);
    list.push_back(number1);

    if(s != e)
    {
        if(s < e)
        {
            for(++s; s < e; ++s)
            {
                lpp::lpp__value::pointer_t value(std::make_shared<lpp::lpp__value>(s));
                list.push_back(value);
            }
        }
        else // if(s > e)
        {
            for(--s; s > e; --s)
            {
                lpp::lpp__value::pointer_t value(std::make_shared<lpp::lpp__value>(s));
                list.push_back(value);
            }
        }

        // since s != e we can push e as is at the end
        //
        list.push_back(number2);
    }

    lpp::lpp__value::pointer_t result(std::make_shared<lpp::lpp__value>(list));

    context->set_return_value(result);
}


// vim: ts=4 sw=4 et nocindent
