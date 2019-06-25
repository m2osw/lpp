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




void primitive_setcursor(lpp::lpp__context::pointer_t context)
{
    lpp::lpp__value::pointer_t position(context->get_thing("position")->get_value());

    if(position->type() != lpp::lpp__value_type_t::LPP__VALUE_TYPE_LIST)
    {
        throw lpp::lpp__error(context
                            , lpp::lpp__error_code_t::ERROR_CODE_FATAL_INVALID_DATUM
                            , "error"
                            , "\"setcursor\" expects a list as its first parameter.");
    }
    lpp::lpp__value::vector_t const & list(position->get_list());

    if(list.size() < 2)
    {
        throw lpp::lpp__error(context
                            , lpp::lpp__error_code_t::ERROR_CODE_FATAL_INVALID_DATUM
                            , "error"
                            , "\"setcursor\" expects a list of at least two coordinates.");
    }

    lpp::lpp__integer_t pos[2];
    for(size_t idx(0); idx < 2; ++idx)
    {
        switch(list[idx]->type())
        {
        case lpp::lpp__value_type_t::LPP__VALUE_TYPE_INTEGER:
            pos[idx] = list[idx]->get_integer();
            break;

        case lpp::lpp__value_type_t::LPP__VALUE_TYPE_FLOAT:
            pos[idx] = static_cast<lpp::lpp__float_t>(list[idx]->get_float());
            break;

        default:
            throw lpp::lpp__error(context
                                , lpp::lpp__error_code_t::ERROR_CODE_FATAL_INVALID_DATUM
                                , "error"
                                , "cursor coordinates need to be a number.");

        }
    }

    if(!lpp::lpp__tty_set_cursor(pos[0], pos[1]))
    {
        throw lpp::lpp__error(context
                            , lpp::lpp__error_code_t::ERROR_CODE_SYSTEM_ERROR
                            , "error"
                            , "a system error occurred, cursor position could not be set; is your application connected to a TTY?");
    }
}


// vim: ts=4 sw=4 et nocindent
