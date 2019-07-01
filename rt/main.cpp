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
#include <lpp/lpp.hpp>

// C++ lib
//
#include <iostream>


// program is always defined in the lpp_startup() function
//
void lpp__startup(lpp::lpp__context::pointer_t context);


int main(int argc, char * argv[])
{
    try
    {
        lpp::lpp__auto_register_procedures primitives(nullptr, 0);  // register primitives

        lpp::lpp__context::pointer_t context(std::make_shared<lpp::lpp__context>(std::string(), "program", 0, false));
        context->set_global(context);

        lpp::lpp__value::vector_t argument_list;
        for(int i(0); i < argc; ++i)
        {
            lpp::lpp__value::pointer_t arg(std::make_shared<lpp::lpp__value>(std::string(argv[i])));
            if(arg->represents_integer())
            {
                arg->set_integer(arg->to_integer());
            }
            else if(arg->represents_float())
            {
                arg->set_float(arg->to_float());
            }
            argument_list.push_back(arg);
        }
        lpp::lpp__value::pointer_t arguments(std::make_shared<lpp::lpp__value>(argument_list));

        context->set_thing("arguments", arguments, lpp::lpp__thing_type_t::LPP__THING_TYPE_GLOBAL);

        lpp__startup(context);

        if(context->has_returned_value())
        {
            lpp::lpp__value::pointer_t exit_code(context->get_returned_value());
            if(exit_code->type() != lpp::lpp__value_type_t::LPP__VALUE_TYPE_INTEGER)
            {
                std::cerr << "error: exit code is expected to be an integer.\n";
                return 1;
            }
            else
            {
                return exit_code->get_integer();
            }
        }

        return 0;
    }
    catch(lpp::lpp__error const & e)
    {
        std::cerr << "error: "
                  << (e.tag() == "error" ? "" : e.tag() + ": ")
                  << e.what()
                  << std::endl;
        // TODO: display value if not nullptr
        return 1;
    }
    catch(std::logic_error const & e)
    {
        std::cerr << "internal error: "
                  << e.what()
                  << std::endl;
        return 1;
    }
}


// vim: ts=4 sw=4 et nocindent
