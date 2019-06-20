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

lpp::lpp__float_t flt_bin_operation(lpp::lpp__context::pointer_t , lpp::lpp__float_t lhs, lpp::lpp__float_t rhs)
{
    // note: logo takes (x y) so here we swap the parameters
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
    if(lhs == 0.0)
    {
        return rhs < 0 ? -90.0 : 90.0;
    }
#pragma GCC diagnostic pop
    return atan2(rhs, lhs) * 180.0 / M_PI;
}

lpp::lpp__float_t flt_una_operation(lpp::lpp__context::pointer_t , lpp::lpp__float_t lhs, lpp::lpp__float_t )
{
    return atan(lhs) * 180.0 / M_PI;
}

} // no name namespace


void primitive_arctan(lpp::lpp__context::pointer_t context)
{
    lpp::lpp__thing::pointer_t rest(context->find_thing("rest"));
    if(rest != nullptr
    && rest->get_value()->get_list().size() == 1)
    {
        // this is a non-standard binary
        //
        lpp::lpp__number::compute(
                  context
                , "number1"
                , "rest"
                , std::string()
                , nullptr
                , flt_bin_operation);
    }
    else
    {
        // standard unary
        //
        lpp::lpp__number::compute(
                  context
                , "number1"
                , std::string()
                , std::string()
                , nullptr
                , flt_una_operation);
    }
}


// vim: ts=4 sw=4 et nocindent
