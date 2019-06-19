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


void Parser::control_primitive(control_t & control_info)
{
    std::string const name(control_info.m_function_call->get_word());

    switch(name[0])
    {
    case 'c':
        if(name == "catch")
        {
            control_catch(control_info);
            return;
        }
        break;

    case 'f':
        if(name == "forever")
        {
            control_forever(control_info);
            return;
        }
        break;

    case 'g':
        if(name == "goto")
        {
            control_goto(control_info);
            return;
        }
        break;

    case 'i':
        if(name == "if")
        {
            control_if(control_info, false);
            return;
        }
        if(name == "ifelse")
        {
            control_if(control_info, true);
            return;
        }
        if(name == "iftrue" || name == "ift")
        {
            control_if_test(control_info, true);
            return;
        }
        if(name == "iffalse" || name == "iff")
        {
            control_if_test(control_info, false);
            return;
        }
        break;

    case 'o':
        if(name == "output" || name == "op")
        {
            control_output(control_info);
            return;
        }
        break;

    case 'r':
        if(name == "repeat")
        {
            control_repeat(control_info);
            return;
        }
        break;

    case 's':
        if(name == "stop")
        {
            control_stop(control_info);
            return;
        }
        break;

    case 't':
        if(name == "throw")
        {
            control_throw(control_info);
            return;
        }
        if(name == "tag")
        {
            control_tag(control_info);
            return;
        }
        break;

    case 'u':
        if(name == "until")
        {
            control_until(control_info);
            return;
        }
        break;

    default:
        break;

    }

    throw std::logic_error("primitive \"" + name + "\" was not handled properly.");
}


void Parser::control_catch(control_t & control_info)
{
    if(control_info.m_max_args != 2)
    {
        throw std::logic_error("primitive \"catch\" called with a number of parameters not equal to 2.");
    }

    std::string tag_value;
    std::string tag_name;
    bool direct_value(false);
    Token::pointer_t arg(control_info.m_function_call->get_list_item(0));
    switch(arg->get_token())
    {
    case token_t::TOK_FUNCTION_CALL:
        // the output of a function call will stack a parameter
        //
        tag_value = get_unique_name();
        f_out << "lpp::lpp__value::pointer_t "
              << tag_value
              << ";\n";
        output_function_call(arg, tag_value);
        break;

    case token_t::TOK_INTEGER:
        direct_value = true;
        tag_name = std::to_string(arg->get_integer());
        break;

    case token_t::TOK_FLOAT:
        direct_value = true;
        tag_name = std::to_string(arg->get_float());
        break;

    case token_t::TOK_QUOTED:
    case token_t::TOK_WORD:
        direct_value = true;
        tag_name = arg->get_word();
        break;

    case token_t::TOK_THING:
        tag_value = get_unique_name();
        f_out << "lpp::lpp__value::pointer_t "
              << tag_value
              << "(context->get_thing("
              << word_to_cpp_literal_string(arg->get_word())
              << ")->get_value());\n";
        break;

    default:
        arg->error("unexpected token type ("
                  + to_string(arg->get_token())
                  + ") for the tag of the catch instruction.");
        return;

    }

    f_out << "try\n"
             "{\n";

    Token::pointer_t instruction_list(control_info.m_function_call->get_list_item(1));
    Token::pointer_t instructions(parse_body(instruction_list));
    output_body(instructions);

    f_out << "}\n"
             "catch(lpp::lpp__error const&e)\n"
             "{\n"
             "if(e.tag()!=";

    if(direct_value)
    {
        f_out << "\""
              << tag_name
              << "\"";
    }
    else
    {
        f_out << tag_value
              << "->to_word()";
    }

    f_out << ")\n"
             "{\n"
             "throw;\n"     // not caught, rethrow
             "}\n"
             "context->set_error(e);\n"
             "}\n";
}


void Parser::control_forever(control_t & control_info)
{
    if(control_info.m_max_args != 1)
    {
        throw std::logic_error("primitive \"forever\" called with a number of parameters not equal to 1.");
    }

    std::string value_name;
    Token::pointer_t arg(control_info.m_function_call->get_list_item(0));

    std::string const repeat_var(get_unique_name());
    f_out << "for(lpp::lpp__integer_t "
          << repeat_var
          << "(0);;++"
          << repeat_var
          << ")\n"
          << "{\n";

    // for the `repcount` primitive
    //
    f_out << "lpp::lpp__raii_repeat_count "
          << get_unique_name()
          << "(context,"
          << repeat_var
          << ");\n";

    Token::pointer_t instruction_list(control_info.m_function_call->get_list_item(0));
    Token::pointer_t instructions(parse_body(instruction_list));
    output_body(instructions);

    f_out << "}\n";
}


void Parser::control_goto(control_t & control_info)
{
    if(control_info.m_max_args != 1)
    {
        throw std::logic_error("primitive \"goto\" called with a number of parameters not equal to 1.");
    }

    std::string value_name;
    Token::pointer_t arg(control_info.m_function_call->get_list_item(0));
    if(arg->get_token() != token_t::TOK_QUOTED)
    {
        arg->error("the \"goto\" primitive only accepts a quoted word as parameter. "
                  + to_string(arg->get_token())
                  + " is not acceptable.");
    }

    std::string const repeat_var(get_unique_name());
    f_out << "goto "
          << arg->get_word()
          << ";\n";
}


