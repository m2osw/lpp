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




void primitive_pop(lpp::lpp__context::pointer_t context)
{
    lpp::lpp__value::pointer_t stack(context->get_thing("stack")->get_value());
    switch(stack->type())
    {
    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_WORD:
    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_INTEGER:
    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_FLOAT:
    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_BOOLEAN:
        break;

    default:
        throw lpp::lpp__error(context
                            , lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                            , "logic"
                            , "the :STACK argument is expected to be the name of a variable and therefore it has to be a word.");

    }

    std::string const name(stack->to_word());
    if(name.empty())
    {
        throw lpp::lpp__error(context
                            , lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                            , "logic"
                            , "the :STACK argument is not expected to be empty.");
    }

    lpp::lpp__value::pointer_t thing(context->get_thing(name)->get_value());
    switch(thing->type())
    {
    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_LIST:
        {
            lpp::lpp__value::vector_t list(thing->get_list());
            if(list.empty())
            {
                throw lpp::lpp__error(context
                                    , lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                                    , "logic"
                                    , "the stack is not expected to be empty when using \"pop\".");
            }
            context->set_return_value(list[0]);

            list.erase(list.begin());
            context->set_thing(name, std::make_shared<lpp::lpp__value>(list));
        }
        break;

    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_WORD:
    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_INTEGER:
    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_FLOAT:
    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_BOOLEAN:
        {
            std::string word(thing->to_word());
            if(word.empty())
            {
                throw lpp::lpp__error(context
                                    , lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                                    , "logic"
                                    , "the stack is not expected to be empty when using \"pop\".");
            }
            std::string first_letter;
            first_letter += word[0];
            lpp::lpp__value::pointer_t result(std::make_shared<lpp::lpp__value>(first_letter));
            context->set_return_value(result);

            word.erase(0, 1);
            context->set_thing(name, std::make_shared<lpp::lpp__value>(word));
        }
        break;

    default:
        throw lpp::lpp__error(context
                            , lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                            , "logic"
                            , "the :STACK argument is expected to be a list or a word.");

    }
}


// vim: ts=4 sw=4 et nocindent
