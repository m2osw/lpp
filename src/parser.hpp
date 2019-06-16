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

#include <fstream>



namespace lpp
{


class Parser
{
public:
    typedef std::shared_ptr<Parser>     pointer_t;

                            Parser();

    void                    set_output_object(bool status);
    void                    set_trace(bool status);
    void                    add_lexer(Lexer::pointer_t lexer);

    void                    parse();
    void                    optimize();
    void                    generate();

private:
    struct control_t
    {
        Token::pointer_t            m_function_call;
        std::string const &         m_result_var;
        Token::vector_t::size_type  m_max_args;
        Token::pointer_t            m_declaration;
        procedure_flag_t const      m_procedure_flags;
    };

    void                    parse_declarations();
    void                    parse_procedures();
    void                    parse_program();
    Token::pointer_t        parse_body(Token::pointer_t body);
    Token::pointer_t        parse_expression(Token::pointer_t expr);
    Token::pointer_t        next_lexer_token();
    Token::pointer_t        next_body_token();
    void                    input_lines();
    void                    line();
    void                    procedure(Token::pointer_t keyword);
    void                    program();
    Token::pointer_t        call_function(bool must_return);
    Token::pointer_t        expression();
    Token::pointer_t        equality_expression();
    Token::pointer_t        relational_expression();
    Token::pointer_t        additive_expression();
    Token::pointer_t        multiplicative_expression();
    Token::pointer_t        unary_expression();
    void                    output_body(Token::pointer_t body);
    void                    output_function_call(Token::pointer_t function_call, std::string const & result_var = std::string());
    void                    output_argument(Token::pointer_t arg, std::string const & value_name);
    void                    build_list(Token::pointer_t list);
    void                    control(control_t & control_info);
    void                    control_repeat(control_t & control_info);
    std::string             logo_to_cpp_name(std::string const & name);
    std::string             word_to_cpp_literal_string(std::string const & word);
    std::string             get_unique_name();

    bool                    f_output_object = false;
    bool                    f_enable_trace = false;
    bool                    f_rt_main = false;
    std::uint_fast32_t      f_body_pos = 0;
    Token::pointer_t        f_body = Token::pointer_t();
    Token::pointer_t        f_function = Token::pointer_t();
    std::uint_fast64_t      f_unique = 0;
    std::uint_fast16_t      f_lexer_pos = 0;
    std::uint_fast32_t      f_parsing_list = 0;
    Lexer::vector_t         f_lexer = Lexer::vector_t();
    Token::pointer_t        f_current_token = Token::pointer_t();   // last f_lexer.next_token() / next_body_token();
    Token::pointer_t        f_program = Token::pointer_t();         // PROGRAM ...
    Token::pointer_t        f_declarations = Token::pointer_t();    // DECLARE/PRIMITIVE ...
    Token::pointer_t        f_procedures = Token::pointer_t();      // TO ...
    std::ofstream           f_out = std::ofstream();
};



} // lpp namespace

// vim: ts=4 sw=4 et nocindent
