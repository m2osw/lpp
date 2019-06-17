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




void primitive_rseq(lpp::lpp__context::pointer_t context)
{
    lpp::lpp__value::pointer_t number3(context->get_thing("number3")->get_value());
    lpp::lpp__number n3(context, number3);
    if(!n3.is_integer())
    {
        throw lpp::lpp__error(context
                            , lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                            , "error"
                            , "rseq only accepts an integer as its third parameter.");
    }
    lpp::lpp__integer_t const count(n3.get_integer());
    if(count < 0)
    {
        throw lpp::lpp__error(context
                            , lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                            , "error"
                            , "rseq expect the count parameter to be a positive number.");
    }

    lpp::lpp__value::pointer_t number1(context->get_thing("number1")->get_value());
    lpp::lpp__number n1(context, number1);
    lpp::lpp__float_t start(0.0);
    if(n1.is_integer())
    {
        start = static_cast<lpp::lpp__float_t>(n1.get_integer());
    }
    else
    {
        start = n1.get_float();
    }

    lpp::lpp__value::pointer_t number2(context->get_thing("number2")->get_value());
    lpp::lpp__number n2(context, number2);
    lpp::lpp__float_t end(0.0);
    if(n2.is_integer())
    {
        end = static_cast<lpp::lpp__float_t>(n2.get_integer());
    }
    else
    {
        end = n2.get_float();
    }

    lpp::lpp__float_t gap(end - start);

    lpp::lpp__value::vector_t list;
    list.reserve(count);
    for(lpp::lpp__integer_t idx(0); idx < count; ++idx)
    {
        lpp::lpp__value::pointer_t value(std::make_shared<lpp::lpp__value>(gap * static_cast<lpp::lpp__float_t>(idx) + start));
        list.push_back(value);
    }

    lpp::lpp__value::pointer_t result(std::make_shared<lpp::lpp__value>(list));

    context->set_return_value(result);
}


// vim: ts=4 sw=4 et nocindent
