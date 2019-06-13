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

#include "lexer.hpp"

#include "exception.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

namespace lpp
{



Lexer::Lexer(std::string const & filename)
    : f_filename(filename)
{
    if(filename == "-")
    {
        f_in = &std::cin;
    }
    else
    {
        std::ifstream * file(new std::ifstream);
        file->open(filename);
        if(!file->is_open())
        {
            delete file;
            throw lpp_error("could not open file \""
                          + filename
                          + "\".");
        }
        f_file = file;
        f_in = file;
    }
}


Lexer::Lexer(std::string const & filename, std::string const & primitive)
    : f_filename(filename)
{
    std::stringstream * file = new std::stringstream;
    *file << primitive;
    file->seekg(0);

    f_file = file;
    f_in = file;
    f_line = 1;
}


Lexer::~Lexer()
{
    if(f_file != nullptr)
    {
        delete f_file;
    }
}


void Lexer::ungetc(char_type c)
{
    if(f_unget_pos >= sizeof(f_unget_chars) / sizeof(f_unget_chars[0]))
    {
        throw std::logic_error("Lexer::ungetc() called when the buffer is already full.");
    }

    f_unget_chars[f_unget_pos] = c;
    ++f_unget_pos;
}


Lexer::char_type Lexer::getc()
{
    for(;;)
    {
        if(f_unget_pos > 0)
        {
            --f_unget_pos;
            return f_unget_chars[f_unget_pos];
        }

        auto result(f_in->get());
        if(result == '\r')
        {
            result = f_in->get();
            if(result != '\n')
            {
                ungetc(result);
                result = '\n';
            }
        }
        if(result == '\n')
        {
            ++f_line;
            auto const line_continuation(f_in->get());
            if(line_continuation != '~')
            {
                ungetc(line_continuation);
                return static_cast<char_type>(result);
            }
        }
        else
        {
            return static_cast<char_type>(result);
        }
    }
}


Token::pointer_t Lexer::next_token()
{
    auto get_word = [&](char_type c = '\0')
        {
            std::string word;
            if(c == '\0')
            {
                c = getc();
            }
            for(;;)
            {
                if(c == '|')
                {
                    for(;;)
                    {
                        c = getc();
                        if(c == std::istream::traits_type::eof())
                        {
                            return word;
                        }
                        if(c == '|')
                        {
                            break;
                        }
                        if(c == '\\')
                        {
                            c = getc();
                            if(c == std::istream::traits_type::eof())
                            {
                                return word;
                            }
                        }
                        word += c;
                    }
                }
                else
                {
                    switch(c)
                    {
                    case std::istream::traits_type::eof():
                    case ' ':
                    case '\t':
                    case '\n':
                        // no need to unget spaces
                        return word;

                    case '+':       // TBD: quoted words are expected to include this character and following?!
                    case '-':
                    case '*':
                    case '/':
                    case '<':
                    case '>':
                    case '=':
                    case '(':
                    case ')':
                    case '[':
                    case ']':
                        ungetc(c);
                        return word;

                    }
                    if(c == '\\')
                    {
                        c = getc();
                        if(c == std::istream::traits_type::eof())
                        {
                            return word;
                        }
                    }
                    word += c;
                }

                c = getc();
            }
        };

    Token::pointer_t result(std::make_shared<Token>(token_t::TOK_EOF, f_filename, f_line));
    for(;;)
    {
        char_type c(getc());

        if(f_start_of_line
        && c == '#')
        {
            // skip comments ('#' has to be in column 1)
            // this allows for "#!/usr/bin/lpp" in your scripts
            do
            {
                c = getc();
            }
            while(c != std::istream::traits_type::eof() && c != '\n');
            continue;
        }

        bool const start_of_line(f_start_of_line); // for WORD
        f_start_of_line = false;
        switch(c)
        {
        case std::istream::traits_type::eof():
            result->set_token(token_t::TOK_EOF);
            return result;

        case ';':
            // skip comment
            do
            {
                c = getc();
            }
            while(c != std::istream::traits_type::eof() && c != '\n');
            break;

        case ' ':
        case '\t':
        case '\n':
            // skip white space
            f_start_of_line = true;
            break;

        case '+':
            c = getc();
            ungetc(c);
            if(c == ' '
            || c == '\t'
            || c == '\n')
            {
                result->set_token(token_t::TOK_SUM);
            }
            else
            {
                result->set_token(token_t::TOK_PLUS);
            }
            return result;

        case '-':
            c = getc();
            ungetc(c);
            if(c == ' '
            || c == '\t'
            || c == '\n')
            {
                result->set_token(token_t::TOK_DIFFERENCE);
            }
            else
            {
                result->set_token(token_t::TOK_MINUS);
            }
            return result;

        case '*':
            result->set_token(token_t::TOK_PRODUCT);
            return result;

        case '/':
            result->set_token(token_t::TOK_QUOTIENT);
            return result;

        case '=':
            result->set_token(token_t::TOK_EQUALP);
            return result;

        case '<':
            c = getc();
            if(c == '=')
            {
                result->set_token(token_t::TOK_LESSEQUALP);
            }
            else if(c == '>')
            {
                result->set_token(token_t::TOK_NOTEQUALP);
            }
            else
            {
                ungetc(c);
                result->set_token(token_t::TOK_LESSP);
            }
            return result;

        case '>':
            c = getc();
            if(c == '=')
            {
                result->set_token(token_t::TOK_GREATEREQUALP);
            }
            else
            {
                ungetc(c);
                result->set_token(token_t::TOK_GREATERP);
            }
            return result;

        case '(':
            result->set_token(token_t::TOK_OPEN_PARENTHESIS);
            return result;

        case ')':
            result->set_token(token_t::TOK_CLOSE_PARENTHESIS);
            return result;

        case '[':
            result->set_token(token_t::TOK_OPEN_LIST);
            return result;

        case ']':
            result->set_token(token_t::TOK_CLOSE_LIST);
            return result;

        case ':':
            {
                std::string word(get_word());
                if(!word.empty())
                {
                    result->set_word(token_t::TOK_THING, word);
                    return result;
                }
                result->error("dangling ':', expected a word following it.");
            }
            break;

        case '"':
            result->set_word(token_t::TOK_QUOTED, get_word());
            return result;

        case '?':
            result->set_word(token_t::TOK_WORD, "?");
            return result;

        default:
            {
                bool is_number(c >= '0' && c <= '9');
                if(c == '.')
                {
                    c = getc();
                    if(c >= '0' && c <= '9')
                    {
                        is_number = true;
                    }
                    ungetc(c);
                    c = '.';
                }
                if(is_number)
                {
                    std::string number(get_word(c));
                    if(number.find('.') == std::string::npos)
                    {
                        char * e(nullptr);
                        std::uint64_t const v(strtoull(number.c_str(), &e, 10));
                        // TODO: verify conversion
                        result->set_integer(v);
                    }
                    else
                    {
                        char *e(nullptr);
                        double const v(strtod(number.c_str(), &e));
                        // TODO: verify conversion
                        result->set_float(v);
                    }
                }
                else
                {
                    std::string const word(get_word(c));
                    if(word.length() > 1)
                    {
                        switch(word[0])
                        {
                        case 'd':
                            if(word == "declare")
                            {
                                result->set_token(token_t::TOK_DECLARE);
                                return result;
                            }
                            break;

                        case 'e':
                            if(word == "end")
                            {
                                result->set_token(token_t::TOK_END);
                                return result;
                            }
                            break;

                        case 'f':
                            if(word == "false")
                            {
                                result->set_boolean(false);
                                return result;
                            }
                            else if(word == "function")
                            {
                                result->set_token(token_t::TOK_FUNCTION);
                                return result;
                            }
                            break;

                        case 'p':
                            if(word == "primitive")
                            {
                                result->set_token(token_t::TOK_PRIMITIVE);
                                return result;
                            }
                            else if(word == "procedure")
                            {
                                result->set_token(token_t::TOK_PROCEDURE);
                                return result;
                            }
                            else if(word == "program")
                            {
                                result->set_token(token_t::TOK_PROGRAM);
                                return result;
                            }
                            break;

                        case 't':
                            if(word == "to")
                            {
                                result->set_token(token_t::TOK_TO);
                                return result;
                            }
                            else if(word == "true")
                            {
                                result->set_boolean(true);
                                return result;
                            }
                            break;

                        case 'v':
                            if(word == "void")
                            {
                                result->set_token(token_t::TOK_VOID);
                                return result;
                            }
                            break;

                        }
                    }
                    result->set_word(token_t::TOK_WORD, word, start_of_line);
                }
                return result;
            }

        }
    }
}



} // lpp namespace

// vim: ts=4 sw=4 et nocindent
