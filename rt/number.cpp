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
#include <cmath>
#include <iostream>
#include <sstream>

// boost lib
//
#include <boost/math/special_functions/sign.hpp>



namespace lpp
{



lpp__number::lpp__number(lpp__context::pointer_t context)
    : f_context(context)
{
}


lpp__number::lpp__number(lpp__context::pointer_t context, lpp__value::pointer_t number)
    : f_context(context)
{
    if(!from_value(number))
    {
        throw lpp__error(f_context
                       , lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                       , "error"
                       , "argument was expected to represent a number");
    }
}


bool lpp__number::from_string(std::string const & number)
{
    char * end(nullptr);

    f_integer = std::strtoll(number.c_str(), &end, 10);
    if(end == nullptr
    || number.c_str() == end
    || *end != '\0')
    {
        f_float = std::strtod(number.c_str(), &end);
        if(end == nullptr
        || number.c_str() == end
        || *end != '\0')
        {
            return false;
        }
        f_is_integer = false;
    }

    return true;
}


bool lpp__number::from_value(lpp__value::pointer_t number)
{
    switch(number->type())
    {
    case lpp__value_type_t::LPP__VALUE_TYPE_INTEGER:
        f_integer = number->get_integer();
        return true;

    case lpp__value_type_t::LPP__VALUE_TYPE_FLOAT:
        f_is_integer = false;
        f_float = number->get_float();
        return true;

    case lpp__value_type_t::LPP__VALUE_TYPE_WORD:
        return from_string(number->get_word());

    default:
        return false;

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
std::cerr << "-> saving integer = " << f_integer << "\n";
        return std::make_shared<lpp__value>(f_integer);
    }
    else
    {
std::cerr << "-> saving float = " << f_float << "\n";
        lpp__value::pointer_t flt(std::make_shared<lpp__value>(f_float));
        if(flt->represents_integer())
        {
std::cerr << "-> but auto-casting to integer = " << f_float << "\n";
            flt->set_integer(static_cast<lpp__integer_t>(f_float));
        }
        return flt;
    }
}


void lpp__number::save_as_return_value()
{
    f_context->set_return_value(to_value());
}



void lpp__number::apply_unary(
          std::function<lpp__integer_t(lpp__context::pointer_t context, lpp__integer_t lhs, lpp__integer_t rhs)> i
        , std::function<lpp__float_t(lpp__context::pointer_t context, lpp__float_t lhs, lpp__float_t rhs)> f)
{
    int state(f_is_integer ? 0 : 1);

    if(state != 0
    && f == nullptr)
    {
        throw lpp__error(f_context
                       , lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                       , "error"
                       , "function not valid against floating points");
        return;
    }

    if(state == 0
    && i == nullptr)
    {
        f_is_integer = false;
        f_float = static_cast<lpp__float_t>(f_integer);
        state = 1;
    }

    switch(state)
    {
    case 0:
        f_integer = i(f_context, f_integer, 0);
        break;

    case 1:
std::cerr << "calling f with " << f_float << "\n";
        f_float = f(f_context, f_float, 0.0);
std::cerr << "f returned with " << f_float << "\n";
        break;

    }
}


void lpp__number::apply_binary(lpp__value::pointer_t number
        , std::function<lpp__integer_t(lpp__context::pointer_t context, lpp__integer_t lhs, lpp__integer_t rhs)> i
        , std::function<lpp__float_t(lpp__context::pointer_t context, lpp__float_t lhs, lpp__float_t rhs)> f)
{
    lpp__number rhs(f_context, number);

    int state((    f_is_integer ? 0 : 1)
            | (rhs.f_is_integer ? 0 : 2));

    if(state != 0
    && f == nullptr)
    {
        throw lpp__error(f_context
                       , lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                       , "error"
                       , "function not valid against floating points");
        return;
    }

    if(state == 0
    && i == nullptr)
    {
        // do a cast in this case
        //
        f_is_integer = false;
        f_float = static_cast<lpp__float_t>(f_integer);
        rhs.f_float = static_cast<lpp__float_t>(rhs.f_integer);
        state = 3;
    }

    switch(state)
    {
    case 0:
        f_integer = i(f_context, f_integer, rhs.f_integer);
        break;

    case 1:
        f_float = f(f_context, f_float, static_cast<lpp__float_t>(rhs.f_integer));
        break;

    case 2:
        f_is_integer = false;
        f_float = f(f_context, static_cast<lpp__float_t>(f_integer), rhs.f_float);
        break;

    case 3:
        f_float = f(f_context, f_float, rhs.f_float);
        break;

    }
}


void lpp__number::compute(
              lpp__context::pointer_t context
            , std::string const & lhs_name
            , std::string const & rhs_name
            , std::string const & rest_name
            , std::function<lpp__integer_t(lpp__context::pointer_t context, lpp__integer_t lhs, lpp__integer_t rhs)> i
            , std::function<lpp__float_t(lpp__context::pointer_t context, lpp__float_t lhs, lpp__float_t rhs)> f)
{
    lpp__number result(context, context->get_thing(lhs_name)->get_value());

    if(rhs_name.empty())
    {
        result.apply_unary(i, f);
    }
    else
    {
        if(rhs_name == "rest")
        {
            // special case where the rest is a second parameter making
            // the operation a binary operation
            //
            lpp__value::pointer_t rest(context->get_thing("rest")->get_value());
            if(rest == nullptr
            || rest->get_list().size() == 0)
            {
                result.apply_unary(i, f);
            }
            else
            {
                lpp__value::vector_t const & items(rest->get_list());
                if(items.size() != 1)
                {
                    throw lpp__error(context
                                   , lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                                   , "error"
                                   , "unexpected number of arguments passed to function.");
                }
                result.apply_binary(items[0], i, f);
            }
        }
        else
        {
            result.apply_binary(context->get_thing(rhs_name)->get_value(), i, f);

            lpp__thing::pointer_t rest(context->find_thing(rest_name));
            if(rest != nullptr)
            {
                lpp__value::vector_t const & items(rest->get_value()->get_list());
                for(auto const & r : items)
                {
                    result.apply_binary(r, i, f);
                }
            }
        }
    }

    result.save_as_return_value();
}



lpp__integer_t compare_values(
          lpp__context::pointer_t context
        , lpp__value::pointer_t lhs
        , lpp__value::pointer_t rhs
        , bool characters)
{
    if(!characters)
    {
        lpp__number l(context);
        lpp__number r(context);

        if(l.from_value(lhs)
        && r.from_value(rhs))
        {
            // if both are numbers, then compare as number
            //
            lpp__float_t fl(0.0);
            lpp__float_t fr(0.0);
            int const state((l.is_integer() ? 0 : 1)
                          + (r.is_integer() ? 0 : 2));
            switch(state)
            {
            case 0:
                return boost::math::sign(l.get_integer() - r.get_integer());

            case 1:
                fl = l.get_float();
                fr = static_cast<lpp__float_t>(r.get_integer());
                break;

            case 2:
                fl = static_cast<lpp__float_t>(l.get_integer());
                fr = r.get_float();
                break;

            case 3:
                fl = l.get_float();
                fr = r.get_float();
                break;

            }
            if(std::isunordered(fl, fr))
            {
                return 2;
            }
            return boost::math::sign(fl - fr);
        }

        if(lhs->type() == lpp__value_type_t::LPP__VALUE_TYPE_BOOLEAN
        && rhs->type() == lpp__value_type_t::LPP__VALUE_TYPE_BOOLEAN)
        {
            return boost::math::sign((lhs->get_boolean() ? 0 : 1)
                                - (rhs->get_boolean() ? 0 : 1));
        }

        if(lhs->type() == lpp__value_type_t::LPP__VALUE_TYPE_LIST
        && rhs->type() == lpp__value_type_t::LPP__VALUE_TYPE_LIST)
        {
            lpp__value::vector_t ll(lhs->get_list());
            lpp__value::vector_t lr(rhs->get_list());
            size_t const max(std::min(ll.size(), lr.size()));
            for(size_t idx(0); idx < max; ++idx)
            {
                lpp__integer_t const c(compare_values(context, ll[idx], lr[idx], characters));
                if(c != 0)
                {
                    return c;
                }
            }
            if(ll.size() == lr.size())
            {
                return 0;
            }
            if(ll.size() > lr.size())
            {
                return 1;
            }
            return -1;
        }

        if(lhs->type() == lpp__value_type_t::LPP__VALUE_TYPE_LIST
        || rhs->type() == lpp__value_type_t::LPP__VALUE_TYPE_LIST)
        {
            return -2;
        }
    }

    return boost::math::sign(lhs->to_word().compare(rhs->to_word()));
}


void lpp__number::compare(
          lpp__context::pointer_t context
        , std::string const & lhs_name
        , std::string const & rhs_name
        , std::string const & rest_name
        , std::function<bool(lpp__integer_t compare)> f
        , bool characters)
{
    lpp__value::pointer_t lhs(context->get_thing(lhs_name)->get_value());
    lpp__value::pointer_t rhs(context->get_thing(rhs_name)->get_value());

    lpp__integer_t c(compare_values(context, lhs, rhs, characters));
    bool boolean(f(c));

    if(boolean)
    {
        lpp__thing::pointer_t rest(context->find_thing(rest_name));
        if(rest != nullptr)
        {
            lpp__value::vector_t const & items(rest->get_value()->get_list());
            for(auto const & it : items)
            {
                c = compare_values(context, lhs, it, characters);
                if(!f(c))
                {
                    boolean = false;
                    break;
                }
            }
        }
    }

    lpp__value::pointer_t result(std::make_shared<lpp__value>(boolean));
    context->set_return_value(result);
}


void lpp__number::logical(
          lpp__context::pointer_t context
        , std::string const & lhs_name
        , std::string const & rhs_name
        , std::string const & rest_name
        , std::function<bool(bool lhs, bool rhs)> b)
{
    lpp__value::pointer_t lhs(context->get_thing(lhs_name)->get_value());
    lpp__value::pointer_t rhs(context->get_thing(rhs_name)->get_value());

    if(lhs->type() != lpp__value_type_t::LPP__VALUE_TYPE_BOOLEAN
    || rhs->type() != lpp__value_type_t::LPP__VALUE_TYPE_BOOLEAN)
    {
        throw lpp__error(context
                       , lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                       , "error"
                       , "logical functions expects boolean values only.");
    }

    bool l(lhs->get_boolean());
    bool r(rhs->get_boolean());

    bool boolean(b(l, r));

    if(boolean)
    {
        lpp__thing::pointer_t rest(context->find_thing(rest_name));
        if(rest != nullptr)
        {
            lpp__value::vector_t const & items(rest->get_value()->get_list());
            for(auto const & it : items)
            {
                if(it->type() != lpp__value_type_t::LPP__VALUE_TYPE_BOOLEAN)
                {
                    throw lpp__error(context
                                   , lpp__error_code_t::ERROR_CODE_INVALID_DATUM
                                   , "error"
                                   , "logical functions expects boolean values only.");
                }
                boolean = b(boolean, it->get_boolean());
            }
        }
    }

    lpp__value::pointer_t result(std::make_shared<lpp__value>(boolean));
    context->set_return_value(result);
}



} // lpp namespace
// vim: ts=4 sw=4 et nocindent