void Parser::control_if(control_t & control_info, bool always_else)
{
    if(always_else)
    {
        if(control_info.m_max_args != 3)
        {
            throw std::logic_error("primitive \"ifelse\" called with a number of parameters not equal to 3.");
        }
    }
    else
    {
        if(control_info.m_max_args != 2
        && control_info.m_max_args != 3)
        {
            throw std::logic_error("primitive \"if\" called with a number of parameters not equal to 2 or 3.");
        }
    }

    std::string value_name;
    Token::pointer_t arg(control_info.m_function_call->get_list_item(0));

    bool direct_value(false);
    bool tf(false);

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

    case token_t::TOK_BOOLEAN:
        direct_value = true;
        tf = arg->get_boolean();
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
                  + ") for the if condition.");
        return;

    }

    if(direct_value)
    {
        if(tf)
        {
            Token::pointer_t instruction_list(control_info.m_function_call->get_list_item(1));
            Token::pointer_t instructions(parse_body(instruction_list));
            output_body(instructions);
        }
        else if(control_info.m_max_args == 3)
        {
            Token::pointer_t instruction_list(control_info.m_function_call->get_list_item(2));
            Token::pointer_t instructions(parse_body(instruction_list));
            output_body(instructions);
        }
        // else -- nothing
    }
    else
    {
        std::string const tf_var(get_unique_name());
        f_out << "bool "
              << tf_var
              << ";\n"
                 "if("
              << value_name
              << "->type()"
                 "=="
                 "lpp::lpp__value_type_t::LPP__VALUE_TYPE_BOOLEAN)\n"
                 "{\n"
              << tf_var
              << "="
              << value_name
              << "->get_boolean();\n"
                 "}\n"
                 "else\n"
                 "{\n"
                 "throw lpp::lpp__error(context,lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM,\"error\",\"if expression must be a boolean.\");\n"
                 "}\n"
                 "if("
              << tf_var
              << ")\n"
              << "{\n";

        {
            Token::pointer_t instruction_list(control_info.m_function_call->get_list_item(1));
            Token::pointer_t instructions(parse_body(instruction_list));
            output_body(instructions);
        }

        f_out << "}\n";

        if(control_info.m_max_args == 3)
        {
            f_out << "else\n"
                  << "{\n";

            Token::pointer_t instruction_list(control_info.m_function_call->get_list_item(2));
            Token::pointer_t instructions(parse_body(instruction_list));
            output_body(instructions);

            f_out << "}\n";
        }
    }
}


void Parser::control_if_test(control_t & control_info, bool true_or_false)
{
    if(control_info.m_max_args != 1)
    {
        throw std::logic_error("primitives \"iftrue\" and \"iffalse\" called with a number of parameters not equal to 1.");
    }

    f_out << "if(context->get_test()==lpp::test_t::TEST_"
          << (true_or_false ? "TRUE" : "FALSE")
          << ")\n"
             "{\n";

    Token::pointer_t instruction_list(control_info.m_function_call->get_list_item(0));
    Token::pointer_t instructions(parse_body(instruction_list));
    output_body(instructions);

    f_out << "}\n";
}


void Parser::control_output(control_t & control_info)
{
    std::string const value_name(get_unique_name());
    output_argument(control_info.m_function_call->get_list_item(0), value_name);
    f_out << "context->set_return_value("
          << value_name
          << ");\n"
             "return;\n";
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
                 "throw lpp::lpp__error(context,lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM,\"error\",\"repeat count must be a number.\");\n"
                 "}\n"
                 "for(lpp::lpp__integer_t "
              << repeat_var
              << "(0);"
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
    f_out << "lpp::lpp__raii_repeat_count "
          << get_unique_name()
          << "(context,"
          << repeat_var
          << ");\n";

    Token::pointer_t instruction_list(control_info.m_function_call->get_list_item(1));
    Token::pointer_t instructions(parse_body(instruction_list));
    output_body(instructions);

    f_out << "}\n";
}


void Parser::control_stop(control_t & control_info)
{
    static_cast<void>(control_info);

    f_out << "return;\n";
}


void Parser::control_tag(control_t & control_info)
{
    if(control_info.m_max_args != 1)
    {
        throw std::logic_error("primitive \"tag\" called with a number of parameters not equal to 1.");
    }

    std::string value_name;
    Token::pointer_t arg(control_info.m_function_call->get_list_item(0));
    if(arg->get_token() != token_t::TOK_QUOTED)
    {
        arg->error("the \"tag\" primitive only accepts a quoted word as parameter. "
                  + to_string(arg->get_token())
                  + " is not acceptable.");
    }

    std::string const repeat_var(get_unique_name());
    f_out << arg->get_word()
          << ":;\n";
}


