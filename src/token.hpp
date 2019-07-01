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

#include "exception.hpp"

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace lpp
{



// <var>  -> std::variant
// <var>$ -> std::string
// <var>% -> std::int64_t
// <var># -> double
// <var>@ -> list
// <var>! -> bool
// <var>& -> map (plist)

enum class token_t
{
    // Special tokens
    //
    TOK_EOF,                        // end of input
    TOK_VOID,                       // an "empty" token which is not exactly an empty list
    TOK_MAP,                        // a "list" with a name as index for procedures
    TOK_FUNCTION_CALL,              // a TOK_WORD which becomes a function call

    // Lexer tokens
    //
    TOK_BOOLEAN,                    // "TRUE" or "FALSE"
    TOK_CLOSE_LIST,                 // ]
    TOK_CLOSE_PARENTHESIS,          // )
    TOK_COUNT,                      // #
    TOK_DECLARE,                    // "DECLARE" word
    TOK_DIFFERENCE,                 // -
    TOK_END,                        // "END" word
    TOK_EQUALP,                     // =
    TOK_FLOAT,                      // \d+\.\d+ or \d+[eE][-+]\d+
    TOK_FUNCTION,                   // "FUNCTION" word
    TOK_GREATEREQUALP,              // >=
    TOK_GREATERP,                   // >
    TOK_INTEGER,                    // \d+
    TOK_LESSEQUALP,                 // <=
    TOK_LESSP,                      // <
    TOK_MINUS,                      // -<something> (opposed to -<space><something>)
    TOK_NOTEQUALP,                  // <>
    TOK_OPEN_LIST,                  // [
    TOK_OPEN_PARENTHESIS,           // (
    TOK_PLUS,                       // +<something> (opposed to +<space><something>)
    TOK_PRIMITIVE,                  // "PRIMITIVE" word
    TOK_PROCEDURE,                  // "PROCEDURE" word
    TOK_PRODUCT,                    // *
    TOK_PROGRAM,                    // "PROGRAM" word
    TOK_QUOTED,                     // "<letters>
    TOK_QUOTIENT,                   // /
    TOK_SUM,                        // +
    TOK_THING,                      // :<letters>
    TOK_TO,                         // "TO" word
    TOK_WORD,                       // <letters>

    // Conglomeration
    //
    TOK_LIST,                       // tokens defined between '[' and ']'
};


std::string     to_string(token_t token);

typedef std::uint_fast32_t          procedure_flag_t;

// type
//
constexpr procedure_flag_t          PROCEDURE_FLAG_TYPE_MASK    = 0x0000000F;
constexpr procedure_flag_t          PROCEDURE_FLAG_PROCEDURE    = 0x00000000;      // TO/PROCEDURE/FUNCTION ...
constexpr procedure_flag_t          PROCEDURE_FLAG_PRIMITIVE    = 0x00000001;      // PRIMITIVE ...
constexpr procedure_flag_t          PROCEDURE_FLAG_DECLARE      = 0x00000002;      // DECLARE ...
constexpr procedure_flag_t          PROCEDURE_FLAG_C            = 0x00000003;      // PRIMITIVE [C] ...

// flags
//
constexpr procedure_flag_t          PROCEDURE_FLAG_INLINE       = 0x00000010;
constexpr procedure_flag_t          PROCEDURE_FLAG_ARITHMETIC   = 0x00000020;
constexpr procedure_flag_t          PROCEDURE_FLAG_CONTROL      = 0x00000040;
constexpr procedure_flag_t          PROCEDURE_FLAG_FUNCTION     = 0x00000080;       // function uses OUTPUT <expr>

constexpr procedure_flag_t          PROCEDURE_FLAG_TRACE                  = 0x40000000;       // turn ON trace mode at compile time
constexpr procedure_flag_t          PROCEDURE_FLAG_OPTIONAL_OPTION_PARSED = 0x80000000;       // function uses OUTPUT <expr>


typedef std::int64_t                integer_t;
typedef double                      float_t;

typedef std::uint32_t               argument_count_t;


class Token
{
public:
    typedef std::shared_ptr<Token>              pointer_t;
    typedef std::vector<pointer_t>              vector_t;
    typedef std::map<std::string, pointer_t>    map_t;

                        Token(token_t token = token_t::TOK_EOF, std::string const & filename = std::string(), line_t line = 0);
                        Token(token_t token, Token::pointer_t location);

    void                set_token(token_t token);
    void                set_word(token_t token, std::string const & word, bool start_of_line = false);
    void                set_integer(integer_t value);
    void                set_float(float_t value);
    void                set_boolean(bool value);
    void                set_procedure_flags(procedure_flag_t flags);
    void                add_procedure_flags(procedure_flag_t flags);
    void                remove_procedure_flags(procedure_flag_t flags);
    void                set_function_limits(argument_count_t min_param, argument_count_t def_param, argument_count_t max_param);
    void                set_declaration(pointer_t declaration);
    void                add_list_item(pointer_t item);
    void                set_list_item(int idx, pointer_t item);
    void                add_map_item(std::string const & name, pointer_t item);

    token_t             get_token() const;
    bool                get_start_of_line() const;
    std::string         get_word() const;
    integer_t           get_integer() const;
    float_t             get_float() const;
    bool                get_boolean() const;
    procedure_flag_t    get_procedure_flags() const;
    argument_count_t    get_min_args() const;
    argument_count_t    get_def_args() const;
    argument_count_t    get_max_args() const;
    pointer_t           get_declaration() const;
    vector_t::size_type get_list_size() const;
    pointer_t           get_list_item(vector_t::size_type idx) const;
    map_t const &       get_map() const;
    pointer_t           get_map_item(std::string const & name) const;

    std::string const & get_filename() const;
    line_t              get_line() const;
    void                error(std::string const & message);

private:
    token_t             f_token = token_t::TOK_EOF;
    bool                f_start_of_line = false;
    bool                f_boolean = false;
    std::string         f_word = std::string();
    integer_t           f_integer = 0;
    float_t             f_float = 0.0;
    vector_t            f_list = vector_t();
    map_t               f_map = map_t();
    procedure_flag_t    f_procedure_flags = 0;
    argument_count_t    f_min_args = 0;
    argument_count_t    f_def_args = 0;
    argument_count_t    f_max_args = 0;
    pointer_t           f_declaration = pointer_t();
    std::string         f_filename = std::string();
    line_t              f_line = 0;
};


} // lpp namespace


std::ostream & operator << (std::ostream & out, lpp::Token const & token);


// vim: ts=4 sw=4 et nocindent
