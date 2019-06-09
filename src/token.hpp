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
    TOK_DECLARE,                    // "DECLARE" word
    TOK_DIFFERENCE,                 // -
    TOK_END,                        // "END" word
    TOK_EQUALP,                     // =
    TOK_FLOAT,                      // \d+\.\d+ or \d+[eE][-+]\d+
    TOK_GREATEREQUALP,              // >=
    TOK_GREATERP,                   // >
    TOK_INTEGER,                    // \d+
    TOK_LESSEQUALP,                 // <=
    TOK_LESSP,                      // <
    TOK_NOTEQUALP,                  // <>
    TOK_OPEN_LIST,                  // [
    TOK_OPEN_PARENTHESIS,           // (
    TOK_PRODUCT,                    // *
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


class Token
{
public:
    typedef std::shared_ptr<Token>              pointer_t;
    typedef std::vector<pointer_t>              vector_t;
    typedef std::map<std::string, pointer_t>    map_t;
    typedef std::uint32_t                       argument_count_t;

                        Token(token_t token = token_t::TOK_EOF);

    void                set_token(token_t token);
    void                set_word(token_t token, std::string const & word, bool start_of_line = false);
    void                set_integer(std::int64_t value);
    void                set_float(double value);
    void                set_boolean(bool value);
    void                set_function_limits(argument_count_t min_param, argument_count_t def_param, argument_count_t max_param);
    void                add_list_item(pointer_t item);
    void                add_map_item(std::string const & name, pointer_t item);

    token_t             get_token() const;
    bool                get_start_of_line() const;
    std::string const & get_word() const;
    std::int64_t        get_integer() const;
    double              get_float() const;
    bool                get_boolean() const;
    argument_count_t    get_min_args() const;
    argument_count_t    get_def_args() const;
    argument_count_t    get_max_args() const;
    vector_t::size_type get_list_size() const;
    pointer_t           get_list_item(vector_t::size_type idx) const;
    pointer_t           get_map_item(std::string const & name) const;

private:
    token_t             f_token = token_t::TOK_EOF;
    bool                f_start_of_line = false;
    bool                f_boolean = false;
    std::string         f_word = std::string();
    std::int64_t        f_integer = 0;
    double              f_float = 0.0;
    vector_t            f_list = vector_t();
    map_t               f_map = map_t();
    argument_count_t    f_min_args = 0;
    argument_count_t    f_def_args = 0;
    argument_count_t    f_max_args = 0;
};



} // lpp namespace

// vim: ts=4 sw=4 et nocindent
