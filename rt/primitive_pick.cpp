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




namespace
{


lpp::lpp__integer_t rnd(lpp::lpp__integer_t max)
{
    // TODO: get a better random function
    return ((static_cast<lpp::lpp__integer_t>(rand()) <<  0)
          ^ (static_cast<lpp::lpp__integer_t>(rand()) << 16)
          ^ (static_cast<lpp::lpp__integer_t>(rand()) << 32)
          ^ (static_cast<lpp::lpp__integer_t>(rand()) << 48)) % max;
}


}



void primitive_pick(lpp::lpp__context::pointer_t context)
{
    lpp::lpp__value::pointer_t thing(context->get_thing("thing")->get_value());

    lpp::lpp__value::pointer_t result(std::make_shared<lpp::lpp__value>());

    switch(thing->type())
    {
    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_BOOLEAN:
    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_INTEGER:
    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_FLOAT:
    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_WORD:
        {
            std::string const word(thing->to_word());
            if(word.empty())
            {
                throw lpp::lpp__error(context
                                    , lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                                    , "error"
                                    , "pick parameter must be a non-empty word or list.");
            }
            result->set_word(word.substr(rnd(word.length()), 1));
        }
        break;

    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_LIST:
        {
            lpp::lpp__value::vector_t const & l(thing->get_list());
            if(l.empty())
            {
                throw lpp::lpp__error(context
                                    , lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                                    , "error"
                                    , "pick parameter must be a non-empty word or list.");
            }
            result = l[rnd(l.size())];
        }
        break;

    default:
        throw lpp::lpp__error(context
                            , lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                            , "error"
                            , "item used with an unexpected parameter type.");

    }

    context->set_return_value(result);
}


// vim: ts=4 sw=4 et nocindent
