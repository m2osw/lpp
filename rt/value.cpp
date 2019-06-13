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



lpp__value::lpp__value(lpp__special_value_t value)
    : f_value(value)
{
}


lpp__value::lpp__value(bool value)
    : f_value(value)
{
}


lpp__value::lpp__value(lpp__integer_t value)
    : f_value(value)
{
}


lpp__value::lpp__value(lpp__float_t value)
    : f_value(value)
{
}


lpp__value::lpp__value(std::string const & value)
    : f_value(value)
{
}


lpp__value::lpp__value(vector_t const & value)
    : f_value(value)
{
}


lpp::lpp__value_type_t lpp__value::type() const
{
    return static_cast<lpp__value_type_t>(f_value.which());
}


bool lpp__value::is_set() const
{
    return type() != lpp__value_type_t::LPP__VALUE_TYPE_SPECIAL
        || boost::get<lpp__special_value_t>(f_value) != lpp__special_value_t::LPP__SEPCIAL_VALUE_NOT_SET;
}


bool lpp__value::get_boolean() const
{
    return boost::get<bool>(f_value);
}


lpp__integer_t lpp__value::get_integer() const
{
    return boost::get<lpp__integer_t>(f_value);
}


lpp__float_t lpp__value::get_float() const
{
    return boost::get<lpp__float_t>(f_value);
}


std::string const & lpp__value::get_string() const
{
    return boost::get<std::string>(f_value);
}


lpp__value::vector_t const & lpp__value::get_list() const
{
    return boost::get<vector_t>(f_value);
}


void lpp__value::unset()
{
    f_value = lpp__special_value_t::LPP__SEPCIAL_VALUE_NOT_SET;
}


void lpp__value::set_boolean(bool value)
{
    f_value = value;
}


void lpp__value::set_integer(lpp__integer_t value)
{
    f_value = value;
}


void lpp__value::set_float(lpp__float_t value)
{
    f_value = value;
}


void lpp__value::set_string(std::string const & value)
{
    f_value = value;
}


void lpp__value::set_list(vector_t const & value)
{
    f_value = value;
}






} // lpp namespace
// vim: ts=4 sw=4 et nocindent
