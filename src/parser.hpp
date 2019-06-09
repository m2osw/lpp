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

#include "lexer.hpp"

namespace lpp
{


class Parser
{
public:
                            Parser(Lexer & lexer);

    void                    parse();

private:
    Token::pointer_t        next_token();
    void                    input_lines();
    void                    line();
    void                    to_definition(bool declaration_only);
    Token::pointer_t        call_function();
    Token::pointer_t        expression();
    Token::pointer_t        equality_expression();
    Token::pointer_t        relational_expression();
    Token::pointer_t        additive_expression();
    Token::pointer_t        multiplicative_expression();
    Token::pointer_t        unary_expression();

    Lexer &                 f_lexer;
    Token::pointer_t        f_current_token = Token::pointer_t();   // last f_lexer.next_token()
    Token::pointer_t        f_program = Token::pointer_t();         // as-is function calls
    Token::pointer_t        f_declarations = Token::pointer_t();    // DECLARE ...
    Token::pointer_t        f_procedures = Token::pointer_t();      // TO ...
};



} // lpp namespace

// vim: ts=4 sw=4 et nocindent
