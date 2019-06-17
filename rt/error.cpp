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





namespace lpp
{



lpp__error::lpp__error()
    : std::runtime_error("no error")
{
}


lpp__error::lpp__error(lpp__context::const_pointer_t context
                     , lpp__error_code_t code
                     , std::string const & tag
                     , std::string const & message
                     , lpp::lpp__value::pointer_t value)
    : std::runtime_error(message)
    , f_code(code)
    , f_tag(tag)
    , f_value(value)
{
    if(context != nullptr)
    {
        f_filename  = context->get_filename();
        f_procedure = context->get_procedure_name();
        f_line      = context->get_current_line();
        f_primitive = context->get_primitive_name();

        std::const_pointer_cast<lpp__context>(context)->set_error(*this);
    }
}


lpp__error_code_t lpp__error::code() const
{
    return f_code;
}


std::string const & lpp__error::tag() const
{
    return f_tag;
}


lpp__value::pointer_t lpp__error::value() const
{
    return f_value;
}

std::string const & lpp__error::filename() const
{
    return f_filename;
}


std::string const & lpp__error::procedure() const
{
    return f_procedure;
}


lpp__integer_t lpp__error::line() const
{
    return f_line;
}


std::string const & lpp__error::primitive() const
{
    return f_primitive;
}


} // lpp namespace
// vim: ts=4 sw=4 et nocindent

