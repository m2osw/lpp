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
    [static_cast<int>(token_t::TOK_COUNT)]             = "COUNT",
    [static_cast<int>(token_t::TOK_DECLARE)]           = "DECLARE",
    [static_cast<int>(token_t::TOK_DIFFERENCE)]        = "DIFFERENCE",
    [static_cast<int>(token_t::TOK_END)]               = "END",
    [static_cast<int>(token_t::TOK_EQUALP)]            = "EQUALP",
    [static_cast<int>(token_t::TOK_FLOAT)]             = "FLOAT",
    [static_cast<int>(token_t::TOK_FUNCTION)]          = "FUNCTION",
    [static_cast<int>(token_t::TOK_GREATEREQUALP)]     = "GREATEREQUALP",
    [static_cast<int>(token_t::TOK_GREATERP)]          = "GREATERP",
    [static_cast<int>(token_t::TOK_INTEGER)]           = "INTEGER",
    [static_cast<int>(token_t::TOK_LESSEQUALP)]        = "LESSEQUALP",
    [static_cast<int>(token_t::TOK_LESSP)]             = "LESSP",
    [static_cast<int>(token_t::TOK_MINUS)]             = "MINUS",
    [static_cast<int>(token_t::TOK_NOTEQUALP)]         = "NOTEQUALP",
    [static_cast<int>(token_t::TOK_OPEN_LIST)]         = "OPEN_LIST",
    [static_cast<int>(token_t::TOK_OPEN_PARENTHESIS)]  = "OPEN_PARENTHESIS",
    [static_cast<int>(token_t::TOK_PLUS)]              = "PLUS",
    [static_cast<int>(token_t::TOK_PRIMITIVE)]         = "PRIMITIVE",
    [static_cast<int>(token_t::TOK_PROCEDURE)]         = "PROCEDURE",
    [static_cast<int>(token_t::TOK_PRODUCT)]           = "PRODUCT",
    [static_cast<int>(token_t::TOK_PROGRAM)]           = "PROGRAM",
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


Token::Token(token_t token, std::string const & filename, line_t line)
    : f_token(token)
    , f_filename(filename)
    , f_line(line)
{
}


Token::Token(token_t token, Token::pointer_t location)
    : f_token(token)
    , f_filename(location->f_filename)
    , f_line(location->f_line)
{
}


