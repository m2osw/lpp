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




void primitive_remprop(lpp::lpp__context::pointer_t context)
{
    lpp::lpp__value::pointer_t listname(context->get_thing("plistname")->get_value());
    lpp::lpp__value::pointer_t propname(context->get_thing("propname")->get_value());

    if(!listname->represents_word())
    {
        throw lpp::lpp__error(context
                            , lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                            , "error"
                            , "\"pprop\" first parameter must be a word representing the name of the property list.");
    }
    if(!propname->represents_word())
    {
        throw lpp::lpp__error(context
                            , lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                            , "error"
                            , "\"pprop\" second parameter must be a word representing the property name.");
    }

    context->set_property(
            listname->to_word()
          , propname->to_word()
          , lpp::lpp__value::pointer_t());   // setting to null is like removing
}


// vim: ts=4 sw=4 et nocindent
