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
#include <cstring>
#include <sstream>



namespace lpp
{



//lpp__number::lpp__number(lpp__context::pointer_t context)
//    : f_context(context)
//{
//}
//
//
//lpp__number::lpp__number(lpp__context::pointer_t context, lpp__integer_t number)
//    : f_context(context)
//    , f_integer(number)
//{
//}
//
//
//lpp__number::lpp__number(lpp__context::pointer_t context, lpp__float_t number)
//    : f_context(context)
//    , f_is_integer(false)
//    , f_float(number)
//{
//}


lpp__number::lpp__number(lpp__context::pointer_t context, std::string const & number)
    : f_context(context)
{
    char * end(nullptr);
    if(number.find('.') != std::string::npos)
    {
        f_float = std::strtod(number.c_str(), &end);
        f_is_integer = false;
        if(end == nullptr
        || number.c_str() == end
        || *end != '\0')
        {
            throw lpp__error(f_context
                           , "error"
                           , "invalid float");
        }
    }
    else
    {
        f_integer = std::strtoll(number.c_str(), &end, 10);
        if(end == nullptr
        || number.c_str() == end
        || *end != '\0')
        {
            throw lpp__error(f_context
                           , "error"
                           , "invalid integer");
        }
    }
}


lpp__number::lpp__number(lpp__context::pointer_t context, lpp__value::pointer_t number)
    : f_context(context)
{
    switch(number->type())
    {
    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_INTEGER:
        f_integer = number->get_integer();
        break;

    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_FLOAT:
        f_is_integer = false;
        f_float = number->get_float();
        break;

    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_WORD:
        {
            lpp__number const n(context, number->get_word());
            *this = n;
        }
        break;

    default:
        throw lpp__error(f_context
                       , "error"
                       , "number expected");

    }
}



bool lpp__number::is_integer() const
{
    return f_is_integer;
}


lpp__integer_t lpp__number::get_integer() const
{
    return f_integer;
}


lpp__float_t lpp__number::get_float() const
{
    return f_float;
}


lpp__value::pointer_t lpp__number::to_value() const
{
    if(f_is_integer)
    {
        return std::make_shared<lpp__value>(f_integer);
    }
    else
    {
        return std::make_shared<lpp__value>(f_float);
    }
}


void lpp__number::save_as_return_value()
{
    f_context->set_return_value(to_value());
}



void lpp__number::apply_unary(
           lpp__integer_t (*i)(lpp__integer_t lhs, lpp__integer_t rhs)
         , lpp__float_t (*f)(lpp__float_t lhs, lpp__float_t rhs))
{
    int const state(f_is_integer ? 0 : 1);

    if(state != 0
    && f == nullptr)
    {
        throw lpp__error(f_context
                       , "error"
                       , "function not valid against floating points");
        return;
    }

    switch(state)
    {
    case 0:
        f_integer = i(f_integer, 0);
        break;

    case 1:
        f_float = f(f_float, 0.0);
        break;

    }
}


void lpp__number::apply_binary(lpp__value::pointer_t number
         , lpp__integer_t (*i)(lpp__integer_t lhs, lpp__integer_t rhs)
         , lpp__float_t (*f)(lpp__float_t lhs, lpp__float_t rhs))
{
    lpp__number rhs(f_context, number);

    int const state((    f_is_integer ? 0 : 1)
                  | (rhs.f_is_integer ? 0 : 2));

    if(state != 0
    && f == nullptr)
    {
        throw lpp__error(f_context
                       , "error"
                       , "function not valid against floating points");
        return;
    }

    switch(state)
    {
    case 0:
        f_integer = i(f_integer, rhs.f_integer);
        break;

    case 1:
        f_float = f(f_float, static_cast<lpp__float_t>(rhs.f_integer));
        break;

    case 2:
        f_is_integer = false;
        f_float = f(static_cast<lpp__float_t>(f_integer), rhs.f_float);
        break;

    case 3:
        f_float = f(f_float, rhs.f_float);
        break;

    }
}


void lpp__number::compute(
              lpp__context::pointer_t context
            , std::string const & lhs_name
            , std::string const & rhs_name
            , std::string const & rest_name
            , lpp__integer_t (*i)(lpp__integer_t lhs, lpp__integer_t rhs)
            , lpp__float_t (*f)(lpp__float_t lhs, lpp__float_t rhs))
{
    lpp::lpp__number result(context, context->get_thing(lhs_name)->get_value());

    if(rhs_name.empty())
    {
        result.apply_unary(i, f);
    }
    else
    {
        result.apply_binary(context->get_thing(rhs_name)->get_value(), i, f);

        lpp::lpp__value::pointer_t rest(context->find_thing(rest_name)->get_value());
        if(rest != nullptr)
        {
            lpp::lpp__value::vector_t const & items(rest->get_list());
            for(auto const & r : items)
            {
                result.apply_binary(r, i, f);
            }
        }
    }

    result.save_as_return_value();
}



} // lpp namespace
// vim: ts=4 sw=4 et nocindent
