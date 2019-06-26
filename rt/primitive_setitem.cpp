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



void primitive_setitem(lpp::lpp__context::pointer_t context)
{
    lpp::lpp__value::pointer_t index(context->get_thing("position")->get_value());
    lpp::lpp__value::pointer_t list(context->get_thing("list")->get_value());
    lpp::lpp__value::pointer_t value(context->get_thing("value")->get_value());

    if(!index->represents_float())
    {
        throw lpp::lpp__error(context
                            , lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                            , "error"
                            , "\"setitem\" first parameter must be a number.");
    }

    list = std::make_shared<lpp::lpp__value>(list->get_list());
    context->set_return_value(list);

    if(list->type() != lpp::lpp__value_type_t::LPP__VALUE_TYPE_LIST)
    {
        throw lpp::lpp__error(context
                            , lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                            , "error"
                            , "\"mdsetitem\" second parameters must be a list of lists.");
    }

    lpp::lpp__integer_t pos(static_cast<lpp::lpp__integer_t>(index->to_float()));
    if(pos <= 0)
    {
        throw lpp::lpp__error(context
                            , lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                            , "error"
                            , "\"mdsetitem\" first parameter must be a list of valid positions (a positive number).");
    }
    --pos;     // C++ position start at 0

    lpp::lpp__value::vector_t l(list->get_list());
    if(static_cast<size_t>(pos) >= l.size())
    {
        throw lpp::lpp__error(context
                            , lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                            , "error"
                            , "\"mdsetitem\" index must be within the number of items in the list and sub-lists.");
    }

    l[pos] = value;
    list->set_list(l);
}


// vim: ts=4 sw=4 et nocindent
