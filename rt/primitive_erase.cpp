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





void primitive_erase(lpp::lpp__context::pointer_t context)
{
    lpp::lpp__value::pointer_t names(context->get_thing("names")->get_value());

    if(names->type() != lpp::lpp__value_type_t::LPP__VALUE_TYPE_LIST)
    {
        throw lpp::lpp__error(context
                            , lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                            , "error"
                            , "the :NAMES argument is expected to be a list.");
    }

    lpp::lpp__value::vector_t list(names->get_list());
    for(auto & it : list)
    {
        std::string const item_name(it->to_word());
        context->erase_thing(item_name);
    }
}


// vim: ts=4 sw=4 et nocindent
