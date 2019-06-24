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




void primitive_combine(lpp::lpp__context::pointer_t context)
{
    lpp::lpp__value::pointer_t thing1(context->get_thing("thing1")->get_value());
    lpp::lpp__value::pointer_t thing2(context->get_thing("thing2")->get_value());

    switch(thing2->type())
    {
    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_INTEGER:
    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_FLOAT:
    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_WORD:
        switch(thing1->type())
        {
        case lpp::lpp__value_type_t::LPP__VALUE_TYPE_INTEGER:
        case lpp::lpp__value_type_t::LPP__VALUE_TYPE_FLOAT:
        case lpp::lpp__value_type_t::LPP__VALUE_TYPE_WORD:
            {
                std::string const word(thing1->to_word() + thing2->to_word());
                context->set_return_value(std::make_shared<lpp::lpp__value>(word));
            }
            break;

        default:
            throw lpp::lpp__error(context
                                , lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                                , "logic"
                                , "the :THING2 argument is expected to be a word when the second item is a word.");

        }
        break;

    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_LIST:
        {
            lpp::lpp__value::vector_t list(thing2->get_list());
            list.insert(list.begin(), thing1);
            lpp::lpp__value::pointer_t result(std::make_shared<lpp::lpp__value>(list));
            context->set_return_value(result);
        }
        break;

    default:
        throw lpp::lpp__error(context
                            , lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                            , "logic"
                            , "the :THING2 argument is expected to be a word or a list.");

    }
}


// vim: ts=4 sw=4 et nocindent
