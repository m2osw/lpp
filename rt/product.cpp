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


#define T1T2(t1, t2) (static_cast<int>(t1) * 256 + static_cast<int>(t2))


void product(lpp::lpp__context::pointer_t context)
{
    // TODO: handle overflow/underflow

    lpp::lpp__value::pointer_t n1(context->get_thing("number1")->get_value());
    lpp::lpp__value::pointer_t n2(context->get_thing("number2")->get_value());

    lpp::lpp__value::pointer_t result(std::make_shared<lpp::lpp__value>());

    switch(T1T2(n1->type(), n2->type()))
    {
    case T1T2(lpp::lpp__value_type_t::LPP__VALUE_TYPE_INTEGER, lpp::lpp__value_type_t::LPP__VALUE_TYPE_INTEGER):
        result->set_integer(n1->get_integer() * n2->get_integer());
        break;

    case T1T2(lpp::lpp__value_type_t::LPP__VALUE_TYPE_INTEGER, lpp::lpp__value_type_t::LPP__VALUE_TYPE_FLOAT):
        result->set_float(n1->get_integer() * n2->get_float());
        break;

    case T1T2(lpp::lpp__value_type_t::LPP__VALUE_TYPE_FLOAT, lpp::lpp__value_type_t::LPP__VALUE_TYPE_INTEGER):
        result->set_float(n1->get_float() * n2->get_integer());
        break;

    case T1T2(lpp::lpp__value_type_t::LPP__VALUE_TYPE_FLOAT, lpp::lpp__value_type_t::LPP__VALUE_TYPE_FLOAT):
        result->set_float(n1->get_float() * n2->get_float());
        break;

    default:
        // TODO: create logo error
        std::cerr << "error: unexpected params to product.\n";
        exit(1);
        break;

    }

    lpp::lpp__value::pointer_t rest(context->find_thing("rest")->get_value());
    if(rest != nullptr)
    {
        lpp::lpp__value::vector_t const & items(rest->get_list());
        auto const max(items.size());
        for(auto const & r : items)
        {
            switch(T1T2(result->type(), r->type()))
            {
            case T1T2(lpp::lpp__value_type_t::LPP__VALUE_TYPE_INTEGER, lpp::lpp__value_type_t::LPP__VALUE_TYPE_INTEGER):
                result->set_integer(result->get_integer() * r->get_integer());
                break;

            case T1T2(lpp::lpp__value_type_t::LPP__VALUE_TYPE_INTEGER, lpp::lpp__value_type_t::LPP__VALUE_TYPE_FLOAT):
                result->set_float(result->get_integer() * r->get_float());
                break;

            case T1T2(lpp::lpp__value_type_t::LPP__VALUE_TYPE_FLOAT, lpp::lpp__value_type_t::LPP__VALUE_TYPE_INTEGER):
                result->set_float(result->get_float() * r->get_integer());
                break;

            case T1T2(lpp::lpp__value_type_t::LPP__VALUE_TYPE_FLOAT, lpp::lpp__value_type_t::LPP__VALUE_TYPE_FLOAT):
                result->set_float(result->get_float() * r->get_float());
                break;

            default:
                // TODO: create logo error
                std::cerr << "error: unexpected params to product.\n";
                exit(1);
                break;

            }
        }
    }

    context->set_return_value(result);
}


// vim: ts=4 sw=4 et nocindent
