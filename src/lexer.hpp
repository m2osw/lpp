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

#pragma once

#include "token.hpp"

namespace lpp
{


class Lexer
{
public:
    typedef int         char_type;

                        Lexer(std::istream & in);

    Token::pointer_t    next_token();

private:
    void                ungetc(char_type c);
    char_type           getc();
    bool                isspace(char_type c) const;

    std::istream &      f_in;
    std::uint_fast32_t  f_line = 0;
    std::uint_fast8_t   f_unget_pos = 0;
    char_type           f_unget_chars[2];
    bool                f_start_of_line = true;
};



} // lpp namespace

// vim: ts=4 sw=4 et nocindent
