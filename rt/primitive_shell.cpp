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
#include "utf8_iterator.hpp"

// C++ lib
//
#include <iostream>




void primitive_shell(lpp::lpp__context::pointer_t context)
{
    lpp::lpp__value::pointer_t command(context->get_thing("command")->get_value());

    lpp::lpp__value::vector_t list;

    std::string line;
    std::string cmd(command->to_word());
    std::unique_ptr<FILE, decltype(&::pclose)> p(popen(cmd.c_str(), "r"), &::pclose);
    if(p == nullptr)
    {
        throw lpp::lpp__error(context
                            , lpp::lpp__error_code_t::ERROR_CODE_FILE_SYSTEM_ERROR
                            , "error"
                            , "connecting to command \""
                            + cmd
                            + "\" failed.");
    }

    for(;;)
    {
        int c(fgetc(p.get()));
        if(c == EOF)
        {
            break;
        }
        int n('\0');
        if(c == '\r')
        {
            c = fgetc(p.get());
            if(c == EOF)
            {
                break;
            }
            if(c != '\n')
            {
                n = c;
                c = '\n';
            }
        }
        if(c == '\n')
        {
            // TODO: add support for lists of words
            //
            list.push_back(std::make_shared<lpp::lpp__value>(line));

            line.clear();
            if(n != '\0')
            {
                line += n;
            }
        }
        else
        {
            line += c;
        }
    }

    if(!line.empty())
    {
        list.push_back(std::make_shared<lpp::lpp__value>(line));
    }

    lpp::lpp__value::pointer_t result(std::make_shared<lpp::lpp__value>(list));
    context->set_return_value(result);
}


// vim: ts=4 sw=4 et nocindent