void Token::set_token(token_t token)
{
    switch(token)
    {
    case token_t::TOK_FUNCTION_CALL:
    case token_t::TOK_THING:
        switch(f_token)
        {
        case token_t::TOK_WORD:
            f_token = token;
            return;

        default:
            break;

        }
        break;

    default:
        break;

    }

    switch(f_token)
    {
    case token_t::TOK_FLOAT:
    case token_t::TOK_INTEGER:
    case token_t::TOK_QUOTED:
    case token_t::TOK_THING:
    case token_t::TOK_WORD:
        throw std::logic_error("set_token("
                             + to_string(token)
                             + ") called with the current token type set to "
                             + to_string(f_token));

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
    case token_t::TOK_FUNCTION_CALL:
        break;

    default:
        throw std::logic_error("set_word() called with token type "
                             + to_string(token)
                             + " which is not one of TOK_QUOTED/THING/WORD/FUNCTION_CALL");

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


void Token::set_procedure_flags(procedure_flag_t flags)
{
    f_procedure_flags = flags;
}


void Token::add_procedure_flags(procedure_flag_t flags)
{
    f_procedure_flags |= flags;
}


void Token::remove_procedure_flags(procedure_flag_t flags)
{
    f_procedure_flags &= ~flags;
}


void Token::set_function_limits(argument_count_t min_args, argument_count_t def_args, argument_count_t max_args)
{
    if(f_token != token_t::TOK_LIST)
    {
        throw std::logic_error("set_function_limits() called when the token type is not TOK_LIST.");
    }

    f_min_args = min_args;
    f_def_args = def_args;
    f_max_args = max_args;
}


void Token::set_declaration(pointer_t declaration)
{
    if(f_token != token_t::TOK_FUNCTION_CALL)
    {
        throw std::logic_error("set_declaration() called when the token type is not TOK_FUNCTION_CALL.");
    }
    if(f_declaration != nullptr)
    {
        throw std::logic_error("set_declaration() called twice on the same token.");
    }

    f_declaration = declaration;
}


void Token::add_list_item(Token::pointer_t item)
{
    switch(f_token)
    {
    case token_t::TOK_LIST:
    case token_t::TOK_FUNCTION_CALL:
        break;

    default:
        throw std::logic_error("add_list_item() called with token "
                            + to_string(f_token)
                            + ", expected TOK_LIST/TOK_FUNCTION_CALL instead.");

    }

    f_list.push_back(item);
}


void Token::set_list_item(int idx, Token::pointer_t item)
{
    switch(f_token)
    {
    case token_t::TOK_LIST:
    case token_t::TOK_FUNCTION_CALL:
        break;

    default:
        throw std::logic_error("set_list_item() called with token "
                            + to_string(f_token)
                            + ", expected TOK_LIST/TOK_FUNCTION_CALL instead.");

    }

    if(static_cast<size_t>(idx) >= f_list.size())
    {
        throw std::logic_error("set_list_item() called with index "
                            + std::to_string(idx)
                            + " which is out of range (max is "
                            + std::to_string(f_list.size())
                            + " for token "
                            + to_string(f_token)
                            + ".");
    }

    f_list[idx] = item;
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


std::string Token::get_word() const
{
    switch(f_token)
    {
    case token_t::TOK_QUOTED:
    case token_t::TOK_THING:
    case token_t::TOK_WORD:
    case token_t::TOK_FUNCTION_CALL:
        break;

    case token_t::TOK_LIST:
        if(!f_list.empty()
        && f_list[0]->f_token == token_t::TOK_WORD)
        {
            // special case for output_function_call() to print the
            // long name only
            //
            std::string const & name(f_list[0]->f_word);
            std::string::size_type const pos(name.find('&'));
            if(pos == std::string::npos)
            {
                return name;
            }
            return name.substr(0, pos);
        }
        /*[[fallthrough]]*/
    default:
        throw std::logic_error("get_word() called when the token type is TOK_"
                             + to_string(f_token)
                             + " not TOK_QUOTED/THING/WORD/FUNCTION_CALL");

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


procedure_flag_t Token::get_procedure_flags() const
{
    return f_procedure_flags;
}


argument_count_t Token::get_min_args() const
{
    if(f_token != token_t::TOK_LIST)
    {
        throw std::logic_error("get_min_args() called when the token type is not TOK_LIST");
    }

    return f_min_args;
}


argument_count_t Token::get_def_args() const
{
    if(f_token != token_t::TOK_LIST)
    {
        throw std::logic_error("get_def_args() called when the token type is not TOK_LIST.");
    }

    return f_def_args;
}


argument_count_t Token::get_max_args() const
{
    if(f_token != token_t::TOK_LIST)
    {
        throw std::logic_error("get_max_args() called when the token type is not TOK_LIST.");
    }

    return f_max_args;
}


Token::pointer_t Token::get_declaration() const
{
    if(f_token != token_t::TOK_FUNCTION_CALL)
    {
        throw std::logic_error("get_declaration() called when the token type is not TOK_FUNCTION_CALL.");
    }

    return f_declaration;
}


Token::vector_t::size_type Token::get_list_size() const
{
    switch(f_token)
    {
    case token_t::TOK_LIST:
    case token_t::TOK_FUNCTION_CALL:
        break;

    default:
        throw std::logic_error("get_list_size() called with the token type set to "
                             + to_string(f_token)
                             + ", it expects one of TOK_LIST/TOK_FUNCTION_CALL");

    }

    return f_list.size();
}


Token::pointer_t Token::get_list_item(vector_t::size_type idx) const
{
    switch(f_token)
    {
    case token_t::TOK_LIST:
    case token_t::TOK_FUNCTION_CALL:
        break;

    default:
        throw std::logic_error("get_list_item() called with the token type set to "
                             + to_string(f_token)
                             + ", it expects one of TOK_LIST/TOK_FUNCTION_CALL");

    }

    if(idx >= f_list.size())
    {
        throw std::logic_error("trying to get a list item with an index out of range.");
    }

    return f_list[idx];
}


Token::map_t const & Token::get_map() const
{
    return f_map;
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


std::string const & Token::get_filename() const
{
    return f_filename;
}


line_t Token::get_line() const
{
    return f_line;
}


void Token::error(std::string const & message)
{
    throw lpp_error(message, f_filename, f_line);
}



} // lpp namespace


std::ostream & operator << (std::ostream & out, lpp::Token const & token)
{
    switch(token.get_token())
    {
    case lpp::token_t::TOK_FUNCTION_CALL:
        out << "*" << token.get_word() << " args:";
    case lpp::token_t::TOK_LIST:
        {
            out << "[";
            auto const max(token.get_list_size());
            if(max > 0)
            {
                out << *token.get_list_item(0);
                for(std::remove_const<decltype(max)>::type idx(1); idx < max; ++idx)
                {
                    out << " ";
                    out << *token.get_list_item(idx);
                }
            }
            out << "]";
        }
        break;

    case lpp::token_t::TOK_QUOTED:
        out << '"' << token.get_word();
        break;

    case lpp::token_t::TOK_THING:
        out << ':' << token.get_word();
        break;

    case lpp::token_t::TOK_WORD:
        out << token.get_word();
        break;

    default:
        out << lpp::to_string(token.get_token());
        break;

    }

    return out;
}



// vim: ts=4 sw=4 et nocindent
