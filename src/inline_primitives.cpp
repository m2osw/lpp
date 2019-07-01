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



bool Parser::inline_primitive(control_t & control_info)
{
    if((control_info.f_procedure_flags & PROCEDURE_FLAG_INLINE) == 0
    || (control_info.f_procedure_flags & PROCEDURE_FLAG_TYPE_MASK) != PROCEDURE_FLAG_PRIMITIVE)
    {
        return false;
    }

    std::string const name(control_info.f_function_call->get_word());

    switch(name[0])
    {
    case 't':
        if(name == "thing")
        {
            primitive_thing(control_info);
            return true;
        }
        break;

    case 'w':
        if(name == "wait")
        {
            primitive_wait(control_info);
            return true;
        }
        break;

    default:
        break;

    }

    return false;
}





void Parser::primitive_thing(control_t & control_info)
{
    if(control_info.f_max_args != 1)
    {
        throw std::logic_error("primitive \"thing\" called with a number of parameters not equal to 1.");
    }

    Token::pointer_t arg(control_info.f_function_call->get_list_item(0));

    bool direct_value(false);
    std::string thing_name;
    std::string value_name;

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

    case token_t::TOK_WORD:
    case token_t::TOK_QUOTED:
        direct_value = true;
        thing_name = arg->get_word();
        break;

    case token_t::TOK_THING:
        value_name = get_unique_name();
        f_out << "lpp::lpp__value::pointer_t "
              << value_name
              << "(context->get_thing("
              << word_to_cpp_string_literal(arg->get_word())
              << ")->get_value());\n";
        break;

    default:
        arg->error("unexpected token type ("
                  + to_string(arg->get_token())
                  + ") for THING count.");
        return;

    }

    if(direct_value)
    {
        f_out << control_info.f_result_var
              << "=context->get_thing("
              << word_to_cpp_string_literal(thing_name)
              << ")->get_value();\n";
    }
    else
    {
        f_out << "if("
              << value_name
              << "->type()!=lpp::lpp__value_type_t::LPP__VALUE_TYPE_WORD)\n"
                 "{\n"
                 "throw lpp::lpp__error(context,lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM,\"error\",\"thing parameter must be a valid word. \\\"\"+"
              << value_name
              << "->to_string()+\"\\\" is not considered valid\");\n"
                 "}\n"
              << control_info.f_result_var
              << "=context->get_thing("
              << value_name
              << "->get_word())->get_value();\n";
    }
}




void Parser::primitive_wait(control_t & control_info)
{
    if(control_info.f_max_args != 1)
    {
        throw std::logic_error("primitive \"wait\" called with a number of parameters not equal to 1.");
    }

    Token::pointer_t arg(control_info.f_function_call->get_list_item(0));

    bool direct_value(false);
    float_t duration;
    std::string value_name;

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
        duration = arg->get_integer();
        break;

    case token_t::TOK_FLOAT:
        direct_value = true;
        duration = arg->get_float();
        break;

    case token_t::TOK_THING:
        value_name = get_unique_name();
        f_out << "lpp::lpp__value::pointer_t "
              << value_name
              << "(context->get_thing("
              << word_to_cpp_string_literal(arg->get_word())
              << ")->get_value());\n";
        break;

    default:
        arg->error("unexpected token type ("
                  + to_string(arg->get_token())
                  + ") for WAIT.");
        return;

    }

    f_out << "lpp::lpp__tty_flush();\n";

    if(direct_value)
    {
        double integral_part(0.0);
        double fraction(modf(duration, &integral_part));

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
        if(fraction != 0.0)
        {
            std::string const ts_name(get_unique_name());
            f_out << "timespec "
                  << ts_name
                  << ";\n"
                  << ts_name
                  << ".tv_sec="
                  << integral_part
                  << ";\n"
                  << ts_name
                  << ".tv_nsec="
                  << static_cast<integer_t>(floor(fraction * 1000000000.0))
                  << ";\n"
                     "nanosleep(&"
                  << ts_name
                  << ",nullptr);\n";
        }
        else
        {
            f_out << "sleep("
                  << integral_part
                  << ");\n";
        }
#pragma GCC diagnostic push
    }
    else
    {
        std::string const ts_name(get_unique_name());
        std::string const duration_name(get_unique_name());
        f_out << "if(!"
              << value_name
              << "->represents_float())\n"
                 "{\n"
                 "throw lpp::lpp__error(context,lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM,\"error\",\"wait parameter must be a valid floating point number\");\n"
                 "}\n"
                 "lpp::lpp__float_t "
              << duration_name
              << "("
              << value_name
              << "->to_float());\n"
                 "double "
              << duration_name
              << "_integral_part(0.0);\n"
                 "double const "
              << duration_name
              << "_fraction(modf("
              << duration_name
              << ", &"
              << duration_name
              << "_integral_part));\n"
                 "timespec "
              << ts_name
              << "\n{\n"
                 "static_cast<lpp::lpp__integer_t>("
              << duration_name
              << "_integral_part),\n"
                 "static_cast<lpp::lpp__integer_t>("
              << duration_name
              << "_fraction*1000000000.0)\n"
                 "};\n"
                 "nanosleep(&"
              << ts_name
              << ",nullptr);\n";
    }
}



} // lpp namespace

// vim: ts=4 sw=4 et nocindent
