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




void primitive_settextcolor(lpp::lpp__context::pointer_t context)
{
    lpp::lpp__value::pointer_t color[2];
    color[0] = context->get_thing("foreground")->get_value();
    color[1] = context->get_thing("background")->get_value();

    lpp::lpp__integer_t col[2];
    for(size_t idx(0); idx < 2; ++idx)
    {
        switch(color[idx]->type())
        {
        case lpp::lpp__value_type_t::LPP__VALUE_TYPE_INTEGER:
            col[idx] = color[idx]->get_integer();
            break;

        case lpp::lpp__value_type_t::LPP__VALUE_TYPE_FLOAT:
            col[idx] = static_cast<lpp::lpp__float_t>(color[idx]->get_float());
            break;

        default:
            throw lpp::lpp__error(context
                                , lpp::lpp__error_code_t::ERROR_CODE_FATAL_INVALID_DATUM
                                , "error"
                                , "text colors need to be a number.");

        }
    }

    if(!lpp::lpp__tty_set_text_colors(col[0], col[1]))
    {
        throw lpp::lpp__error(context
                            , lpp::lpp__error_code_t::ERROR_CODE_SYSTEM_ERROR
                            , "error"
                            , "a system error occurred, text colors could not be changed; is your application connected to a TTY?");
    }
}


// vim: ts=4 sw=4 et nocindent
