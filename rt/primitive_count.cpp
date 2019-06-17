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




void primitive_count(lpp::lpp__context::pointer_t context)
{
    lpp::lpp__value::pointer_t thing(context->get_thing("thing")->get_value());

    lpp::lpp__integer_t count(0);
    switch(thing->type())
    {
    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_BOOLEAN:
        {
            std::string const n(thing->to_word());
            count = n.length();
        }
        break;

    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_INTEGER:
        {
            std::string const n(std::to_string(thing->get_integer()));
            count = n.length();
        }
        break;

    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_FLOAT:
        {
            std::string const n(std::to_string(thing->get_float()));
            count = n.length();
        }
        break;

    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_WORD:
        {
            std::string const n(thing->get_word());
            count = n.length();
        }
        break;

    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_LIST:
        {
            lpp::lpp__value::vector_t const & l(thing->get_list());
            count = l.size();
        }
        break;

    default:
        throw lpp::lpp__error(context
                            , lpp::lpp__error_code_t::ERROR_CODE_FATAL_INVALID_DATUM
                            , "error"
                            , "count used with an unexpected parameter type.");

    }

    lpp::lpp__value::pointer_t result(std::make_shared<lpp::lpp__value>(count));
    context->set_return_value(result);
}


// vim: ts=4 sw=4 et nocindent
