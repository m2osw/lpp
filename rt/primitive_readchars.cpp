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





void primitive_readchars(lpp::lpp__context::pointer_t context)
{
    lpp::lpp__value::pointer_t number(context->get_thing("number")->get_value());
    lpp::lpp__integer_t count(static_cast<lpp::lpp__integer_t>(number->to_float()));
    lpp::lpp__value::pointer_t filename(context->get_thing("filename")->get_value());
    std::string word;
    for(int c(0); c < count; ++c)
    {
        word += lpp::lpp__read_file(context, filename->to_word(), lpp::read_mode_t::READ_MODE_CHAR)->get_word();
    }
    lpp::lpp__value::pointer_t result(std::make_shared<lpp::lpp__value>(word));
    context->set_return_value(result);
}


// vim: ts=4 sw=4 et nocindent
