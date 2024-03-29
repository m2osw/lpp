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




void primitive_word(lpp::lpp__context::pointer_t context)
{
    lpp::lpp__value::pointer_t w1(context->get_thing("word1")->get_value());
    std::string word(w1->to_word());

    lpp::lpp__value::pointer_t w2(context->get_thing("word2")->get_value());
    word += w2->to_word();

    lpp::lpp__thing::pointer_t rest(context->find_thing("rest"));
    if(rest != nullptr)
    {
        lpp::lpp__value::vector_t const & items(rest->get_value()->get_list());
        for(auto const & r : items)
        {
            word += r->to_word();
        }
    }

    lpp::lpp__value::pointer_t result(std::make_shared<lpp::lpp__value>(word));
    context->set_return_value(result);
}


// vim: ts=4 sw=4 et nocindent
