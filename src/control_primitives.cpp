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
    std::string const name(control_info.f_function_call->get_word());

    switch(name[0])
    {
    case 'c':
        if(name == "call")
        {
            control_call(control_info, false);
            return;
        }
        else if(name == "callfunction")
        {
            control_call(control_info, true);
            return;
        }
        else if(name == "catch")
        {
            control_catch(control_info);
            return;
        }
        break;

    case 'd':
        if(name == "do.until")
        {
            control_while(control_info, true, true);
            return;
        }
        if(name == "do.while")
        {
            control_while(control_info, false, true);
            return;
        }
        break;

    case 'f':
        if(name == "for")
        {
            control_for(control_info);
            return;
        }
        else if(name == "forever")
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
            control_while(control_info, true, false);
            return;
        }
        break;

    case 'w':
        if(name == "while")
        {
            control_while(control_info, false, false);
            return;
        }
        break;

    default:
        break;

    }

    throw std::logic_error("primitive \"" + name + "\" was not handled properly.");
}


void Parser::control_call(control_t & control_info, bool function_call)
{
    if(control_info.f_max_args != 2)
    {
        throw std::logic_error("primitive \""
                             + control_info.f_function_call->get_word()
                             + "\" called with a number of parameters not equal to 2.");
    }

    std::string proc_value;
    std::string proc_name;
    bool direct_value(false);
    Token::pointer_t procedure_name(control_info.f_function_call->get_list_item(0));
    switch(procedure_name->get_token())
    {
    case token_t::TOK_FUNCTION_CALL:
        // the output of a function call will generate a parameter
        //
        proc_value = get_unique_name();
        f_out << "lpp::lpp__value::pointer_t "
              << proc_value
              << ";\n";
        output_function_call(procedure_name, proc_value);
        break;

    case token_t::TOK_INTEGER:
        direct_value = true;
        proc_name = std::to_string(procedure_name->get_integer());
        break;

    case token_t::TOK_FLOAT:
        direct_value = true;
        proc_name = std::to_string(procedure_name->get_float());
        break;

    case token_t::TOK_QUOTED:
    case token_t::TOK_WORD:
        direct_value = true;
        proc_name = procedure_name->get_word();
        break;

    case token_t::TOK_THING:
        proc_value = get_unique_name();
        f_out << "lpp::lpp__value::pointer_t "
              << proc_value
              << "(context->get_thing("
              << word_to_cpp_string_literal(procedure_name->get_word())
              << ")->get_value());\n";
        break;

    default:
        procedure_name->error("unexpected token type ("
                  + to_string(procedure_name->get_token())
                  + ") for the procedure name of the \"call\" instruction.");
        return;

    }

    std::string const actual_proc_name(get_unique_name());
    f_out << "{\n"
             "std::string const "
          << actual_proc_name
          << "(";
    if(direct_value)
    {
        f_out << word_to_cpp_string_literal(proc_name);
    }
    else
    {
        f_out << proc_value
              << "->to_word()";
    }
    std::string const proc_info(get_unique_name());
    std::string const context_name(get_unique_name());
    f_out << ");\n"
             "lpp::lpp__procedure_info_t const*"
          << proc_info
          << "(lpp::find_procedure("
          << actual_proc_name
          << "));\n"
             "if("
          << proc_info
          << "==nullptr)\n"
             "{\n"
             "throw lpp::lpp__error(context,lpp::lpp__error_code_t::ERROR_CODE_UNKNOWN_PROCEDURE,\"error\",\"procedure named \\\"\"+"
          << actual_proc_name
          << "+\"\\\" not found.\");\n"
             "}\n"
             "lpp::lpp__context::pointer_t "
          << context_name
          << "(std::make_shared<lpp::lpp__context>(std::string(),"
          << actual_proc_name
          << ",0,("
          << proc_info
          << "->f_flags&lpp::PROCEDURE_FLAG_PRIMITIVE)!=0"
             "));\n";

    Token::pointer_t arguments(control_info.f_function_call->get_list_item(1));
    if(arguments->get_token() != token_t::TOK_LIST)
    {
        arguments->error("unexpected token type ("
                  + to_string(arguments->get_token())
                  + ") for the list of arguments of the call instruction.");
        return;
    }

    size_t const max(arguments->get_list_size());
    for(size_t idx(0); idx < max; ++idx)
    {
        Token::pointer_t item(arguments->get_list_item(idx));
        if(item->get_token() != token_t::TOK_LIST)
        {
            arguments->error("unexpected token type ("
                      + to_string(item->get_token())
                      + ") for an argument of the call instruction, each item must be a list.");
            return;
        }
        if(item->get_list_size() != 2)
        {
            arguments->error("each argument of the call instruction must be a list of two items with the name of the variable and the content; this list has "
                           + std::to_string(item->get_list_size())
                           + ".");
            return;
        }
        Token::pointer_t arg_name(item->get_list_item(0));
        if(arg_name->get_token() != token_t::TOK_WORD
        && arg_name->get_token() != token_t::TOK_QUOTED)
        {
            arguments->error("unexpected token type ("
                      + to_string(arg_name->get_token())
                      + ") for an argument of the call instruction, the first item must be a word representing the argument name.");
            return;
        }
        Token::pointer_t arg_value(item->get_list_item(1));

        // execute the value first then save th value in this arg_name
        //
        std::string const arg_var(get_unique_name());
        std::string value;
        bool arg_direct_value(false);
        switch(arg_value->get_token())
        {
        case token_t::TOK_FUNCTION_CALL:
            // the output of a function call will generate a parameter
            //
            f_out << "lpp::lpp__value::pointer_t "
                  << arg_var
                  << ";\n";
            output_function_call(arg_value, arg_var);
            break;

        case token_t::TOK_INTEGER:
            arg_direct_value = true;
            value = "static_cast<lpp::lpp__integer_t>(" + std::to_string(arg_value->get_integer()) + ")";
            break;

        case token_t::TOK_FLOAT:
            arg_direct_value = true;
            value = "static_cast<lpp::lpp__float_t>(" + std::to_string(arg_value->get_float()) + ")";
            break;

        case token_t::TOK_QUOTED:
        case token_t::TOK_WORD:
            arg_direct_value = true;
            value = "std::string(" + word_to_cpp_string_literal(arg_value->get_word()) + ")";
            break;

        case token_t::TOK_THING:
            f_out << "lpp::lpp__value::pointer_t "
                  << arg_var
                  << "(context->get_thing("
                  << word_to_cpp_string_literal(arg_value->get_word())
                  << ")->get_value());\n";
            break;

        default:
            arg_value->error("unexpected token type ("
                      + to_string(arg_value->get_token())
                      + ") for the procedure name of the \"call\" instruction.");
            return;

        }

        f_out << context_name
              << "->set_thing("
              << word_to_cpp_string_literal(arg_name->get_word())
              << ",";

        if(arg_direct_value)
        {
            f_out << "std::make_shared<lpp::lpp__value>("
                  << value
                  << ")";
        }
        else
        {
            f_out << arg_var;
        }

        f_out << ",lpp::lpp__thing_type_t::LPP__THING_TYPE_CONTEXT);\n";
    }

    // we expect the user to define a "rest in his list when necessary
    //
    //f_out << "lpp::lpp__value::vector_t rest;\n"
    //      << context_name
    //      << "->set_thing(\"rest\",std::make_shared<lpp::lpp__value>(rest),lpp::lpp__thing_type_t::LPP__THING_TYPE_CONTEXT);\n";

    f_out << context_name
          << "->attach(context);\n"
          << "(*"
          << proc_info
          << "->f_procedure)("
          << context_name
          << ");\n";

    if(function_call)
    {
        f_out << control_info.f_result_var
              << "="
              << context_name
              << "->get_returned_value();\n";
    }

    f_out << "}\n";
}


