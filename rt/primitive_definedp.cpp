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




void primitive_definedp(lpp::lpp__context::pointer_t context)
{
    lpp::lpp__value::pointer_t procedure_name(context->get_thing("name")->get_value());
    if(procedure_name->type() != lpp::lpp__value_type_t::LPP__VALUE_TYPE_WORD)
    {
        throw lpp::lpp__error(context
                            , lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                            , "logic"
                            , "the :NAME argument is expected to be a word naming a procedure.");
    }
    std::string const name(procedure_name->to_word());
    if(name.empty())
    {
        throw lpp::lpp__error(context
                            , lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                            , "logic"
                            , "the :NAME argument is not expected to be empty.");
    }
    lpp::lpp__procedure_info_t const * proc(lpp::find_procedure(name));
    lpp::lpp__value::pointer_t result(std::make_shared<lpp::lpp__value>(
                   proc != nullptr
                && (proc->f_flags & lpp::PROCEDURE_FLAG_PROCEDURE) != 0));
    context->set_return_value(result);
}


// vim: ts=4 sw=4 et nocindent
