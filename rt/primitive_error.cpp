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





void primitive_error(lpp::lpp__context::pointer_t context)
{
    lpp::lpp__value::vector_t list;
    lpp::lpp__value::pointer_t result(std::make_shared<lpp::lpp__value>(list));

    lpp::lpp__error const & e(context->get_error());

    if(!e.tag().empty())
    {
        // not empty, stick all the info we have in that list
        //
        // UCB Logo generates 4 parameters:
        //   an integer code corresponding to the type of error
        //   the text of the error message
        //   the name of the procedure in which the error occurred
        //   the instruction line on which the error occurred.
        //
        result->add_prop("code"
                       , std::make_shared<lpp::lpp__value>(static_cast<lpp::lpp__integer_t>(e.code())));

        result->add_prop("tag"
                       , std::make_shared<lpp::lpp__value>(e.tag()));

        result->add_prop("timestamp"
                       , std::make_shared<lpp::lpp__value>(e.timestamp()));

        result->add_prop("message"
                       , std::make_shared<lpp::lpp__value>(std::string(e.what())));

        if(!e.filename().empty())
        {
            result->add_prop("filename"
                           , std::make_shared<lpp::lpp__value>(e.filename()));
        }

        if(!e.procedure().empty())
        {
            result->add_prop("procedure"
                           , std::make_shared<lpp::lpp__value>(e.procedure()));
        }

        if(e.line() != 0)
        {
            result->add_prop("line"
                          , std::make_shared<lpp::lpp__value>(e.line()));
        }

        if(!e.primitive().empty())
        {
            result->add_prop("primitive"
                          , std::make_shared<lpp::lpp__value>(e.primitive()));
        }

        if(e.value() != nullptr)
        {
            result->add_prop("value"
                          , e.value());
        }

        if(e.caught())
        {
            result->add_prop("caught"
                          , std::make_shared<lpp::lpp__value>(true));
        }
    }

    context->set_return_value(result);
}


// vim: ts=4 sw=4 et nocindent
