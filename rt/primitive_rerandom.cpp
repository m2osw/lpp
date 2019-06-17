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




void primitive_rerandom(lpp::lpp__context::pointer_t context)
{
    lpp::lpp__integer_t seed(0);
    lpp::lpp__value::pointer_t rest(context->find_thing("rest")->get_value());
    if(rest != nullptr)
    {
        lpp::lpp__value::vector_t const & items(rest->get_list());
        if(!items.empty())
        {
            if(items.size() > 1)
            {
                throw lpp::lpp__error(context
                                    , lpp::lpp__error_code_t::ERROR_CODE_TOO_MANY_INPUTS
                                    , "error"
                                    , "rerandom accepts 0 or 1 integer only.");
            }
            lpp::lpp__value::pointer_t seed_value(items[0]);
            lpp::lpp__number seed_number(context, seed_value);
            if(!seed_number.is_integer())
            {
                throw lpp::lpp__error(context
                                    , lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                                    , "error"
                                    , "rerandom only accepts integers.");
            }
            seed = seed_number.get_integer();
        }
    }

    srand(seed);
}


// vim: ts=4 sw=4 et nocindent