void Parser::control_catch(control_t & control_info)
{
    if(control_info.f_max_args != 2)
    {
        throw std::logic_error("primitive \"catch\" called with a number of parameters not equal to 2.");
    }

    std::string tag_value;
    std::string tag_name;
    bool direct_value(false);
    Token::pointer_t arg(control_info.f_function_call->get_list_item(0));
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
              << word_to_cpp_string_literal(arg->get_word())
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

    Token::pointer_t instruction_list(control_info.f_function_call->get_list_item(1));
    Token::pointer_t instructions(parse_body(instruction_list));
    output_body(instructions);

    f_out << "}\n"
             "catch(lpp::lpp__error const&e)\n"
             "{\n"
             "if(e.tag()!=";

    if(direct_value)
    {
        f_out << word_to_cpp_string_literal(tag_name);
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
             "e.caught(true);\n"
             "context->set_error(e);\n"
             "}\n";
}


void Parser::control_for(control_t & control_info)
{
    if(control_info.f_max_args != 2)
    {
        throw std::logic_error("primitive \"for\" called with a number of parameters not equal to 2.");
    }

    // the first argument must be a TOK_LIST
    //
    Token::pointer_t arg(control_info.f_function_call->get_list_item(0));
    if(arg->get_token() != token_t::TOK_LIST)
    {
        arg->error("unexpected token type ("
                  + to_string(arg->get_token())
                  + ") for the \"for\" instruction; expected a LIST.");
        return;
    }

    size_t const max_ctrl(arg->get_list_size());
    if(max_ctrl != 3
    && max_ctrl != 4)
    {
        arg->error("unexpected number of arguments in the \"for\" instruction control list. Expected 3 or 4 items, got "
                  + std::to_string(max_ctrl)
                  + " instead.");
        return;
    }

    Token::pointer_t var_name(arg->get_list_item(0));
    if(var_name->get_token() != token_t::TOK_WORD
    && var_name->get_token() != token_t::TOK_QUOTED
    && var_name->get_token() != token_t::TOK_THING)
    {
        arg->error("unexpected token type ("
                  + to_string(var_name->get_token())
                  + ") for the \"for\" instruction; the first item in the control list is expected to be a word.");
        return;
    }

    std::string ctrl_var_name[3];
    double ctrl_var_value[3]{ 0, 0, 0 };
    bool direct_value[3]{ false, false, false };
    bool all_direct(true);
    for(size_t idx(0); idx + 1 < max_ctrl; ++idx)
    {
        Token::pointer_t ctrl_range(arg->get_list_item(idx + 1));

        switch(ctrl_range->get_token())
        {
        case token_t::TOK_FUNCTION_CALL:
            // the output of a function call will stack a parameter
            //
            all_direct = false;
            ctrl_var_name[idx] = get_unique_name();
            f_out << "lpp::lpp__value::pointer_t "
                  << ctrl_var_name[idx]
                  << ";\n";
            output_function_call(ctrl_range, ctrl_var_name[idx]);
            break;

        case token_t::TOK_INTEGER:
            direct_value[idx] = true;
            ctrl_var_value[idx] = static_cast<float_t>(ctrl_range->get_integer());
            break;

        case token_t::TOK_FLOAT:
            direct_value[idx] = true;
            ctrl_var_value[idx] = ctrl_range->get_float();
            break;

        case token_t::TOK_THING:
            all_direct = false;
            ctrl_var_name[idx] = get_unique_name();
            f_out << "lpp::lpp__value::pointer_t "
                  << ctrl_var_name[idx]
                  << "(context->get_thing("
                  << word_to_cpp_string_literal(ctrl_range->get_word())
                  << ")->get_value());\n";
            break;

        // TODO: support a list as well, although we don't need it in
        //       our Logo so at this point it's not available
        //case token_t::TOK_LIST:
        //    ctrl_var_name[idx] = get_unique_name();
        //    f_out << "lpp::lpp__value::pointer_t "
        //          << ctrl_var_name[idx]
        //          << ";\n";
        //    output_function_call(arg, ctrl_var_name[idx]);
        //    break;

        default:
            arg->error("unexpected token type ("
                      + to_string(ctrl_range->get_token())
                      + ") for one of the \"for\" loop numbers.");
            return;

        }
    }

    std::string const repeat_var(get_unique_name());
    if(all_direct)
    {
        // in this one special case, we can generate the for() loop
        // very easily
        //
        bool growing(true);
        if(max_ctrl == 3)
        {
            growing = ctrl_var_value[0] <= ctrl_var_value[1];
            if(growing)
            {
                ctrl_var_value[2] = 1.0;
            }
            else
            {
                ctrl_var_value[2] = -1.0;
            }
        }
        else if(ctrl_var_value[2] < 0)
        {
            growing = false;
        }

        f_out << "for(lpp::lpp__float_t "
              << repeat_var
              << "("
              << ctrl_var_value[0]
              << ");"
              << repeat_var
              << (growing ? "<=" : ">=")
              << ctrl_var_value[1]
              << ";"
              << repeat_var
              << "+="
              << ctrl_var_value[2]
              << ")\n"
                 "{\n"
                 "lpp::lpp__value::pointer_t "
              << repeat_var
              << "_value(std::make_shared<lpp::lpp__value>("
              << repeat_var
              << "));\n"
                 "context->set_thing("
              << word_to_cpp_string_literal(var_name->get_word())
              << ","
              << repeat_var
              << "_value,lpp::lpp__thing_type_t::LPP__THING_TYPE_PROCEDURE);\n";

        Token::pointer_t instruction_list(control_info.f_function_call->get_list_item(1));
        Token::pointer_t instructions(parse_body(instruction_list));
        output_body(instructions);

        f_out << "}\n";
    }
    else
    {
        for(size_t idx(0); idx + 1 < max_ctrl; ++idx)
        {
            if(!direct_value[0])
            {
                f_out << "lpp::lpp__float_t "
                      << ctrl_var_name[idx]
                      << "_for(0);\n"
                         "switch("
                      << ctrl_var_name[idx]
                      << "->type())\n"
                         "{\n"
                         "case lpp::lpp__value_type_t::LPP__VALUE_TYPE_INTEGER:\n"
                      << ctrl_var_name[idx]
                      << "_for=static_cast<lpp::lpp__float_t>("
                      << ctrl_var_name[idx]
                      << "->get_integer());\n"
                         "break;\n"
                         "case lpp::lpp__value_type_t::LPP__VALUE_TYPE_FLOAT:\n"
                      << ctrl_var_name[idx]
                      << "_for="
                      << ctrl_var_name[idx]
                      << "->get_float();\n"
                         "break;\n"
                         "default:\n"
                         "throw lpp::lpp__error(context,lpp::lpp__error_code_t::ERROR_CODE_INVALID_DATUM,\"error\",\"for control values must be numbers.\");\n"
                         "}\n";
            }
        }

        if(max_ctrl == 3)
        {
            // dynamically determine the step size (1.0 or -1.0)
            //
            ctrl_var_name[2] = get_unique_name();
            f_out << "lpp::lpp__float_t "
                  << ctrl_var_name[2]
                  << "_for(";
            if(direct_value[0])
            {
                f_out << direct_value[0];
            }
            else
            {
                f_out << ctrl_var_name[0]
                      << "_for";
            }
            f_out << "<=";
            if(direct_value[1])
            {
                f_out << direct_value[1];
            }
            else
            {
                f_out << ctrl_var_name[1]
                      << "_for";
            }
            f_out << "?1.0:-1.0);\n";
        }

        f_out << "for(lpp::lpp__float_t "
              << repeat_var
              << "("
              << (direct_value[0] ? std::to_string(ctrl_var_value[0]) : ctrl_var_name[0] + "_for")
              << ");";

        if(direct_value[2])
        {
            f_out << repeat_var
                  << (ctrl_var_value[2] >= 0 ? "<=" : ">=")
                  << (direct_value[1] ? std::to_string(ctrl_var_value[1]) : ctrl_var_name[1] + "_for");
        }
        else
        {
            f_out << ctrl_var_name[2]
                  << "_for>=0?"
                  << repeat_var
                  << "<="
                  << (direct_value[1] ? std::to_string(ctrl_var_value[1]) : ctrl_var_name[1] + "_for")
                  << ":"
                  << repeat_var
                  << ">="
                  << (direct_value[1] ? std::to_string(ctrl_var_value[1]) : ctrl_var_name[1] + "_for");
        }

        f_out << ";"
              << repeat_var
              << "+="
              << (direct_value[2] ? std::to_string(ctrl_var_value[2]) : ctrl_var_name[2] + "_for")
              << ")\n"
                 "{\n"
                 "lpp::lpp__value::pointer_t "
              << repeat_var
              << "_value(std::make_shared<lpp::lpp__value>("
              << repeat_var
              << "));\n"
                 "context->set_thing("
              << word_to_cpp_string_literal(var_name->get_word())
              << ","
              << repeat_var
              << "_value,lpp::lpp__thing_type_t::LPP__THING_TYPE_PROCEDURE);\n";

        Token::pointer_t instruction_list(control_info.f_function_call->get_list_item(1));
        Token::pointer_t instructions(parse_body(instruction_list));
        output_body(instructions);

        f_out << "}\n";
    }
}


void Parser::control_forever(control_t & control_info)
{
    if(control_info.f_max_args != 1)
    {
        throw std::logic_error("primitive \"forever\" called with a number of parameters not equal to 1.");
    }

    std::string value_name;
    Token::pointer_t arg(control_info.f_function_call->get_list_item(0));

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

    Token::pointer_t instruction_list(control_info.f_function_call->get_list_item(0));
    Token::pointer_t instructions(parse_body(instruction_list));
    output_body(instructions);

    f_out << "}\n";
}


void Parser::control_goto(control_t & control_info)
{
    if(control_info.f_max_args != 1)
    {
        throw std::logic_error("primitive \"goto\" called with a number of parameters not equal to 1.");
    }

    std::string value_name;
    Token::pointer_t arg(control_info.f_function_call->get_list_item(0));
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
        if(control_info.f_max_args != 3)
        {
            throw std::logic_error("primitive \"ifelse\" called with a number of parameters not equal to 3.");
        }
    }
    else
    {
        if(control_info.f_max_args != 2
        && control_info.f_max_args != 3)
        {
            throw std::logic_error("primitive \"if\" called with a number of parameters not equal to 2 or 3.");
        }
    }

    std::string value_name;
    Token::pointer_t arg(control_info.f_function_call->get_list_item(0));

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
              << word_to_cpp_string_literal(arg->get_word())
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
            Token::pointer_t instruction_list(control_info.f_function_call->get_list_item(1));
            Token::pointer_t instructions(parse_body(instruction_list));
            output_body(instructions);
        }
        else if(control_info.f_max_args == 3)
        {
            Token::pointer_t instruction_list(control_info.f_function_call->get_list_item(2));
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
            Token::pointer_t instruction_list(control_info.f_function_call->get_list_item(1));
            Token::pointer_t instructions(parse_body(instruction_list));
            output_body(instructions);
        }

        f_out << "}\n";

        if(control_info.f_max_args == 3)
        {
            f_out << "else\n"
                  << "{\n";

            Token::pointer_t instruction_list(control_info.f_function_call->get_list_item(2));
            Token::pointer_t instructions(parse_body(instruction_list));
            output_body(instructions);

            f_out << "}\n";
        }
    }
}


