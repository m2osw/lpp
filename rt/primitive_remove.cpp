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




void primitive_remove(lpp::lpp__context::pointer_t context)
{
    lpp::lpp__value::pointer_t thing(context->get_thing("thing")->get_value());
    lpp::lpp__value::pointer_t list(context->get_thing("list")->get_value());

    lpp::lpp__value::pointer_t result(std::make_shared<lpp::lpp__value>());

    switch(list->type())
    {
    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_BOOLEAN:
    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_INTEGER:
    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_FLOAT:
    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_WORD:
        {
            if(!list->represents_word())
            {
                throw lpp::lpp__error(context
                                    , lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                                    , "error"
                                    , "\"remove\" first parameter must be a word with the second parameter is a word.");
            }
            std::string const pattern(thing->to_word());
            std::string word(list->to_word());
            for(std::string::size_type pos(0);;)
            {
                pos = word.find(pattern, pos);
                if(pos == std::string::npos)
                {
                    break;
                }
                word.erase(word.begin() + pos, word.begin() + pos + pattern.length());
            }
            result->set_word(word);
        }
        break;

    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_LIST:
        {
            lpp::lpp__value::vector_t l(list->get_list());
            size_t idx(l.size());
            while(idx > 0)
            {
                --idx;

                lpp::lpp__integer_t r(lpp::lpp__number::compare_values(
                                          context
                                        , thing
                                        , l[idx]
                                        , false));
                if(r == 0)
                {
                    l.erase(l.begin() + idx);
                }
            }
            result->set_list(l);
        }
        break;

    default:
        throw lpp::lpp__error(context
                            , lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                            , "error"
                            , "item used with an unexpected second parameter type.");

    }

    context->set_return_value(result);
}


// vim: ts=4 sw=4 et nocindent
