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

#include "parser.hpp"

#include "error.hpp"

#include <limits>

namespace lpp
{


Parser::Parser(Lexer & lexer)
    : f_lexer(lexer)
{
}


/** \brief Parse the input file.
 *
 * This function reads the input file by repeatitively calling the
 * Lexer::next_token() function. It returns once the EOF is reached.
 *
 * The grammar looks as follow:
 *
 * \code
 * start:
 *     input_lines
 *
 * input_lines:
 *     line
 *   | input_lines line
 *
 * line:
 *     to_definition
 *   | declaration
 *   | call_function
 *
 * declaration:
 *     DECLARE WORD thing_list optional_thing_list rest_thing default_number_of_inputs
 *
 * to_definition:
 *     TO WORD thing_list optional_thing_list rest_thing default_number_of_inputs list_of_lines END
 *
 * thing_list:
 *     <empty>
 *   | THING
 *   | thing_list THING
 *
 * optional_thing_list:
 *     <empty>
 *   | OPEN_LIST THING expression CLOSE_LIST
 *
 * rest_thing:
 *     <empty>
 *   | OPEN_LIST THING CLOSE_LIST
 *
 * default_number_of_inputs:
 *     <empty>
 *   | INTEGER
 *
 * list_of_lines:
 *     call_function
 *   | list_of_lines call_function
 *
 * call_function:
 *     WORD arguments_n
 *   | OPEN_PARENTHESIS WORD expression_list CLOSE_PARENTHESIS
 *
 * arguments_n:
 *     expression
 *   | arguments_n expression
 *
 * expression_list:
 *     expression
 *   | expression_list expression
 *
 * expression:
 *     equality_expression
 *
 * equality_expression:
 *     relational_expression EQUALP relational_expression
 *   | relational_expression NOTEQUALP relational_expression
 *
 * relational_expression:
 *     additive_expression LESSP additive_expression
 *   | additive_expression LESSEQUALP additive_expression
 *   | additive_expression GREATERP additive_expression
 *   | additive_expression GREATEREQUALP additive_expression
 *
 * additive_expression:
 *     multiplicative_expression SUM multiplicative_expression
 *   | multiplicative_expression DIFFERENCE multiplicative_expression
 *
 * multiplicative_expression:
 *     unary_expression PRODUCT unary_expression
 *   | unary_expression QUOTIENT unary_expression
 *
 * unary_expression:
 *     INTEGER
 *   | FLOAT
 *   | BOOLEAN
 *   | QUOTED
 *   | SUM unary_expression
 *   | DIFFERENCE unary_expression
 *   | OPEN_PARENTHESIS expression CLOSE_PARENTHESIS
 *   | OPEN_LIST expression_list CLOSE_LIST
 *   | call_function
 * \endcode
 */
void Parser::parse()    // => "start: ..."
{
    if(f_program == nullptr)
    {
        f_program = std::make_shared<Token>();
        f_program->set_token(token_t::TOK_LIST);    // list of "instructions"
    }

    if(f_declarations == nullptr)
    {
        f_declarations = std::make_shared<Token>();
        f_declarations->set_token(token_t::TOK_MAP);    // list of DECLARE ...
    }

    if(f_procedures == nullptr)
    {
        f_procedures = std::make_shared<Token>();
        f_procedures->set_token(token_t::TOK_MAP);    // list of TO ...
    }

    input_lines();
}


Token::pointer_t Parser::next_token()
{
    f_current_token = f_lexer.next_token();
    return f_current_token;
}


void Parser::input_lines()
{
    for(;;)
    {
        next_token();
        if(f_current_token->get_token() == token_t::TOK_EOF)
        {
            return;
        }
        line();
    }
}


void Parser::line()
{
    switch(f_current_token->get_token())
    {
    case token_t::TOK_TO:
        to_definition(false);
        break;

    case token_t::TOK_DECLARE:
        to_definition(true);
        break;

    case token_t::TOK_WORD:
    case token_t::TOK_OPEN_PARENTHESIS:
        // program are instructions outside of a procedure
        //
        f_program->add_list_item(call_function());
        break;

    default:
        error("expected a WORD, '(', or TO token.");
        break;

    }
}


void Parser::to_definition(bool declaration_only)
{
    // TO WORD
    //    ^
    //
    Token::pointer_t name(next_token());
    if(name->get_token() != token_t::TOK_WORD
    && name->get_token() != token_t::TOK_QUOTED) // accept `TO "name` as well
    {
        error("TO definition expects a WORD representing the procedure name as its first parameter.");
        // TODO: search the END token
        return;
    }


    // TO WORD thing_list
    //         ^
    //
    Token::pointer_t required_arguments(std::make_shared<Token>(token_t::TOK_LIST));

    Token::argument_count_t min_args(0);
    Token::pointer_t arg;
    for(;;)
    {
        arg = next_token();
        if(arg->get_token() != token_t::TOK_THING
        && arg->get_token() != token_t::TOK_WORD
        && arg->get_token() != token_t::TOK_QUOTED)
        {
            break;
        }
        required_arguments->add_list_item(arg);
        ++min_args;
    }

    Token::argument_count_t max_args(min_args);
    Token::argument_count_t def_args(min_args);

    // TO WORD thing_list optional_thing_list
    //                    ^
    //
    Token::pointer_t optional_arguments(std::make_shared<Token>(token_t::TOK_LIST));

    Token::pointer_t rest_argument;

    for(;;)
    {
        // TO WORD ... [ THING expr ]
        //             ^
        //
        if(arg->get_token() != token_t::TOK_OPEN_LIST)
        {
            break;
        }

        // TO WORD ... [ THING expr ]
        //               ^
        //
        arg = next_token();
        if(arg->get_token() != token_t::TOK_THING
        && arg->get_token() != token_t::TOK_WORD
        && arg->get_token() != token_t::TOK_QUOTED)
        {
            error("Lists of optional parameters or the REST must start with a THING: [:VARNAME ...]");
            // TODO: parse up to END
            return;
        }

        Token::pointer_t t(next_token());
        if(t->get_token() == token_t::TOK_CLOSE_LIST)
        {
            // TO WORD ... [ THING ]
            //                     ^
            //
            rest_argument = arg;
            max_args = std::numeric_limits<Token::argument_count_t>::max();
            break;
        }

        // otherwise it has to be an expression representing the default value
        //
        // TO WORD ... [ THING expr ]
        //                          ^
        //
        Token::pointer_t expr(expression()); // defaults can't be parsed now

        Token::pointer_t default_arguments(std::make_shared<Token>(token_t::TOK_LIST));
        default_arguments->add_list_item(arg);
        default_arguments->add_list_item(expr);

        optional_arguments->add_list_item(default_arguments);
        ++max_args;

        // TO WORD thing_list [ THING expr ]
        //                                 ^
        //
        arg = next_token();
        if(arg->get_token() != token_t::TOK_CLOSE_LIST)
        {
            error("optional parameter default value must be an expression immediately followed by a ']'");
            // TODO: skip to END
            return;
        }

        arg = next_token();
    }

    // TO WORD thing_list [ THING expr ] [ THING ] default_number_of_inputs
    //                                             ^
    //
    if(arg->get_token() == token_t::TOK_INTEGER)
    {
        def_args = arg->get_integer();
        if(def_args < min_args)
        {
            error("the default number of parameter (" + std::to_string(def_args)
                + ") cannot be smaller than the minimum number of parameters (" + std::to_string(min_args)
                + ").");
            return;
        }
        if(def_args > max_args)
        {
            error("the default number of parameter (" + std::to_string(def_args)
                + ") cannot be larger than the maximum number of parameters (" + std::to_string(max_args)
                + ").");
            return;
        }
        next_token();
    }

    Token::pointer_t declaration(std::make_shared<Token>());
    declaration->set_token(token_t::TOK_LIST);
    declaration->add_list_item(name);
    declaration->add_list_item(required_arguments);
    declaration->add_list_item(optional_arguments);
    if(rest_argument != nullptr)
    {
        declaration->add_list_item(rest_argument);
    }
    declaration->set_function_limits(min_args, def_args, max_args);

    // TO or DECLARE ... are added to the declaration list
    //
    f_declarations->add_map_item(name->get_word(), declaration);

    if(declaration_only)
    {
        return;
    }

    // the rest of the procedure is read as a vector of tokens and
    // not parsed yet because LOGO allows for _late declarations_
    // (i.e. for A to call B and B to call A you need to define
    // A and B then parse A and B)
    //
    // Note: the declaration is required to know how many parameters
    //       are passed to a procedure hence this "quirk"

    // read tokens up to END
    //

    Token::pointer_t body(std::make_shared<Token>());
    body->set_token(token_t::TOK_LIST);
    declaration->add_list_item(body);

    f_procedures->add_map_item(name->get_word(), declaration);

    for(;;)
    {
        if(f_current_token->get_token() == token_t::TOK_EOF)
        {
            error("end of file found before end of procedure, END is missing.");
            return;
        }

        if(f_current_token->get_token() == token_t::TOK_END)
        {
            break;
        }
        body->add_list_item(f_current_token);
        next_token();
    }

    // we allow the END keyword to be followed by a quoted name which
    // has to match the TO <name>
    //
    next_token();
    if(f_current_token->get_token() == token_t::TOK_QUOTED)
    {
        if(f_current_token->get_word() != name->get_word())
        {
            error("found `to \"" + name->get_word()
                + "` however we found `end \"" + f_current_token->get_word()
                + "`");
        }
    }
}


Token::pointer_t Parser::call_function()
{
    bool const parenthesis(f_current_token->get_token() == token_t::TOK_OPEN_PARENTHESIS);
    if(parenthesis)
    {
        next_token();
        if(f_current_token->get_token() != token_t::TOK_WORD)
        {
            error("expected a WORD token after '('.");
            return f_current_token;
        }

    }

    Token::pointer_t procedure(f_current_token);

    // now t is a WORD, we can search the procedure declaration to
    // make sure it is defined and if we did not have a parenthesis
    // we can use the number of arguments expected
    //
    Token::pointer_t declaration(f_declarations->get_map_item(procedure->get_word()));
    if(declaration == nullptr)
    {
        error("unknown command \"" + procedure->get_word() + "\"");
        return f_current_token;
    }

    Token::argument_count_t min_args(declaration->get_min_args());
    Token::argument_count_t def_args(declaration->get_def_args());
    Token::argument_count_t max_args(std::numeric_limits<Token::argument_count_t>::max());
    if(!parenthesis)
    {
        max_args = declaration->get_def_args();
    }

    // change the WORD into a FUNCTION-CALL
    //
    procedure->set_token(token_t::TOK_FUNCTION_CALL);

    for(;;)
    {
        if(parenthesis)
        {
            if(f_current_token->get_token() == token_t::TOK_CLOSE_PARENTHESIS)
            {
                next_token();
                break;
            }
        }
        else
        {
            if((f_current_token->get_token() == token_t::TOK_WORD
                    && f_current_token->get_start_of_line()
                    && procedure->get_list_size() >= min_args)
            || procedure->get_list_size() >= def_args)
            {
                break;
            }
        }

        procedure->add_list_item(expression());
    }

    return procedure;
}


Token::pointer_t Parser::expression()
{
    return equality_expression();
}


Token::pointer_t Parser::equality_expression()
{
    Token::pointer_t lhs(relational_expression());

    for(;;)
    {
        token_t const token(f_current_token->get_token());
        switch(token)
        {
        case token_t::TOK_EQUALP:
        case token_t::TOK_NOTEQUALP:
            {
                Token::pointer_t rhs(relational_expression());

                Token::pointer_t op(std::make_shared<Token>());
                op->set_word(token_t::TOK_FUNCTION_CALL, to_string(token));
                op->add_list_item(lhs);
                op->add_list_item(rhs);

                lhs = op;
            }
            break;

        default:
            return lhs;

        }
    }
}


Token::pointer_t Parser::relational_expression()
{
    Token::pointer_t lhs(additive_expression());

    for(;;)
    {
        token_t const token(f_current_token->get_token());
        switch(token)
        {
        case token_t::TOK_LESSP:
        case token_t::TOK_LESSEQUALP:
        case token_t::TOK_GREATERP:
        case token_t::TOK_GREATEREQUALP:
            {
                Token::pointer_t rhs(additive_expression());

                Token::pointer_t op(std::make_shared<Token>());
                op->set_word(token_t::TOK_FUNCTION_CALL, to_string(token));
                op->add_list_item(lhs);
                op->add_list_item(rhs);

                lhs = op;
            }
            break;

        default:
            return lhs;

        }
    }
}


Token::pointer_t Parser::additive_expression()
{
    Token::pointer_t lhs(multiplicative_expression());

    for(;;)
    {
        token_t const token(f_current_token->get_token());
        switch(token)
        {
        case token_t::TOK_SUM:
        case token_t::TOK_DIFFERENCE:
            {
                Token::pointer_t rhs(multiplicative_expression());

                Token::pointer_t op(std::make_shared<Token>());
                op->set_word(token_t::TOK_FUNCTION_CALL, to_string(token));
                op->add_list_item(lhs);
                op->add_list_item(rhs);

                lhs = op;
            }
            break;

        default:
            return lhs;

        }
    }
}


Token::pointer_t Parser::multiplicative_expression()
{
    Token::pointer_t lhs(unary_expression());

    for(;;)
    {
        token_t const token(f_current_token->get_token());
        switch(token)
        {
        case token_t::TOK_PRODUCT:
        case token_t::TOK_QUOTIENT:
            {
                Token::pointer_t rhs(unary_expression());

                Token::pointer_t op(std::make_shared<Token>());
                op->set_word(token_t::TOK_FUNCTION_CALL, to_string(token));
                op->add_list_item(lhs);
                op->add_list_item(rhs);

                lhs = op;
            }
            break;

        default:
            return lhs;

        }
    }
}


Token::pointer_t Parser::unary_expression()
{
    switch(f_current_token->get_token())
    {
    case token_t::TOK_INTEGER:
    case token_t::TOK_FLOAT:
    case token_t::TOK_BOOLEAN:
    case token_t::TOK_QUOTED:
        return f_current_token;

    case token_t::TOK_SUM:
        {
            Token::pointer_t rhs(unary_expression());

            Token::pointer_t op(std::make_shared<Token>());
            op->set_word(token_t::TOK_FUNCTION_CALL, "PLUS");
            op->add_list_item(rhs);
            return op;
        }
        break;

    case token_t::TOK_DIFFERENCE:
        {
            Token::pointer_t rhs(unary_expression());

            Token::pointer_t op(std::make_shared<Token>());
            op->set_word(token_t::TOK_FUNCTION_CALL, "MINUS");
            op->add_list_item(rhs);
            return op;
        }
        break;

    case token_t::TOK_OPEN_PARENTHESIS:
        {
            Token::pointer_t rhs(expression());

            if(f_current_token->get_token() != token_t::TOK_CLOSE_PARENTHESIS)
            {
                error("expression expected a closing parenthesis");
            }
            return rhs;
        }

    case token_t::TOK_OPEN_LIST:
        {
            Token::pointer_t list(std::make_shared<Token>(token_t::TOK_LIST));

            for(;;)
            {
                next_token();
                if(f_current_token->get_token() == token_t::TOK_CLOSE_LIST)
                {
                    break;
                }
                list->add_list_item(expression());
            }
            return list;
        }

    case token_t::TOK_WORD:
        return call_function();

    default:
        error("invalid token (" + to_string(f_current_token->get_token()) + ") for an expression");
        return std::make_shared<Token>(token_t::TOK_VOID);

    }
}




} // lpp namespace

// vim: ts=4 sw=4 et nocindent
