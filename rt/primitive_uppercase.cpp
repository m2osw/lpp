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

#include "utf8_iterator.hpp"

// C++ lib
//
#include <iostream>




void primitive_uppercase(lpp::lpp__context::pointer_t context)
{
    lpp::lpp__value::pointer_t w(context->get_thing("word")->get_value());
    std::string word(w->to_word());

    std::string str;
    lpp::utf8_iterator it(word);
    while(it != word.end())
    {
        str += lpp::wctombs(std::towupper(*it));
        ++it;
    }

    lpp::lpp__value::pointer_t result(std::make_shared<lpp::lpp__value>(str));
    context->set_return_value(result);
}


// vim: ts=4 sw=4 et nocindent
