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

#include "token.hpp"



namespace lpp
{



namespace
{


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
char const * const g_token_to_string[] =
{
    [static_cast<int>(token_t::TOK_EOF)]               = "EOF",
    [static_cast<int>(token_t::TOK_VOID)]              = "VOID",
    [static_cast<int>(token_t::TOK_MAP)]               = "MAP",
    [static_cast<int>(token_t::TOK_FUNCTION_CALL)]     = "FUNCTION_CALL",
    [static_cast<int>(token_t::TOK_BOOLEAN)]           = "BOOLEAN",
    [static_cast<int>(token_t::TOK_CLOSE_LIST)]        = "CLOSE_LIST",
    [static_cast<int>(token_t::TOK_CLOSE_PARENTHESIS)] = "CLOSE_PARENTHESIS",
    [static_cast<int>(token_t::TOK_DECLARE)]           = "DECLARE",
    [static_cast<int>(token_t::TOK_DIFFERENCE)]        = "DIFFERENCE",
    [static_cast<int>(token_t::TOK_END)]               = "END",
    [static_cast<int>(token_t::TOK_EQUALP)]            = "EQUALP",
    [static_cast<int>(token_t::TOK_FLOAT)]             = "FLOAT",
    [static_cast<int>(token_t::TOK_GREATEREQUALP)]     = "GREATEREQUALP",
    [static_cast<int>(token_t::TOK_GREATERP)]          = "GREATERP",
    [static_cast<int>(token_t::TOK_INTEGER)]           = "INTEGER",
    [static_cast<int>(token_t::TOK_LESSEQUALP)]        = "LESSEQUALP",
    [static_cast<int>(token_t::TOK_LESSP)]             = "LESSP",
    [static_cast<int>(token_t::TOK_NOTEQUALP)]         = "NOTEQUALP",
    [static_cast<int>(token_t::TOK_OPEN_LIST)]         = "OPEN_LIST",
    [static_cast<int>(token_t::TOK_OPEN_PARENTHESIS)]  = "OPEN_PARENTHESIS",
    [static_cast<int>(token_t::TOK_PRODUCT)]           = "PRODUCT",
    [static_cast<int>(token_t::TOK_QUOTED)]            = "QUOTED",
    [static_cast<int>(token_t::TOK_QUOTIENT)]          = "QUOTIENT",
    [static_cast<int>(token_t::TOK_SUM)]               = "SUM",
    [static_cast<int>(token_t::TOK_THING)]             = "THING",
    [static_cast<int>(token_t::TOK_TO)]                = "TO",
    [static_cast<int>(token_t::TOK_WORD)]              = "WORD",
    [static_cast<int>(token_t::TOK_LIST)]              = "LIST"
};
#pragma GCC diagnostic pop



} // no name namespace



std::string to_string(token_t token)
{
    if(static_cast<size_t>(token) >= sizeof(g_token_to_string) / sizeof(g_token_to_string[0]))
    {
        throw std::logic_error("to_string(token_t) called when an out of bounds token.");
    }

    if(g_token_to_string[static_cast<int>(token)] == nullptr)
    {
        throw std::logic_error("to_string(token_t) called with a token which name is not defined.");
    }

    return g_token_to_string[static_cast<int>(token)];
}


Token::Token(token_t token)
    : f_token(token)
{
}


void Token::set_token(token_t token)
{
    switch(f_token)
    {
    case token_t::TOK_FLOAT:
    case token_t::TOK_INTEGER:
    case token_t::TOK_QUOTED:
    case token_t::TOK_THING:
    case token_t::TOK_WORD:
        throw std::logic_error("set_token() called with token type TOK_FLOAT/INTEGER/QUOTED/THING/WORD");

    default:
        f_token = token;
        break;

    }
}


void Token::set_word(token_t token, std::string const & word, bool start_of_line)
{
    switch(token)
    {
    case token_t::TOK_QUOTED:
    case token_t::TOK_THING:
        if(start_of_line)
        {
            throw std::logic_error("set_word() does not accept start_of_line == true when the token is TOK_QUOTED or TOK_THING");
        }
        break;

    case token_t::TOK_WORD:
        break;

    default:
        throw std::logic_error("set_word() called when the token type is not TOK_QUOTED/THING/WORD");

    }

    f_token = token;
    f_start_of_line = start_of_line;
    f_word = word;
}


void Token::set_integer(std::int64_t value)
{
    f_token = token_t::TOK_INTEGER;
    f_integer = value;
}


void Token::set_float(double value)
{
    f_token = token_t::TOK_FLOAT;
    f_float = value;
}


void Token::set_boolean(bool value)
{
    f_token = token_t::TOK_BOOLEAN;
    f_boolean = value;
}


void Token::set_function_limits(argument_count_t min_args, argument_count_t def_args, argument_count_t max_args)
{
    if(f_token != token_t::TOK_LIST)
    {
        throw std::logic_error("set_function_limits() called when the token type is not TOK_LIST");
    }

    f_min_args = min_args;
    f_def_args = def_args;
    f_max_args = max_args;
}


void Token::add_list_item(Token::pointer_t item)
{
    if(f_token != token_t::TOK_LIST)
    {
        throw std::logic_error("add_list_item() called when the token type is not TOK_LIST");
    }

    f_list.push_back(item);
}


void Token::add_map_item(std::string const & name, Token::pointer_t item)
{
    if(f_token != token_t::TOK_MAP)
    {
        throw std::logic_error("add_map_item() called when the token type is not TOK_MAP");
    }

    f_map[name] = item;
}


token_t Token::get_token() const
{
    return f_token;
}


bool Token::get_start_of_line() const
{
    return f_start_of_line;
}


std::string const & Token::get_word() const
{
    switch(f_token)
    {
    case token_t::TOK_QUOTED:
    case token_t::TOK_THING:
    case token_t::TOK_WORD:
        break;

    default:
        throw std::logic_error("get_word() called when the token type is not TOK_QUOTED/THING/WORD");

    }

    return f_word;
}


std::int64_t Token::get_integer() const
{
    if(f_token != token_t::TOK_INTEGER)
    {
        throw std::logic_error("get_integer() called when the token type is not TOK_INTEGER");
    }

    return f_integer;
}


double Token::get_float() const
{
    if(f_token != token_t::TOK_FLOAT)
    {
        throw std::logic_error("get_float() called when the token type is not TOK_FLOAT");
    }

    return f_float;
}


bool Token::get_boolean() const
{
    if(f_token != token_t::TOK_BOOLEAN)
    {
        throw std::logic_error("get_boolean() called when the token type is not TOK_BOOLEAN");
    }

    return f_boolean;
}


Token::argument_count_t Token::get_min_args() const
{
    if(f_token != token_t::TOK_LIST)
    {
        throw std::logic_error("get_min_args() called when the token type is not TOK_LIST");
    }

    return f_min_args;
}


Token::argument_count_t Token::get_def_args() const
{
    if(f_token != token_t::TOK_LIST)
    {
        throw std::logic_error("get_def_args() called when the token type is not TOK_LIST");
    }

    return f_def_args;
}


Token::argument_count_t Token::get_max_args() const
{
    if(f_token != token_t::TOK_LIST)
    {
        throw std::logic_error("get_max_args() called when the token type is not TOK_LIST");
    }

    return f_max_args;
}


Token::vector_t::size_type Token::get_list_size() const
{
    if(f_token != token_t::TOK_LIST)
    {
        throw std::logic_error("get_list_size() called when the token type is not TOK_LIST");
    }

    return f_list.size();
}


Token::pointer_t Token::get_list_item(vector_t::size_type idx) const
{
    if(f_token != token_t::TOK_LIST)
    {
        throw std::logic_error("get_list_item() called when the token type is not TOK_LIST");
    }

    if(idx >= f_list.size())
    {
        throw std::logic_error("trying to get a list item with an index out of range.");
    }

    return f_list[idx];
}


Token::pointer_t Token::get_map_item(std::string const & name) const
{
    if(f_token != token_t::TOK_MAP)
    {
        throw std::logic_error("get_map_item() called when the token type is not TOK_MAP");
    }

    auto it(f_map.find(name));
    if(it == f_map.end())
    {
        return Token::pointer_t();
    }

    return it->second;
}



} // lpp namespace

// vim: ts=4 sw=4 et nocindent
