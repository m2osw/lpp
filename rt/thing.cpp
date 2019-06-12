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



lpp__thing_type_t lpp__thing::get_type() const
{
    return f_type;
}


void lpp__thing::set_type(lpp__thing_type_t type)
{
    f_type = type;
}


bool lpp__thing::is_set() const
{
    return f_value != nullptr;
}


lpp__value::pointer_t lpp__thing::get_value() const
{
    return f_value;
}


void lpp__thing::set_value(lpp__value::pointer_t value)
{
    f_value = value;
}


void lpp__thing::set_value(lpp__value::pointer_t value, lpp__thing_type_t type)
{
    f_type = type;
    f_value = value;
}





} // lpp namespace
// vim: ts=4 sw=4 et nocindent
