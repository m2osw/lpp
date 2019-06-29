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




void primitive_firstbyte(lpp::lpp__context::pointer_t context)
{
    lpp::lpp__value::pointer_t word(context->get_thing("word")->get_value());

    lpp::lpp__value::pointer_t result(std::make_shared<lpp::lpp__value>());

    switch(word->type())
    {
    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_BOOLEAN:
        if(word->get_boolean())
        {
            result->set_word("t");
        }
        else
        {
            result->set_word("f");
        }
        break;

    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_INTEGER:
        {
            std::string const n(std::to_string(word->get_integer()));
            result->set_word(n.substr(0, 1));
        }
        break;

    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_FLOAT:
        {
            std::string const n(std::to_string(word->get_float()));
            result->set_word(n.substr(0, 1));
        }
        break;

    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_WORD:
        {
            std::string const n(word->get_word());
            if(n.empty())
            {
                throw lpp::lpp__error(context
                                    , lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                                    , "error"
                                    , "\"firstbyte\" cannot be used against an empty word.");
            }
            result->set_word(n.substr(0, 1));
        }
        break;

    default:
        throw lpp::lpp__error(context
                            , lpp::lpp__error_code_t::ERROR_CODE_FATAL_INVALID_DATUM
                            , "error"
                            , "\"firstbyte\" used with an unexpected parameter type.");

    }

    context->set_return_value(result);
}


// vim: ts=4 sw=4 et nocindent