void Parser::control_if_test(control_t & control_info, bool true_or_false)
{
    if(control_info.f_max_args != 1)
    {
        throw std::logic_error("primitives \"iftrue\" and \"iffalse\" called with a number of parameters not equal to 1.");
    }

    f_out << "if(context->get_test()==lpp::test_t::TEST_"
          << (true_or_false ? "TRUE" : "FALSE")
          << ")\n"
             "{\n";

    Token::pointer_t instruction_list(control_info.f_function_call->get_list_item(0));
    Token::pointer_t instructions(parse_body(instruction_list));
    output_body(instructions);

    f_out << "}\n";
}


void Parser::control_output(control_t & control_info)
{
    std::string const value_name(get_unique_name());
    output_argument(control_info.f_function_call->get_list_item(0), value_name);
    f_out << "context->set_return_value("
          << value_name
          << ");\n";

    if(f_enable_trace)
    {
        f_out << "context->trace_procedure(lpp::trace_mode_t::TRACE_MODE_OUTPUT,"
              << value_name
              << ");\n";
    }

    f_out << "return;\n";
}


void Parser::control_repeat(control_t & control_info)
{
    if(control_info.f_max_args != 2)
    {
        throw std::logic_error("primitive \"repeat\" called with a number of parameters not equal to 2.");
    }

    std::string value_name;
    Token::pointer_t arg(control_info.f_function_call->get_list_item(0));

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
              << word_to_cpp_string_literal(arg->get_word())
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

    Token::pointer_t instruction_list(control_info.f_function_call->get_list_item(1));
    Token::pointer_t instructions(parse_body(instruction_list));
    output_body(instructions);

    f_out << "}\n";
}


