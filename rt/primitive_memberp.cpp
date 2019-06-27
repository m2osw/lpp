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




void primitive_memberp(lpp::lpp__context::pointer_t context)
{
    lpp::lpp__value::pointer_t thing1(context->get_thing("thing1")->get_value());
    lpp::lpp__value::pointer_t thing2(context->get_thing("thing2")->get_value());

    switch(thing2->type())
    {
    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_BOOLEAN:
    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_INTEGER:
    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_FLOAT:
    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_WORD:
        if(!thing1->represents_word())
        {
            throw lpp::lpp__error(context
                                , lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                                , "error"
                                , "\"memberp\" first parameter must be a word when the second parameter is a word.");
        }

        {
            std::string word1(thing1->to_word());
            std::string word2(thing2->to_word());
            std::string::size_type pos(word2.find(word1));
            lpp::lpp__value::pointer_t result(std::make_shared<lpp::lpp__value>(pos != std::string::npos));
            context->set_return_value(result);
        }
        break;

    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_LIST:
        {
            lpp::lpp__value::vector_t l(thing2->get_list());

            auto it(std::find_if(
                      l.begin()
                    , l.end()
                    , [&context, &thing1, &l](auto const & item)
                    {
                        lpp::lpp__integer_t r(lpp::lpp__number::compare_values(
                                                  context
                                                , thing1
                                                , item
                                                , false));
                        return r == 0;
                    }));
            lpp::lpp__value::pointer_t result(std::make_shared<lpp::lpp__value>(it != l.end()));
            context->set_return_value(result);
        }
        break;

    default:
        throw lpp::lpp__error(context
                            , lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                            , "error"
                            , "\"memberp\" used with an unexpected second parameter type.");

    }
}


// vim: ts=4 sw=4 et nocindent
