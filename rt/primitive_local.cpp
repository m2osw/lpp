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


void create_local(lpp::lpp__context::pointer_t context, lpp::lpp__value::pointer_t name)
{
    if(!name->represents_word())
    {
        throw lpp::lpp__error(context
                            , lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                            , "error"
                            , "\"local\" expect the variable name to be a word.");
    }
std::cerr << "<<SETTING UP \"" << name->to_word() << "\" AS LOCAL>>\n";

    context->set_thing(name->to_word()
                     , lpp::lpp__value::pointer_t()    // not set (i.e. value is nullptr)
                     , lpp::lpp__thing_type_t::LPP__THING_TYPE_PROCEDURE);
}


} // no name namespace


void primitive_local(lpp::lpp__context::pointer_t context)
{
    lpp::lpp__value::pointer_t name(context->get_thing("name")->get_value());

    if(name->type() == lpp::lpp__value_type_t::LPP__VALUE_TYPE_LIST)
    {
        lpp::lpp__thing::pointer_t rest(context->find_thing("rest"));
        if(rest != nullptr)
        {
            throw lpp::lpp__error(context
                                , lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                                , "error"
                                , "\"local\" cannot be used with a rest when used with a list.");
        }

        lpp::lpp__value::vector_t list(name->get_list());
        for(auto item : list)
        {
            create_local(context, item);
        }
    }
    else
    {
        create_local(context, name);

        lpp::lpp__thing::pointer_t rest(context->find_thing("rest"));
        if(rest != nullptr)
        {
            lpp::lpp__value::vector_t list(rest->get_value()->get_list());
            for(auto item : list)
            {
                create_local(context, item);
            }
        }
    }
}


// vim: ts=4 sw=4 et nocindent