void Parser::control_stop(control_t & control_info)
{
    static_cast<void>(control_info);

    if(f_enable_trace)
    {
        f_out << "context->trace_procedure(lpp::trace_mode_t::TRACE_MODE_STOP,lpp::lpp__value::pointer_t());\n";
    }

    f_out << "return;\n";
}


void Parser::control_tag(control_t & control_info)
{
    if(control_info.f_max_args != 1)
    {
        throw std::logic_error("primitive \"tag\" called with a number of parameters not equal to 1.");
    }

    std::string value_name;
    Token::pointer_t arg(control_info.f_function_call->get_list_item(0));
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
    if(control_info.f_max_args != 1
    && control_info.f_max_args != 2)
    {
        throw std::logic_error("primitive \"throw\" called with a number of parameters not equal to 1 or 2.");
    }

    // tag
    //
    std::string tag_value;
    std::string tag_name;
    bool direct_value(false);
    Token::pointer_t arg(control_info.f_function_call->get_list_item(0));
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
              << word_to_cpp_string_literal(arg->get_word())
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
    if(control_info.f_function_call->get_list_size() > 1)
    {
        value_name = get_unique_name();
        output_argument(control_info.f_function_call->get_list_item(0), value_name);
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


void Parser::control_while(control_t & control_info, bool until, bool once)
{
    if(control_info.f_max_args != 2)
    {
        throw std::logic_error(std::string("primitive \"")
                             + (until ? "until" : "while")
                             + "\" called with a number of parameters not equal to 2.");
    }

    std::string value_name;
    Token::pointer_t arg(control_info.f_function_call->get_list_item(0));

    bool direct_value(false);
    bool tf(false);

    switch(arg->get_token())
    {
    case token_t::TOK_FUNCTION_CALL:
        // the output of a function call will stack a parameter
        //
        value_name = get_unique_name();
        f_out << "for(;;)\n"
                 "{\n";

        if(once)
        {
            Token::pointer_t instruction_list(control_info.f_function_call->get_list_item(1));
            Token::pointer_t instructions(parse_body(instruction_list));
            output_body(instructions);
        }

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
        f_out << "for(;;)\n"
                 "{\n";

        if(once)
        {
            Token::pointer_t instruction_list(control_info.f_function_call->get_list_item(1));
            Token::pointer_t instructions(parse_body(instruction_list));
            output_body(instructions);
        }

        f_out << "lpp::lpp__value::pointer_t "
              << value_name
              << "(context->get_thing("
              << word_to_cpp_string_literal(arg->get_word())
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
        // if false (while) or true (until), it never executes
        //
        if(!tf ^ until)
        {
            return;
        }

        // otherwise, it is equivalent to FOREVER
        //
        f_out << "for(;;)\n"
                 "{\n";

        Token::pointer_t instruction_list(control_info.f_function_call->get_list_item(1));
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
              << (until ? "" : "!")
              << tf_var
              << ")\n"
                 "{\n"
                 "break;\n"
                 "}\n";

        if(!once)
        {
            Token::pointer_t instruction_list(control_info.f_function_call->get_list_item(1));
            Token::pointer_t instructions(parse_body(instruction_list));
            output_body(instructions);
        }

        f_out << "}\n";
    }
}


} // lpp namespace

// vim: ts=4 sw=4 et nocindent