void Parser::control_throw(control_t & control_info)
{
    if(control_info.m_max_args != 1
    && control_info.m_max_args != 2)
    {
        throw std::logic_error("primitive \"throw\" called with a number of parameters not equal to 1 or 2.");
    }

    // tag
    //
    std::string tag_value;
    std::string tag_name;
    bool direct_value(false);
    Token::pointer_t arg(control_info.m_function_call->get_list_item(0));
    switch(arg->get_token())
    {
    case token_t::TOK_FUNCTION_CALL:
        // the output of a function call will stack a parameter
        //
        tag_value = get_unique_name();
        f_out << "lpp::lpp__value::pointer_t "
              << tag_value
              << ";\n";
        output_function_call(arg, tag_value);
        break;

    case token_t::TOK_INTEGER:
        direct_value = true;
        tag_name = std::to_string(arg->get_integer());
        break;

    case token_t::TOK_FLOAT:
        direct_value = true;
        tag_name = std::to_string(arg->get_float());
        break;

    case token_t::TOK_QUOTED:
    case token_t::TOK_WORD:
        direct_value = true;
        tag_name = arg->get_word();
        break;

    case token_t::TOK_THING:
        tag_value = get_unique_name();
        f_out << "lpp::lpp__value::pointer_t "
              << tag_value
              << "(context->get_thing("
              << word_to_cpp_literal_string(arg->get_word())
              << ")->get_value());\n";
        break;

    default:
        arg->error("unexpected token type ("
                  + to_string(arg->get_token())
                  + ") for a throw tag, expected a TOK_WORD.");
        return;

    }

    // value (defined in the REST)
    //
    std::string value_name;
    if(control_info.m_function_call->get_list_size() > 1)
    {
        value_name = get_unique_name();
        output_argument(control_info.m_function_call->get_list_item(0), value_name);
    }

    if(direct_value)
    {
        tag_name = '"' + tag_name + '"';
    }
    else
    {
        f_out << "if(!"
              << tag_value
              << "->represents_word())\n"
                 "{\n"
                 "throw lpp::lpp__error(context,lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM,\"error\",\"throw first parameter is expected to be a word.\");\n"
                 "}\n";

        tag_name = tag_value + "->to_word()";
    }

    f_out << "throw lpp::lpp__error(context,lpp::lpp__error_code_t::ERROR_CODE_THROW,"
          << tag_name
          << ",\"user error\"";

    if(!value_name.empty())
    {
        f_out << ","
              << value_name;
    }

    f_out << ");\n";
}


void Parser::control_until(control_t & control_info)
{
    if(control_info.m_max_args != 2)
    {
        throw std::logic_error("primitive \"until\" called with a number of parameters not equal to 2.");
    }

    std::string value_name;
    Token::pointer_t arg(control_info.m_function_call->get_list_item(0));

    bool direct_value(false);
    bool tf(false);

    switch(arg->get_token())
    {
    case token_t::TOK_FUNCTION_CALL:
        // the output of a function call will stack a parameter
        //
        value_name = get_unique_name();
        f_out << "for(;;)\n"
                 "{\n"
                 "lpp::lpp__value::pointer_t "
              << value_name
              << ";\n";
        output_function_call(arg, value_name);
        break;

    case token_t::TOK_BOOLEAN:
        direct_value = true;
        tf = arg->get_boolean();
        break;

    case token_t::TOK_THING:
        value_name = get_unique_name();
        f_out << "for(;;)\n"
                 "{\n"
                 "lpp::lpp__value::pointer_t "
              << value_name
              << "(context->get_thing("
              << word_to_cpp_literal_string(arg->get_word())
              << ")->get_value());\n";
        break;

    default:
        arg->error("unexpected token type ("
                  + to_string(arg->get_token())
                  + ") for the if condition.");
        return;

    }

    if(direct_value)
    {
        // if true, it never executes
        //
        if(tf)
        {
            return;
        }

        // if false, it is equivalent to FOREVER
        //
        f_out << "for(;;)\n"
                 "{\n";

        Token::pointer_t instruction_list(control_info.m_function_call->get_list_item(1));
        Token::pointer_t instructions(parse_body(instruction_list));
        output_body(instructions);

        f_out << "}\n";
    }
    else
    {
        std::string const tf_var(get_unique_name());
        f_out << "bool "
              << tf_var
              << ";\n"
                 "if("
              << value_name
              << "->type()"
                 "=="
                 "lpp::lpp__value_type_t::LPP__VALUE_TYPE_BOOLEAN)\n"
                 "{\n"
              << tf_var
              << "="
              << value_name
              << "->get_boolean();\n"
                 "}\n"
                 "else\n"
                 "{\n"
                 "throw lpp::lpp__error(context,lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM,\"error\",\"if expression must be a boolean.\");\n"
                 "}\n"
                 "if("
              << tf_var
              << ")\n"
                 "{\n"
                 "break;\n"
                 "}\n";

        Token::pointer_t instruction_list(control_info.m_function_call->get_list_item(1));
        Token::pointer_t instructions(parse_body(instruction_list));
        output_body(instructions);

        f_out << "}\n";
    }
}


} // lpp namespace

// vim: ts=4 sw=4 et nocindent
