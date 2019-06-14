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
#include "parser.hpp"

// lpp compiler
//
#include "exception.hpp"

// boost lib
//
#include <boost/algorithm/string.hpp>

// C++ lib
//
#include <iostream>
#include <limits>
#include <sstream>




namespace lpp
{


void Parser::control(control_t & control_info)
{
    std::string const name(control_info.m_function_call->get_word());

    switch(name[0])
    {
    case 'r':
        if(name == "repeat")
        {
            control_repeat(control_info);
            return;
        }
        break;

    default:
        break;

    }

    throw std::logic_error("primitive \"" + name + "\" was not handled properly.");
}


void Parser::control_repeat(control_t & control_info)
{
    if(control_info.m_max_args != 2)
    {
        throw std::logic_error("primitive \"repeat\" called with a number of parameters not equal to 2.");
    }

    std::string value_name;
    Token::pointer_t arg(control_info.m_function_call->get_list_item(0));

    bool direct_value(false);
    integer_t count(0);

    switch(arg->get_token())
    {
    case token_t::TOK_FUNCTION_CALL:
        // the output of a function call will stack a parameter
        //
        value_name = get_unique_name();
        f_out << "lpp::lpp__value::pointer_t "
              << value_name
              << ";\n";
        output_function_call(arg, value_name);
        break;

    case token_t::TOK_INTEGER:
        direct_value = true;
        count = arg->get_integer();
        break;

    case token_t::TOK_FLOAT:
        direct_value = true;
        count = static_cast<integer_t>(floor(arg->get_float()));
        break;

    case token_t::TOK_THING:
        value_name = get_unique_name();
        f_out << "lpp::lpp__value::pointer_t "
              << value_name
              << "(context->get_thing("
              << word_to_cpp_literal_string(arg->get_word())
              << ")->get_value());\n";
        break;

    default:
        arg->error("unexpected token type ("
                  + to_string(arg->get_token())
                  + ") for a repeat count.");
        return;

    }

    std::string const repeat_var(get_unique_name());
    if(direct_value)
    {
        f_out << "for(lpp::lpp__integer_t "
              << repeat_var
              << "(0);"
              << repeat_var
              << "<"
              << count
              << ";++"
              << repeat_var
              << ")\n"
              << "{\n";
    }
    else
    {
        std::string const count_var(get_unique_name());
        f_out << "lpp::lpp__integer_t "
              << count_var
              << ";\n"
                 "switch("
              << value_name
              << "->type())\n"
                 "{\n"
                 "case lpp::lpp__value_type_t::LPP__VALUE_TYPE_INTEGER:\n"
              << count_var
              << "="
              << value_name
              << "->get_integer();\n"
                 "break;\n"
                 "case lpp::lpp__value_type_t::LPP__VALUE_TYPE_FLOAT:\n"
              << count_var
              << "=static_cast<lpp::lpp__integer_t>(floor("
              << value_name
              << "->get_float()));\n"
                 "break;\n"
                 "default:\n"
                 "throw lpp__error(\"error\",\"repeat count must be a number.\");\n"
                 "}\n"
                 "for(lpp::lpp__integer_t "
              << repeat_var
              << ";"
              << repeat_var
              << "<"
              << count_var
              << ";++"
              << repeat_var
              << ")\n"
              << "{\n";
    }

    // for the `repcount` primitive
    //
    f_out << "lpp::lpp__raii_repeat_count(context,"
          << repeat_var
          << ");\n";

    Token::pointer_t instruction_list(control_info.m_function_call->get_list_item(1));
    Token::pointer_t instructions(parse_body(instruction_list));
    output_body(instructions);

    f_out << "}\n";
}


} // lpp namespace

// vim: ts=4 sw=4 et nocindent
