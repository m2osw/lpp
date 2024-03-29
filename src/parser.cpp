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

// common files
//
#include "utf8_iterator.hpp"

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


Parser::Parser()
    : f_program(std::make_shared<Token>(token_t::TOK_LIST))
    , f_declarations(std::make_shared<Token>(token_t::TOK_MAP))
    , f_procedures(std::make_shared<Token>(token_t::TOK_MAP))
{
    add_lexer(std::make_shared<Lexer>("primitives.logo",                                    // Current status
            // A
            "primitive [function] allopen end allopen\n"                                    // external
            "primitive [function logic] and :boolean1 :boolean2 [:rest] end\n"              // external
            "primitive [function arithmetic] arccos :number1 end\n"                         // external
            "primitive [function arithmetic] arcsin :number1 end\n"                         // external
            "primitive [function arithmetic] arctan :number1 [:number2] end\n"              // external
            "primitive [function] arity :name end arity\n"                                  // external
            "primitive [function arithmetic] ashift :number :shift end\n"                   // external
            // B
            "primitive [function] backslashedp&backslashed? :char end\n"                    // external
            "primitive [function] beforep&before? :word1 :word2 end\n"                      // external
            "primitive [function arithmetic] bitand :number1 :number2 [:rest] end\n"        // external
            "primitive [function arithmetic] bitnot :number end\n"                          // external
            "primitive [function arithmetic] bitor :number1 :number2 [:rest] end\n"         // external
            "primitive [function arithmetic] bitxor :number1 :number2 [:rest] end\n"        // external
            "primitive [function] butfirst&bf :thing end\n"                                 // external
            "primitive [function] butlast&bl :thing end\n"                                  // external
            "primitive [procedure] bye [:code 0] end\n"                                     // external
            "primitive [function] byte :word end byte\n"                                    // external
            // C
            "primitive [procedure control inline] call :name :arguments end call\n"         // inline
            "primitive [function control inline] callfunction :name :arguments end callfunction\n"
                                                                                            // inline
            "primitive [procedure control inline] case :value :clauses end case\n"          // MISSING
            "primitive [procedure control inline] catch :tag :instructions end catch\n"     // inline
            "primitive [function] char :number end char\n"                                  // external
            "primitive [procedure] cleartext&ct end cleartext\n"                            // external
            "primitive [procedure] close :filename [:force true] end close\n"               // external
            "primitive [procedure] closeall end closeall\n"                                 // external
            "primitive [function] combine :thing1 :thing2 end\n"                            // external
            "primitive [function arithmetic] comparablep&comparable? :thing1 :thing2 end\n" // external
            "primitive [procedure control inline] cond :clauses end cond\n"                 // MISSING
            "primitive [function arithmetic] cos :number1 end\n"                            // external
            "primitive [function] count :thing end\n"                                       // external
            "primitive [function] cursor end\n"                                             // external
            // D
            "primitive [function] definedp&defined? :name end\n"                            // external
            "primitive [function arithmetic] difference :number1 :number2 [:rest] end\n"    // external
            "primitive [function] dequeue :queue end\n"                                     // external
            "primitive [procedure control inline] do.until :boolean :once_and_if_false end\n"   // inline
            "primitive [procedure control inline] do.while :boolean :once_and_if_true end\n"    // inline
            // E
            "primitive [function] emptyp&empty? :thing end\n"                               // external
            "primitive [function] equalp&equal? :thing1 :thing2 [:rest] end\n"              // external
            "primitive [function] eofp&eof? :filename end eofp\n"                           // external
            "primitive [procedure] erase&er :names end erase\n"                             // external
            "primitive [procedure] erasefile&erf :filename end erasefile\n"                 // external
            "primitive [function] error end\n"                                              // external
            "primitive [function] errorp&error? end\n"                                      // external
            // F
            "primitive [function] fileinfo :filename end\n"                                 // external
            "primitive [function] filep&file? :filename end\n"                              // external
            "primitive [function] first :thing end\n"                                       // external
            "primitive [function] firstbyte :word end firstbyte\n"                          // external
            "primitive [function] floatp&float? :thing end\n"                               // external
            "primitive [procedure control inline] for :control :instructions end\n"         // inline
            "primitive [procedure control inline] forever :instructions end\n"              // inline
            "primitive [function] fput :item :thing end\n"                                  // external
            // G
            "primitive [function] gensym end\n"                                             // external
            "primitive [procedure control inline] goto :tag end\n"                          // inline
            "primitive [function] gprop :plistname :propname end gprop\n"                   // external
            "primitive [function] greaterequalp&greaterequal? :thing1 :thing2 [:rest] end\n"    // external
            "primitive [function] greaterp&greater? :thing1 :thing2 [:rest] end\n"          // external
            // H
            // I
            "primitive [procedure control inline] if :boolean :if_true [:if_false void] 3 end\n"    // inline
            "primitive [procedure control inline] ifelse :boolean :if_true :if_false end\n" // inline
            "primitive [procedure control inline] iffalse&iff :if_false end\n"              // inline
            "primitive [procedure control inline] iftrue&ift :if_true end\n"                // inline
            "primitive [procedure] ignore :thing [:rest] end\n"                             // inline (special)
            "primitive [function] int :number end\n"                                        // external
            "primitive [function] integerp&integer? :thing end\n"                           // external
            "primitive [function] item :number :thing end\n"                                // external
            // J
            // K
            "primitive [function] keyp&key? end\n"                                          // MISSING
            // L
            "primitive [function] last :thing end last\n"                                   // external
            "primitive [function] lastbyte :word end lastbyte\n"                            // external
            "primitive [function] lessequalp&lessequal? :thing1 :thing2 [:rest] end\n"      // external
            "primitive [function] lessp&less? :thing1 :thing2 [:rest] end\n"                // external
            "primitive [function] list :thing [:rest] 2 end\n"                              // external
            "primitive [function] listp&list? :thing end\n"                                 // external
            "primitive [procedure] local :name [:rest] end\n"                               // external
            "primitive [procedure] localmake :name :thing end\n"                            // external
            "primitive [function] lowercase :word end lowercase\n"                          // external
            "primitive [function] lput :item :thing end lput\n"                             // external
            "primitive [function arithmetic] lshift :number :shift end lshift\n"            // external
            // M
            "primitive [procedure] make :name :thing end\n"                                 // external
            "primitive [function] mditem :positions :list end\n"                            // external
            "primitive [function] mdsetitem :positions :list :value end\n"                  // external
            "primitive [function] member :thing1 :thing2 end\n"                             // external
            "primitive [function] memberp&member? :thing1 :thing2 end\n"                    // external
            "primitive [function arithmetic] minus :number end\n"                           // external
            "primitive [function arithmetic] modulo :number1 :number2 end\n"                // external
            // N
            "primitive [procedure] name :thing :name end\n"                                 // external
            "primitive [function] namedp&named? :name end\n"                                // external
            "primitive [function] names end\n"                                              // external
            "primitive [function arithmetic] nanp&nan? :number end\n"                       // external
            "primitive [function] not :boolean end\n"                                       // external
            "primitive [function] notequalp&notequal? :thing1 :thing2 [:rest] end\n"        // external
            "primitive [procedure] notify :variable :procedure end\n"                       // external
            "primitive [function] numberp&number? :thing end\n"                             // external
            // O
            "primitive [procedure] openappend :filename end\n"                              // external
            "primitive [procedure] openread :filename end\n"                                // external
            "primitive [procedure] openupdate :filename end\n"                              // external
            "primitive [procedure] openwrite :filename end\n"                               // external
            "primitive [function logic] or :boolean1 :boolean2 [:rest] end\n"               // external
            "primitive [procedure control inline] output&op :thing end\n"                   // inline
            // P
            "primitive [function] pick :thing end\n"                                        // external
            "primitive [function] plist :plistname end plist\n"                             // external
            "primitive [function] plistp&plist? :plistname end plistp\n"                    // external
            "primitive [function] plists end plists\n"                                      // external
            "primitive [function arithmetic] plus :number end\n"                            // external
            "primitive [function] pop :stack end\n"                                         // external
            "primitive [procedure] pprop :plistname :propname :value end pprop\n"           // external
            "primitive [function] prefix end\n"                                             // external
            "primitive [function] primitivep&primitive? :name end\n"                        // external
            "primitive [function] primitives end\n"                                         // external
            "primitive [procedure] print&pr :thing [:rest] end\n"                           // external
            "primitive [function] procedurep&procedure? :name end\n"                        // external
            "primitive [function] procedures end\n"                                         // external
            "primitive [function arithmetic] product :number1 :number2 [:rest] end\n"       // external
            "primitive [procedure] push :stack :thing end\n"                                // external
            // Q
            "primitive [procedure] queue :queue :thing end\n"                               // external
            "primitive [function] quoted :thing end quoted\n"                               // external
            "primitive [function arithmetic] quotient :number1 :number2 [:rest] end\n"      // external
            // R
            "primitive [function arithmetic] radarccos :number1 end\n"                      // external
            "primitive [function arithmetic] radarctan :number1 [:number2] end\n"           // external
            "primitive [function arithmetic] radarcsin :number1 end\n"                      // external
            "primitive [function arithmetic] radcos :number1 end\n"                         // external
            "primitive [function arithmetic] radsin :number1 end\n"                         // external
            "primitive [function arithmetic] radtan :number1 end\n"                         // external
            "primitive [function] random :number [:rest] end\n"                             // external
            "primitive [function] readbyte [:filename \"] end readbyte\n"                   // external
            "primitive [function] readbytes :number [:filename \"] end readbytes\n"         // external
            "primitive [function] readchar&rc [:filename \"] end readchar\n"                // external
            "primitive [function] readchars&rcs :number [:filename \"] end readchars\n"     // external
            "primitive [function] reader end reader\n"                                      // external
            "primitive [function] readlist&rl [:filename \"] end readlist\n"                // external
            "primitive [function] readpos [:filename \"] end readpos\n"                     // external
            "primitive [function] readrawline [:filename \"] end readrawline\n"             // external
            "primitive [function] readword&rw end\n"                                        // external
            "primitive [function arithmetic] remainder :number1 :number2 end\n"             // external
            "primitive [function] remdup :list end remdup\n"                                // external
            "primitive [function] remove :thing :list end\n"                                // external
            "primitive [procedure] remprop :plistname :propname end remprop\n"              // external
            "primitive [function] repcount end\n"                                           // external
            "primitive [procedure control inline] repeat :number :instructions end\n"       // inline
            "primitive [function] rerandom [:rest] end\n"                                   // external
            "primitive [function] reverse :thing end\n"                                     // external
            // S
            "primitive [function] sentence&se :thing [:rest] 2 end\n"                       // external
            "primitive [procedure] setcursor :position end\n"                               // external
            "primitive [function] setitem :position :list :value end\n"                     // external
            "primitive [procedure] setprefix :prefix end setprefix\n"                       // external
            "primitive [procedure] setread :filename end setread\n"                         // external
            "primitive [procedure] setreadpos :number [:filename \"] end setreadpos\n"      // external
            "primitive [procedure] settextcolor :foreground :background end\n"              // external
            "primitive [procedure] setwrite :filename end setwrite\n"                       // external
            "primitive [procedure] setwritepos :number [:filename \"] end setwritepos\n"    // external
            "primitive [function] shell :command [:wordflag] end\n"                         // external
            "primitive [procedure] show :thing [:rest] end\n"                               // external
            "primitive [function arithmetic] sin :number1 end\n"                            // external
            "primitive [function arithmetic] sqrt :number end\n"                            // external
            "primitive [procedure] standout :thing [:rest] end\n"                           // external
            "primitive [procedure control inline] stop end\n"                               // inline
            "primitive [function] substringp&substring? :thing1 :thing2 end\n"              // external
            "primitive [function arithmetic] sum :number1 :number2 [:rest] end\n"           // external
            // T
            "primitive [procedure control inline] tag :tag end\n"                           // inline
            "primitive [function arithmetic] tan :number1 end\n"                            // external
            "primitive [procedure] test :boolean end\n"                                     // external
            "primitive [function inline] thing :name end\n"                                 // inline
            "primitive [procedure control inline] throw :tag [:value void] 2 end\n"         // inline
            "primitive [function] time end\n"                                               // external
            "primitive [procedure] trace :list end\n"                                       // external
            "primitive [function] tracedp&traced? :list end\n"                              // external
            "primitive [function] ttyp&tty? end ttyp\n"                                     // external
            "primitive [procedure] type :thing [:rest] end\n"                               // external
            // U
            "primitive [function] unicode :word end unicode\n"                              // external
            "primitive [procedure] unnotify :variable [:procedure \"] 2 end unnotify\n"     // external
            "primitive [function arithmetic] unorderedp&unordered? :thing1 :thing2 end\n"   // external
            "primitive [procedure control inline] until :boolean :if_false end\n"           // inline
            "primitive [procedure] untrace :list end\n"                                     // external
            "primitive [function] uppercase :word end uppercase\n"                          // external
            // V
            // W
            "primitive [procedure inline] wait :seconds end wait\n"                         // inline
            "primitive [procedure control inline] while :boolean :if_true end while\n"      // inline
            "primitive [function] word :word1 :word2 [:rest] end word\n"                    // external
            "primitive [function] wordp&word? :thing end wordp\n"                           // external
            "primitive [procedure] write :filename :data end write\n"                       // external
            "primitive [function] writepos [:filename \"] end writepos\n"                   // external
            "primitive [function] writer end writer\n"                                      // external
            // X
            "primitive [function logic] xor :boolean1 :boolean2 [:rest] end xor\n"          // external
            // Y
            // Z

            // this is part of the run-time context
            //
            //"make \"caseignoredp true\n"
            //"make \"printdepthlimit 0\n"
            //"make \"printwidthlimit 0\n"
        ));
}


Parser::~Parser()
{
    if(!f_keep_l_cpp)
    {
        unlink("l.cpp");
    }
}


void Parser::set_output_object(bool status)
{
    f_output_object = status;
}


void Parser::set_trace(bool status)
{
    f_enable_trace = status;
}


void Parser::set_verbosity(bool status)
{
    f_verbose = status;
}


void Parser::set_keep_l_cpp(bool status)
{
    f_keep_l_cpp = status;
}


bool Parser::get_keep_l_cpp() const
{
    return f_keep_l_cpp;
}


void Parser::add_lexer(Lexer::pointer_t lexer)
{
    f_lexer.push_back(lexer);
}


/** \brief Parse the input file.
 *
 * This function reads the input file by repeatitively calling the
 * Lexer::next_token() function. It returns once the EOF is reached.
 * Note that the procedure body and program body get parsed only
 * in a second pass. This is important since the full compilation
 * requires that we know of all the procedures and declarations.
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
 *     program
 *   | procedure
 *   | declaration
 *
 * program:
 *     PROGRAM list_of_lines END
 *
 * procedure:
 *     to_proc_func procedure_flags WORD thing_list optional_thing_list rest_thing
 *                 default_number_of_inputs list_of_lines END procedure_name
 *
 * to_proc_func:
 *     TO
 *   | PROCEDURE
 *   | FUNCTION
 *
 * declaration:
 *     declare_primitive procedure_flags WORD thing_list optional_thing_list rest_thing
 *                               default_number_of_inputs END procedure_name
 *
 * declare_primitive:
 *     DECLARE
 *     PRIMITIVE
 *
 * procedure_flags:
 *     <empty>
 *     OPEN_LIST flag_list CLOSE_LIST
 *
 * flag_list:
 *     WORD
 *   | procedure_flags WORD
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
 *   | PLUS unary_expression
 *   | MINUS unary_expression
 *   | COUNT unary_expression
 *   | OPEN_PARENTHESIS expression CLOSE_PARENTHESIS
 *   | OPEN_LIST expression_list CLOSE_LIST
 *   | call_function
 * \endcode
 */
void Parser::parse()    // => "start: ..."
{
    if(f_lexer.size() <= 1)
    {
        throw lpp_error("at least one input file is required.");
    }

    // read all the data and especially do not actually compile the
    // programs and procedures since we need a complete list of all
    // the procedures and declarations before we can do so
    //
    input_lines();

    // now we can go ahead and verify the contents of the procedures
    // and program if one is defined
    //
    parse_declarations();
    parse_procedures();
    parse_program();
}


Token::pointer_t Parser::next_lexer_token()
{
    if(f_body != nullptr)
    {
        throw std::logic_error("f_body is not nullptr anymore, next_lexer_token() cannot be called anymore.");
    }

    for(;;)
    {
        if(f_lexer_pos >= f_lexer.size())
        {
            f_current_token = std::make_shared<Token>(token_t::TOK_EOF);
            break;
        }

        f_current_token = f_lexer[f_lexer_pos]->next_token();
        if(f_current_token->get_token() != token_t::TOK_EOF)
        {
            break;
        }
        ++f_lexer_pos;
    }

    return f_current_token;
}


void Parser::input_lines()
{
    next_lexer_token();
    for(;;)
    {
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
    case token_t::TOK_DECLARE:
    case token_t::TOK_PRIMITIVE:
    case token_t::TOK_PROCEDURE:
    case token_t::TOK_FUNCTION:
        procedure(f_current_token);
        break;

    case token_t::TOK_PROGRAM:
        program();
        break;

    default:
        f_current_token->error("expected one of TO, DECLARE, PRIVIMITE, or PROGRAM. Got "
                             + to_string(f_current_token->get_token())
                             + " instead.");
        break;

    }
}


void Parser::procedure(Token::pointer_t keyword)
{
    bool declaration_only(false);
    bool primitive(false);
    bool marked_as_procedure(false);
    procedure_flag_t procedure_flags(0);

    switch(keyword->get_token())
    {
    case token_t::TOK_TO:
        procedure_flags |= PROCEDURE_FLAG_PROCEDURE;
        break;

    case token_t::TOK_PROCEDURE:
        marked_as_procedure = true;
        procedure_flags |= PROCEDURE_FLAG_PROCEDURE;
        break;

    case token_t::TOK_FUNCTION:
        procedure_flags |= PROCEDURE_FLAG_PROCEDURE | PROCEDURE_FLAG_FUNCTION;
        break;

    case token_t::TOK_PRIMITIVE:
        declaration_only = true;
        primitive = true;
        procedure_flags |= PROCEDURE_FLAG_PRIMITIVE;
        break;

    case token_t::TOK_DECLARE:
        declaration_only = true;
        procedure_flags |= PROCEDURE_FLAG_DECLARE;
        break;

    default:
        throw std::logic_error("procedure() called with an unsupported token");

    }

    // TO | PROCEDURE | FUNCTION | DECLARE | PRIMITIVE [ flags ]
    //                                                   ^
    //
    next_lexer_token();
    if(f_current_token->get_token() == token_t::TOK_OPEN_LIST)
    {
        for(;;)
        {
            Token::pointer_t flag(next_lexer_token());

            switch(flag->get_token())
            {
            case token_t::TOK_CLOSE_LIST:
                break;

            case token_t::TOK_WORD:
                {
                    std::string const flag_name(flag->get_word());
                    if(flag_name.empty())
                    {
                        f_current_token->error(to_string(keyword->get_token())
                                             + " flag name cannot be the empty word.");
                        return;
                    }
                    switch(flag_name[0])
                    {
                    case 'a':
                        if(flag_name == "arithmetic")
                        {
                            procedure_flags |= PROCEDURE_FLAG_ARITHMETIC;
                        }
                        break;

                    case 'c':
                        if(flag_name == "c")
                        {
                            if((procedure_flags & PROCEDURE_FLAG_TYPE_MASK) != PROCEDURE_FLAG_PRIMITIVE
                            && (procedure_flags & PROCEDURE_FLAG_TYPE_MASK) != PROCEDURE_FLAG_C)
                            {
                                f_current_token->error("the \"c\" flag cannot be used with "
                                                     + to_string(keyword->get_token())
                                                     + ".");
                                return;
                            }
                            procedure_flags = (procedure_flags & ~PROCEDURE_FLAG_TYPE_MASK) | PROCEDURE_FLAG_C;
                        }
                        else if(flag_name == "control")
                        {
                            if((procedure_flags & PROCEDURE_FLAG_TYPE_MASK) != PROCEDURE_FLAG_PRIMITIVE)
                            {
                                f_current_token->error("the \"control\" flag can only be used with primitives.");
                                return;
                            }

                            procedure_flags |= PROCEDURE_FLAG_CONTROL | PROCEDURE_FLAG_INLINE;
                        }
                        break;

                    case 'i':
                        if(flag_name == "inline")
                        {
                            procedure_flags |= PROCEDURE_FLAG_INLINE;
                        }
                        break;

                    case 'l':
                        if(flag_name == "logic")
                        {
                            // logic is considered the same as arithmetic
                            // (we probably mean "inline expression"...)
                            //
                            procedure_flags |= PROCEDURE_FLAG_ARITHMETIC;
                        }
                        break;

                    }
                }
                continue;

            case token_t::TOK_FUNCTION:
                procedure_flags |= PROCEDURE_FLAG_FUNCTION;
                continue;

            case token_t::TOK_PROCEDURE:
                marked_as_procedure = true;
                continue;

            case token_t::TOK_EOF:
            case token_t::TOK_DECLARE:
            case token_t::TOK_PRIMITIVE:
            case token_t::TOK_TO:
            case token_t::TOK_END:
                f_current_token->error(to_string(keyword->get_token())
                                     + " declaration expects a ']' to end the list of procedure flags.");
                return;

            default:
                f_current_token->error("flags in a "
                                     + to_string(keyword->get_token())
                                     + " declaration must all be words.");
                return;

            }

            next_lexer_token();
            break;
        }
    }

    // TO [ flags ] WORD
    //              ^
    //
    Token::pointer_t name(f_current_token);
    if(name->get_token() != token_t::TOK_WORD)
    {
        if(name->get_token() != token_t::TOK_QUOTED) // accept `TO "name` as well
        {
            f_current_token->error(to_string(keyword->get_token())
                                 + " definition expects a WORD representing the procedure name as its first parameter.");
            // TODO: search the END token
            return;
        }
        name->set_token(token_t::TOK_WORD);
    }

    std::string long_name(name->get_word());
    if((procedure_flags & PROCEDURE_FLAG_TYPE_MASK) != PROCEDURE_FLAG_C)
    {
        // force lowercase on all procedure names
        // (unless it is a C function in which case we do not want to
        // do that)
        //
        for(auto & c : long_name)
        {
            if(c >= 'A' && c <= 'Z')
            {
                c |= 0x20;
            }
        }
    }
    std::string short_name;
    std::string::size_type const pos(long_name.find('&'));
    if(pos != std::string::npos)
    {
        long_name = name->get_word().substr(0, pos);
        short_name = name->get_word().substr(pos + 1);
    }

    if(marked_as_procedure
    && (procedure_flags & PROCEDURE_FLAG_FUNCTION) != 0)
    {
        f_current_token->error("procedure \""
                             + long_name
                             + "\" cannot be declared a PROCEDURE (use `STOP`) and FUNCTION (use `OUTPUT <expr>`) at the same time.");
        return;
    }

    // TO WORD thing_list
    //         ^
    //
    Token::pointer_t required_arguments(std::make_shared<Token>(token_t::TOK_LIST));

    argument_count_t min_args(0);
    Token::pointer_t arg;
    for(;;)
    {
        arg = next_lexer_token();
        if(arg->get_token() != token_t::TOK_THING)
        {
            if(arg->get_token() != token_t::TOK_WORD
            && arg->get_token() != token_t::TOK_QUOTED)
            {
                break;
            }

            // if the word is on the next line we do not want to take
            // it as a parameter name
            //
            if(arg->get_token() == token_t::TOK_WORD
            && arg->get_start_of_line())
            {
                break;
            }

            arg->set_token(token_t::TOK_THING);
        }
        required_arguments->add_list_item(arg);
        ++min_args;
    }

    argument_count_t max_args(min_args);
    argument_count_t def_args(min_args);

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
        arg = next_lexer_token();
        if(arg->get_token() != token_t::TOK_THING)
        {
            if(arg->get_token() != token_t::TOK_WORD
            && arg->get_token() != token_t::TOK_QUOTED)
            {
                f_current_token->error("Lists of optional parameters or the REST must start with a THING: [:VARNAME ...]");
                // TODO: parse up to END
                return;
            }
            arg->set_token(token_t::TOK_THING);
        }

        Token::pointer_t t(next_lexer_token());
        if(t->get_token() == token_t::TOK_CLOSE_LIST)
        {
            // TO WORD ... [ THING ]
            //                     ^
            //
            rest_argument = arg;
            max_args = std::numeric_limits<argument_count_t>::max();
            arg = next_lexer_token();
            break;
        }

        // otherwise it has to be an expression representing the default value
        //
        // TO WORD ... [ THING expr ]
        //                     ^
        //
        //Token::pointer_t expr(expression()); -- defaults can't be parsed now
        Token::pointer_t expr(std::make_shared<Token>(token_t::TOK_LIST, f_current_token));
        std::uint_fast32_t count(1);
        for(;;)
        {
            if(arg->get_token() == token_t::TOK_EOF
            || arg->get_token() == token_t::TOK_END
            || arg->get_token() == token_t::TOK_TO
            || arg->get_token() == token_t::TOK_DECLARE)
            {
                f_current_token->error("optional parameter default value must be an expression immediately followed by a ']'");
                // TODO: skip to END
                return;
            }

            if(t->get_token() == token_t::TOK_CLOSE_LIST)
            {
                --count;
                if(count == 0)
                {
                    // TO WORD thing_list [ THING expr ]
                    //                                 ^
                    //
                    break;
                }
                // closed a list defined in `expr`
            }
            else if(t->get_token() == token_t::TOK_OPEN_LIST)
            {
                if(count == std::numeric_limits<decltype(count)>::max())
                {
                    f_current_token->error("too many '[' in a default expression.");
                    return;
                }
                ++count;
            }
            expr->add_list_item(t);
            t = next_lexer_token();
        }

        Token::pointer_t default_arguments(std::make_shared<Token>(token_t::TOK_LIST));
        default_arguments->add_list_item(arg);
        default_arguments->add_list_item(expr);

        optional_arguments->add_list_item(default_arguments);
        ++max_args;


        arg = next_lexer_token();
    }

#if 0
std::cerr << to_string(keyword->get_token())
          << " [0x"
          << std::hex << procedure_flags << std::dec
          << "] "
          << *name
          << " (long: "
          << long_name
          << " & short: "
          << short_name
          << ") -- {REQUIRED: "
          << *required_arguments
          << "} {OPTIONAL: "
          << *optional_arguments
          << "} {REST: "
          << (rest_argument == nullptr ? std::string() : "[:" + rest_argument->get_word() + "]")
          << "} {min/def/max: "
          << min_args
          << "/"
          << def_args
          << "/"
          << max_args
          << "} ...\n";
#endif

    // TO WORD thing_list [ THING expr ] [ THING ] default_number_of_inputs
    //                                             ^
    //
    if(arg->get_token() == token_t::TOK_INTEGER)
    {
        def_args = arg->get_integer();
        if(def_args < min_args)
        {
            f_current_token->error("the default number of parameter (" + std::to_string(def_args)
                + ") cannot be smaller than the minimum number of parameters (" + std::to_string(min_args)
                + ").");
            return;
        }
        if(def_args > max_args)
        {
            f_current_token->error("the default number of parameter (" + std::to_string(def_args)
                + ") cannot be larger than the maximum number of parameters (" + std::to_string(max_args)
                + ").");
            return;
        }
        next_lexer_token();
    }

    Token::pointer_t declaration(std::make_shared<Token>(token_t::TOK_LIST));
    declaration->add_list_item(name);
    declaration->add_list_item(required_arguments);
    declaration->add_list_item(optional_arguments);
    if(rest_argument != nullptr)
    {
        declaration->add_list_item(rest_argument);
    }
    declaration->set_function_limits(min_args, def_args, max_args);
    declaration->set_procedure_flags(procedure_flags);

    // TO or DECLARE ... are added to the declaration list
    //
    f_declarations->add_map_item(long_name, declaration);
    if(!short_name.empty())
    {
        f_declarations->add_map_item(short_name, declaration);
    }

    if(declaration_only)
    {
        if(f_current_token->get_token() != token_t::TOK_END)
        {
            f_current_token->error("reached the end of "
                                 + to_string(keyword->get_token())
                                 + " \""
                                 + long_name
                                 + "\" without an END keyword.");
            return;
        }
    }
    else
    {

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

        f_procedures->add_map_item(long_name, declaration);
        if(!short_name.empty())
        {
            f_procedures->add_map_item(short_name, declaration);
        }

        bool stop_found(false);
        bool output_found(false);
        for(;;)
        {
            if(f_current_token->get_token() == token_t::TOK_EOF)
            {
                f_current_token->error("end of file found before end of procedure, END is missing.");
                return;
            }

            if(f_current_token->get_token() == token_t::TOK_END)
            {
                break;
            }

            if(f_current_token->get_token() == token_t::TOK_WORD)
            {
                auto const & w(f_current_token->get_word());
                if(w == "stop")
                {
                    stop_found = true;
                }
                else if(w == "output")
                {
                    output_found = true;
                }
            }

            body->add_list_item(f_current_token);
            next_lexer_token();
        }

        if(stop_found && output_found)
        {
            f_current_token->error("a procedure cannot use STOP and OUTPUT <expr> at the same time.");
            return;
        }

        // not useful, but if no OUTPUT <expr> was found we have a procedure
        // (opposed to a function)
        //if(!stop_found && !output_found)
        //{
        //    stop_found = true;
        //}

        if(output_found)
        {
            if(marked_as_procedure)
            {
                f_current_token->error("procedure \""
                                     + name->get_word()
                                     + "\" is declared as a PROCEDURE but it uses OUTPUT <expr>.");
                return;
            }

            declaration->add_procedure_flags(PROCEDURE_FLAG_FUNCTION);
        }
        else
        {
            if((declaration->get_procedure_flags() & PROCEDURE_FLAG_FUNCTION) != 0)
            {
                if(stop_found)
                {
                    // found an explicit STOP
                    //
                    f_current_token->error("a procedure cannot use STOP when marked as being a FUNCTION.");
                }
                else
                {
                    // no STOP and no OUTPUT <expr> found
                    //
                    f_current_token->error("a procedure cannot be marked as being a FUNCTION if it doesn't use OUTPUT <expr>.");
                }
                return;
            }
        }
    }

    // we allow the END keyword to be followed by a quoted name which
    // has to match the TO <name>
    //
    next_lexer_token();
    if(f_current_token->get_token() == token_t::TOK_WORD
    || f_current_token->get_token() == token_t::TOK_QUOTED)
    {
        if(f_current_token->get_word() != long_name)
        {
            f_current_token->error("found `to \"" + long_name
                + "` however we found `end \"" + f_current_token->get_word()
                + "`");
            return;
        }
        next_lexer_token();
    }
}


void Parser::program()
{
    if(f_program->get_list_size() > 0)
    {
        f_current_token->error("You can have at most one program in your logo files.");
        return;
    }
    if(f_output_object)
    {
        f_current_token->error("The --output-object (-c) command line option and the \"program\" command cannot be used together.");
        return;
    }

    // we cannot immediately parse the program, so we just save all the tokens
    // in a list which we will parse later.
    //
    for(;;)
    {
        next_lexer_token();
        switch(f_current_token->get_token())
        {
        case token_t::TOK_PRIMITIVE:
        case token_t::TOK_PROGRAM:
        case token_t::TOK_DECLARE:
        case token_t::TOK_TO:
            // program are instructions outside of a procedure
            //
            f_current_token->error("Unexpected token ("
                                 + to_string(f_current_token->get_token())
                                 + ") in a program.");
            return;

        case token_t::TOK_END:
            // we allow the END keyword to be followed by PROGRAM
            //
            if(f_program->get_list_size() == 0)
            {
                f_current_token->error("You cannot have an empty program. Please do not create a program or put at least one instruction.");
                return;
            }
            next_lexer_token();
            if(f_current_token->get_token() == token_t::TOK_PROGRAM)
            {
                next_lexer_token();
            }
            return;

        default:
            f_program->add_list_item(f_current_token);
            break;

        }
    }
}





//////////////// The parsing at this point comes from lists of tokens
//////////////// (opposed to an input file)


Token::pointer_t Parser::next_body_token()
{
    if(f_body == nullptr)
    {
        throw std::logic_error("f_body is still a nullptr, next_body_token() cannot be called yet.");
    }

    if(f_body_pos >= f_body->get_list_size())
    {
        f_current_token = std::make_shared<Token>(token_t::TOK_EOF);
    }
    else
    {
        f_current_token = f_body->get_list_item(f_body_pos);
        ++f_body_pos;
    }

    return f_current_token;
}


void Parser::parse_declarations()
{
    auto const & declaration(f_declarations->get_map());
    for(auto d : declaration)
    {
        if((d.second->get_procedure_flags() & PROCEDURE_FLAG_OPTIONAL_OPTION_PARSED) != 0)
        {
            continue;
        }
        d.second->add_procedure_flags(PROCEDURE_FLAG_OPTIONAL_OPTION_PARSED);

        // we want to verify all the optional arguments since
        // those expressions were just added to a list thus far
        //
        // note that this does not verify the existance of variables
        //
        auto const max(d.second->get_list_size());
        if(max < 3)
        {
            throw std::logic_error("The number of items in the declaration list has to be at least 3.");
        }

        Token::pointer_t optional_args(d.second->get_list_item(2));
        auto const max_optional(optional_args->get_list_size());
        for(std::remove_const<decltype(max_optional)>::type idx(0); idx < max_optional; ++idx)
        {
            Token::pointer_t arg(optional_args->get_list_item(idx));
            if(arg->get_list_size() != 2)
            {
                throw std::logic_error("The number of items in the optional list item has to be exactly 2.");
            }
            Token::pointer_t expr(arg->get_list_item(1));
            Token::pointer_t new_expr(parse_expression(expr));
            arg->set_list_item(1, new_expr);
        }
    }
}


void Parser::parse_procedures()
{
    auto const & procs(f_procedures->get_map());
    for(auto p : procs)
    {
        // procedures have a "declaration" like the declare and primitive
        // the procedure body is the last item in the list
        //
        // the default argument expressions were already checked by
        // the parse_declarations() so we do not have to do anything
        // more here
        //
        auto const max(p.second->get_list_size());
        if(max < 4)
        {
            throw std::logic_error("The number of items in the procedure list has to be at least 4.");
        }
        Token::pointer_t body(p.second->get_list_item(max - 1));
        Token::pointer_t new_body(parse_body(body));
        p.second->set_list_item(max - 1, new_body);
    }
}


void Parser::parse_program()
{
    // the program directly defines tokens
    //
    Token::pointer_t new_program(parse_body(f_program));
    f_program = new_program;
}


Token::pointer_t Parser::parse_body(Token::pointer_t body)
{
    f_body_pos = 0;
    f_body = body;

    Token::pointer_t new_body(std::make_shared<Token>(token_t::TOK_LIST));
    next_body_token();
    for(;;)
    {
        switch(f_current_token->get_token())
        {
        case token_t::TOK_EOF:
            return new_body;

        case token_t::TOK_WORD:
        case token_t::TOK_FUNCTION_CALL:
        case token_t::TOK_OPEN_PARENTHESIS:
            new_body->add_list_item(call_function(false));
            break;

        default:
            f_current_token->error("procedure or program body expected '(' or a WORD. Got "
                                 + to_string(f_current_token->get_token())
                                 + " instead.");
            return new_body;

        }
    }
}


Token::pointer_t Parser::parse_expression(Token::pointer_t expr)
{
    f_body_pos = 0;
    f_body = expr;

    next_body_token();
    Token::pointer_t result(expression());
    return result;
}


Token::pointer_t Parser::call_function(bool must_return)
{
    bool const parenthesis(f_current_token->get_token() == token_t::TOK_OPEN_PARENTHESIS);
    if(parenthesis)
    {
        next_body_token();
        if(f_current_token->get_token() != token_t::TOK_WORD)
        {
            f_current_token->error("expected a WORD token after '('.");
            return f_current_token;
        }
    }

    Token::pointer_t func_call(f_current_token);

    // now t is a WORD, we can search the procedure declaration to
    // make sure it is defined and if we did not have a parenthesis
    // we can use the number of arguments expected
    //
    Token::pointer_t declaration(f_declarations->get_map_item(func_call->get_word()));
    if(declaration == nullptr)
    {
        f_current_token->error("unknown procedure \"" + func_call->get_word() + "\"");
        return f_current_token;
    }

    if(f_parsing_list == 0)
    {
        procedure_flag_t const procedure_flags(declaration->get_procedure_flags());
        if((procedure_flags & PROCEDURE_FLAG_FUNCTION) == 0)
        {
            // this is a procedure (no output)
            //
            if(must_return)
            {
                f_current_token->error("procedure \"" + func_call->get_word() + "\" cannot be used inside an expression");
                return f_current_token;
            }
        }
        else
        {
            // this is a function (will output)
            //
            if(!must_return)
            {
                f_current_token->error("function \""
                                     + func_call->get_word()
                                     + "\" cannot be used outside of an expression (try `ignore "
                                     + func_call->get_word()
                                     + "` instead");
                return f_current_token;
            }
        }
    }

    argument_count_t min_args(declaration->get_min_args());
    argument_count_t def_args(declaration->get_def_args());
    argument_count_t max_args(declaration->get_max_args());

//std::cerr << "----- working on [" << func_call->get_word() << "] "
//          << min_args
//          << "/"
//          << def_args
//          << "/"
//          << max_args
//          << "\n";

    // change the WORD into a FUNCTION-CALL
    //
    if(func_call->get_token() != token_t::TOK_FUNCTION_CALL)
    {
        func_call->set_token(token_t::TOK_FUNCTION_CALL);
        func_call->set_declaration(declaration);
    }

    next_body_token();
    for(;;)
    {
        if(parenthesis)
        {
            if(f_current_token->get_token() == token_t::TOK_CLOSE_PARENTHESIS)
            {
                next_body_token();
                break;
            }

            if(f_current_token->get_token() == token_t::TOK_EOF)
            {
                f_current_token->error("missing closing parenthesis.");
                return f_current_token;
            }
        }
        else
        {
            // we stop in various circumstances

            // EOF found?
            //
            bool eoa(f_current_token->get_token() == token_t::TOK_EOF);
//std::cerr << "checking eoa -- " << (eoa ? "TOK_EOF" : "no EOF") << " -- count/max: " << func_call->get_list_size() << "/" << max_args << "\n";

            // we found the maximum number of arguments supported
            // anything else will create an error (i.e. use IGNORE ...)
            // if additional functions/literals follow
            //
            if(!eoa
            && func_call->get_list_size() >= max_args)
            {
//std::cerr << "   eoa -- max_args reached " << max_args << "\n";
                eoa = true;
            }

            // a function called within an expression uses at most def_args
            // unless you used parenthesis; so only the outmost procedure
            // receives many args, not inside functions
            //
            if(!eoa
            && must_return
            && func_call->get_list_size() >= def_args)
            {
//std::cerr << "   eoa -- def_args reached " << def_args << "\n";
                eoa = true;
            }

            // next token is a WORD which represents procedure?
            //
            if(!eoa
            && (f_current_token->get_token() == token_t::TOK_WORD
                    || f_current_token->get_token() == token_t::TOK_FUNCTION_CALL))
            {
                Token::pointer_t word_declaration(f_declarations->get_map_item(f_current_token->get_word()));
                if(word_declaration != nullptr
                && (word_declaration->get_procedure_flags() & PROCEDURE_FLAG_FUNCTION) == 0)
                {
//std::cerr << "   eoa -- WORD " << f_current_token->get_word() << " is procedure\n";
                    eoa = true;
                }
//else std::cerr << "   eoa -- WORD " << f_current_token->get_word() << " is function\n";
            }

            // next two tokens are '(' + WORD and the word is a procedure?
            //
            if(!eoa
            && f_current_token->get_token() == token_t::TOK_OPEN_PARENTHESIS
            && f_body_pos + 1 < f_body->get_list_size())
            {
                Token::pointer_t next_token(f_body->get_list_item(f_body_pos + 1));
                if(next_token->get_token() == token_t::TOK_WORD
                || next_token->get_token() == token_t::TOK_FUNCTION_CALL)
                {
                    Token::pointer_t word_declaration(f_declarations->get_map_item(next_token->get_word()));
                    if(word_declaration != nullptr
                    && (word_declaration->get_procedure_flags() & PROCEDURE_FLAG_FUNCTION) == 0)
                    {
//std::cerr << "   eoa -- (WORD " << next_token->get_word() << " ...) is procedure\n";
                        eoa = true;
                    }
//else std::cerr << "   eoa -- (WORD " << next_token->get_word() << " ...) is function\n";
                }
            }

            // an expression cannot start with a ')' or ']'
            //
            if(!eoa
            && (f_current_token->get_token() == token_t::TOK_CLOSE_PARENTHESIS
                    || f_current_token->get_token() == token_t::TOK_CLOSE_LIST))
            {
                eoa = true;
//std::cerr << "   eoa -- the ) and ] close things too\n";
            }

            if(eoa)
            {
                if(func_call->get_list_size() < min_args
                && f_parsing_list == 0)
                {
                    // if we are in a list, we do not generate an error just
                    // yet (maybe later as we generate the output and we
                    // find out that this function indeed needs to be
                    // compiled and needs more arguments...)
                    //
                    f_current_token->error("too few arguments to call \""
                                         + func_call->get_word()
                                         + "\"; the minimum number of required arguments is "
                                         + std::to_string(min_args)
                                         + ".");
                    return f_current_token;
                }
                break;
            }
        }

        if(func_call->get_list_size() >= max_args)
        {
            f_current_token->error("too many arguments to call \""
                                 + func_call->get_word()
                                 + "\"; the maximum number of accepted arguments is "
                                 + std::to_string(max_args)
                                 + ".");
            return f_current_token;
        }

        Token::pointer_t list(expression());
        func_call->add_list_item(list);
    }

    return func_call;
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
                next_body_token();
                Token::pointer_t rhs(relational_expression());

                Token::pointer_t op(std::make_shared<Token>(token_t::TOK_EOF, lhs));
                std::string const function_name(boost::to_lower_copy(to_string(token)));
                op->set_word(token_t::TOK_FUNCTION_CALL, function_name);
                op->set_declaration(f_declarations->get_map_item(function_name));
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
                next_body_token();
                Token::pointer_t rhs(additive_expression());

                Token::pointer_t op(std::make_shared<Token>(token_t::TOK_EOF, lhs));
                std::string const function_name(boost::to_lower_copy(to_string(token)));
                op->set_word(token_t::TOK_FUNCTION_CALL, function_name);
                op->set_declaration(f_declarations->get_map_item(function_name));
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
                next_body_token();
                Token::pointer_t rhs(multiplicative_expression());

                Token::pointer_t op(std::make_shared<Token>(token_t::TOK_EOF, lhs));
                std::string const function_name(boost::to_lower_copy(to_string(token)));
                op->set_word(token_t::TOK_FUNCTION_CALL, function_name);
                op->set_declaration(f_declarations->get_map_item(function_name));
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
                next_body_token();
                Token::pointer_t rhs(unary_expression());

                Token::pointer_t op(std::make_shared<Token>(token_t::TOK_EOF, lhs));
                std::string const function_name(boost::to_lower_copy(to_string(token)));
                op->set_word(token_t::TOK_FUNCTION_CALL, function_name);
                op->set_declaration(f_declarations->get_map_item(function_name));
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
    case token_t::TOK_THING:
    case token_t::TOK_VOID:
        {
            Token::pointer_t result(f_current_token);
            next_body_token();
            return result;
        }

    case token_t::TOK_PLUS:
        {
            next_body_token();
            Token::pointer_t rhs(unary_expression());

            Token::pointer_t op(std::make_shared<Token>(token_t::TOK_EOF, rhs));
            op->set_word(token_t::TOK_FUNCTION_CALL, "plus");
            op->set_declaration(f_declarations->get_map_item("plus"));
            op->add_list_item(rhs);
            return op;
        }
        break;

    case token_t::TOK_MINUS:
        {
            next_body_token();
            Token::pointer_t rhs(unary_expression());

            Token::pointer_t op(std::make_shared<Token>(token_t::TOK_EOF, rhs));
            op->set_word(token_t::TOK_FUNCTION_CALL, "minus");
            op->set_declaration(f_declarations->get_map_item("minus"));
            op->add_list_item(rhs);
            return op;
        }
        break;

    case token_t::TOK_COUNT:
        {
            next_body_token();
            Token::pointer_t rhs(unary_expression());

            Token::pointer_t op(std::make_shared<Token>(token_t::TOK_EOF, rhs));
            op->set_word(token_t::TOK_FUNCTION_CALL, "count");
            op->set_declaration(f_declarations->get_map_item("count"));
            op->add_list_item(rhs);
            return op;
        }
        break;

    case token_t::TOK_OPEN_PARENTHESIS:
        if(f_body_pos < f_body->get_list_size()
        && (f_body->get_list_item(f_body_pos)->get_token() == token_t::TOK_WORD
            || f_body->get_list_item(f_body_pos)->get_token() == token_t::TOK_FUNCTION_CALL))
        {
            // this is a ( <function-call> <many parameters> ) type of
            // expression
            //
            return call_function(true);
        }
        else
        {
            next_body_token();

            Token::pointer_t rhs(expression());

            if(f_current_token->get_token() != token_t::TOK_CLOSE_PARENTHESIS)
            {
                f_current_token->error("expression expected a closing parenthesis instead of a "
                                     + to_string(f_current_token->get_token())
                                     + ".");
            }
            else
            {
                next_body_token();
            }
            return rhs;
        }

    case token_t::TOK_OPEN_LIST:
        {
            ++f_parsing_list;
            Token::pointer_t list(std::make_shared<Token>(token_t::TOK_LIST, f_current_token));

            next_body_token();
            for(;;)
            {
                if(f_current_token->get_token() == token_t::TOK_CLOSE_LIST)
                {
                    next_body_token();
                    break;
                }
                list->add_list_item(expression());
            }
            --f_parsing_list;
            return list;
        }

    case token_t::TOK_WORD:
        {
            Token::pointer_t declaration(f_declarations->get_map_item(f_current_token->get_word()));
            if(declaration == nullptr)
            {
                // if the function we're dealing with is a control function
                // then this list may need to be parsed _properly_
                //
                Token::pointer_t result(f_current_token);
                next_body_token();
                return result;
            }
            return call_function(true);
        }

    case token_t::TOK_LIST:
        {
            Token::pointer_t result(f_current_token);
            next_body_token();
            return result;
        }

    case token_t::TOK_FUNCTION_CALL:
        {
            Token::pointer_t declaration(f_current_token->get_declaration());
            if(declaration != nullptr
            && (declaration->get_procedure_flags() & PROCEDURE_FLAG_FUNCTION) != 0)
            {
                return call_function(true);
            }
        }
        f_current_token->error("unexpected procedure call in ("
                             + to_string(f_current_token->get_token())
                             + ":"
                             + f_current_token->get_word()
                             + ") an expression");
        return std::make_shared<Token>(token_t::TOK_VOID);

    default:
        f_current_token->error("unexpected token ("
                             + to_string(f_current_token->get_token())
                             + ") for an expression");
        return std::make_shared<Token>(token_t::TOK_VOID);

    }
}




void Parser::optimize()
{
    // TODO: go through all the function calls in the procedures and program
    //       and optimize those that we can optimize (i.e. `SUM 3 7` becomes
    //       `10`)
    //
    //       note that at first this may not look useful for basic
    //       arithmetics since the C++ compiler will take care of
    //       such optimizations, but if you end up with an instruction
    //       such as `item 3 [a b c d e f]` you can reduce it to just `c`
    //       which is something the C++ compiler is not likely to do for us
}


void Parser::generate()
{
    class auto_close
    {
    public:
        auto_close(std::ofstream & stream)
            : f_stream(stream)
        {
        }

        ~auto_close()
        {
            f_stream.close();
        }

    private:
        std::ofstream &     f_stream;
    };

    auto_close out_stream(f_out);

    f_out.open("l.cpp");
    if(!f_out.is_open())
    {
        f_current_token->error("unable to open intermediate file \"l.cpp\".");
        return;
    }

    f_out << "// AUTO-GENERATED FILE\n"
          << "#include <lpp/lpp.hpp>\n";

    {
        f_out << "// Function Declarations\n";

        auto const & procedures(f_procedures->get_map());
        for(auto p : procedures)
        {
            std::string const cpp_name(logo_to_cpp_name(p.first));
            f_out << "void procedure_" << cpp_name << "(lpp::lpp__context::pointer_t);\n";
        }
    }

    {
        f_out << "// Function Registration\n"
                 "namespace\n"
                 "{\n"
                 "lpp::lpp__procedure_info_t const lpp__procedures[]{\n";

        auto const & procedures(f_procedures->get_map());
        for(auto p : procedures)
        {
            Token::pointer_t declaration(f_declarations->get_map_item(p.first));
            std::string const cpp_name(logo_to_cpp_name(p.first));
            f_out << "{\""
                  << cpp_name
                  << "\",procedure_"
                  << cpp_name
                  << ","
                  << declaration->get_min_args()
                  << "UL,"
                  << declaration->get_def_args()
                  << "UL,"
                  << declaration->get_max_args()
                  << "UL,"
                     "lpp::PROCEDURE_FLAG_PROCEDURE"
                  << ((declaration->get_procedure_flags() & PROCEDURE_FLAG_FUNCTION) != 0 ? "|lpp::PROCEDURE_FLAG_FUNCTION" : "")
                  << "},\n";
        }

        f_out << "};\n"
                 "lpp::lpp__auto_register_procedures lpp__procs(lpp__procedures,"
              << procedures.size()
              << ");\n"
                 "}\n";
    }

    {
        f_out << "// Function Definitions\n";

        auto const & procedures(f_procedures->get_map());
        for(auto p : procedures)
        {
            auto const max(p.second->get_list_size());

            f_function = p.second;

            std::string const cpp_name(logo_to_cpp_name(p.first));
            f_out << "void procedure_"
                  << cpp_name
                  << "(lpp::lpp__context::pointer_t context)\n"
                     "{\n";

            if(f_enable_trace)
            {
                f_out << "context->trace_procedure(lpp::trace_mode_t::TRACE_MODE_ENTER,lpp::lpp__value::pointer_t());\n";
            }

            output_body(p.second->get_list_item(max - 1));

            // a procedure must OUTPUT <expr> and never reached the END
            //
            // TODO: detect this problem at compile time (which is definitely
            //       doable...) and avoid this throw
            //
            Token::pointer_t declaration(f_declarations->get_map_item(p.first));
            procedure_flag_t const procedure_flags(declaration->get_procedure_flags());
            if((procedure_flags & PROCEDURE_FLAG_FUNCTION) != 0)
            {
                f_out << "context->end_of_function_reached();\n";
            }
            else if(f_enable_trace)
            {
                f_out << "context->trace_procedure(lpp::trace_mode_t::TRACE_MODE_EXIT,lpp::lpp__value::pointer_t());\n";
            }

            f_out << "}\n";
        }
    }

    f_has_program = f_program->get_list_size() > 0;
    if(f_has_program)
    {
        f_out << "// Program Definition\n";

        f_function.reset();

        f_out << "void lpp__startup(lpp::lpp__context::pointer_t context)\n"
                 "{\n";
        output_body(f_program);
        f_out << "}\n";
    }
}


bool Parser::has_program() const
{
    return f_has_program;
}


void Parser::output_body(Token::pointer_t body)
{
    auto const max(body->get_list_size());
    for(std::remove_const<decltype(max)>::type c(0); c < max; ++c)
    {
        // a body is a list of commands (function calls)
        //
        output_function_call(body->get_list_item(c));
    }
}


void Parser::output_function_call(Token::pointer_t function_call, std::string const & result_var)
{
    // a command may have a list of arguments following it
    //
    auto const max_args(function_call->get_list_size());
    Token::pointer_t declaration(function_call->get_declaration());
    if(declaration == nullptr)
    {
        throw std::logic_error("somehow a function_call has a declaration pointer set to nullptr.");
    }
    procedure_flag_t const procedure_flags(declaration->get_procedure_flags());

    // the declaration is built this way:
    //declaration->add_list_item(name);
    //declaration->add_list_item(required_arguments);
    //declaration->add_list_item(optional_arguments);
    //declaration->add_list_item(rest_argument); -- optional

    if((procedure_flags & PROCEDURE_FLAG_CONTROL) != 0)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
        control_t control_info = {
            .f_function_call    = function_call,
            .f_result_var       = result_var,
            .f_max_args         = max_args,
            .f_declaration      = declaration,
            .f_procedure_flags  = procedure_flags
        };
#pragma GCC diagnostic pop

        control_primitive(control_info);
    }
    else
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
        control_t control_info = {
            .f_function_call    = function_call,
            .f_result_var       = result_var,
            .f_max_args         = max_args,
            .f_declaration      = declaration,
            .f_procedure_flags  = procedure_flags
        };
#pragma GCC diagnostic pop

        if(!inline_primitive(control_info))
        {
            std::string const context_name(get_unique_name());

            f_out << "{\n"
                     "lpp::lpp__context::pointer_t "
                  << context_name
                  << "(std::make_shared<lpp::lpp__context>(\""
                  << function_call->get_filename()
                  << "\",\""
                  << logo_to_cpp_name(function_call->get_word())
                  << "\","
                  << function_call->get_line()
                  << ","
                  << ((procedure_flags & PROCEDURE_FLAG_TYPE_MASK) == PROCEDURE_FLAG_PRIMITIVE ? "true" : "false")
                  << "));\n";

            Token::pointer_t required_arguments(declaration->get_list_item(1));
            Token::pointer_t optional_arguments(declaration->get_list_item(2));
            Token::pointer_t rest_argument;
            if(declaration->get_list_size() >= 4)
            {
                if((procedure_flags & PROCEDURE_FLAG_TYPE_MASK) != PROCEDURE_FLAG_PROCEDURE
                || declaration->get_list_size() >= 5)
                {
                    rest_argument = declaration->get_list_item(3);

                    f_out << "lpp::lpp__value::vector_t rest;\n";
                    auto const reserve(max_args - required_arguments->get_list_size() - optional_arguments->get_list_size());
                    if(reserve > 0)
                    {
                        f_out << "rest.reserve("
                              << std::min(std::max(static_cast<decltype(reserve)>(0)
                                                 , reserve)
                                        , static_cast<decltype(reserve)>(20))
                              << ");\n";
                    }
                }
            }

            if(max_args < required_arguments->get_list_size())
            {
                f_current_token->error("too few arguments to call \""
                                     + function_call->get_word()
                                     + "\"; the minimum number of required arguments is "
                                     + std::to_string(required_arguments->get_list_size())
                                     + ".");
                return;
            }

            std::remove_const<decltype(max_args)>::type a(0);
            for(; a < max_args; ++a)
            {
                std::string const value_name(get_unique_name());
                Token::pointer_t arg(function_call->get_list_item(a));

                output_argument(arg, value_name);

                Token::pointer_t arg_name;
                if(a < required_arguments->get_list_size())
                {
                    arg_name = required_arguments->get_list_item(a);
                }
                else if(a - required_arguments->get_list_size() < optional_arguments->get_list_size())
                {
                    Token::pointer_t opt_arg(optional_arguments->get_list_item(a - required_arguments->get_list_size()));
                    arg_name = opt_arg->get_list_item(0);
                }
                // else -- accumulate in the rest list

                if(declaration->get_word() != "ignore")
                {
                    if(arg_name != nullptr)
                    {
//std::cerr << "    -> set param \"" << arg_name->get_word() << "\"\n";
                        f_out << context_name
                              << "->set_thing("
                              << word_to_cpp_string_literal(arg_name->get_word())
                              << ","
                              << value_name
                              << ",lpp::lpp__thing_type_t::LPP__THING_TYPE_CONTEXT);\n";
                    }
                    else
                    {
                        if(rest_argument == nullptr)
                        {
                            throw std::logic_error("while working on \""
                                                 + function_call->get_word()
                                                 + "\" item number "
                                                 + std::to_string(a)
                                                 + " we are somehow attempting to add a rest argument when rest_argument == nullptr (maybe a primitive declaration is missing?)");
                        }

                        // add to rest list
                        f_out << "rest.push_back("
                              << value_name
                              << ");\n";
                    }
                }
            }

            if(declaration->get_word() != "ignore")
            {
                f_out << context_name
                      << "->attach(context);\n";
            }

            decltype(max_args) max_opt(required_arguments->get_list_size() + optional_arguments->get_list_size());
            for(; a < max_opt; ++a)
            {
                Token::pointer_t opt_arg(optional_arguments->get_list_item(a - required_arguments->get_list_size()));
                Token::pointer_t arg_name(opt_arg->get_list_item(0));
                Token::pointer_t def_expr(opt_arg->get_list_item(1));

                std::string const value_name(get_unique_name());
                output_argument(opt_arg->get_list_item(1), value_name);

                if(declaration->get_word() != "ignore")
                {
                    f_out << context_name
                          << "->set_thing("
                          << word_to_cpp_string_literal(arg_name->get_word())
                          << ","
                          << value_name
                          << ",lpp::lpp__thing_type_t::LPP__THING_TYPE_CONTEXT);\n";
                }
            }

            if(declaration->get_word() != "ignore")
            {
                if(rest_argument != nullptr)
                {
                    f_out << context_name
                          << "->set_thing("
                          << word_to_cpp_string_literal(rest_argument->get_word())
                          << ",std::make_shared<lpp::lpp__value>(rest),lpp::lpp__thing_type_t::LPP__THING_TYPE_CONTEXT);\n";
                }

                switch(procedure_flags & PROCEDURE_FLAG_TYPE_MASK)
                {
                case PROCEDURE_FLAG_PRIMITIVE:
                    f_out << "primitive_";
                    break;

                case PROCEDURE_FLAG_PROCEDURE:
                case PROCEDURE_FLAG_DECLARE:
                    f_out << "procedure_";
                    break;

                case PROCEDURE_FLAG_C:  // C functions are called as is
                    break;

                }
//std::cerr << "    -> call func. \"" << declaration->get_word() << "\"\n";
                f_out << logo_to_cpp_name(declaration->get_word())
                      << "("
                      << context_name
                      << ");\n";

                if(!result_var.empty())
                {
                    f_out << result_var
                          << "="
                          << context_name
                          << "->get_returned_value();\n";
                }
            }

            f_out << "}\n";
        }
    }
}


void Parser::output_argument(Token::pointer_t arg, std::string const & value_name)
{
    switch(arg->get_token())
    {
    case token_t::TOK_FUNCTION_CALL:
        // the output of a function call will stack a parameter
        //
        f_out << "lpp::lpp__value::pointer_t "
              << value_name
              << ";\n";
        output_function_call(arg, value_name);
        break;

    case token_t::TOK_BOOLEAN:
        f_out << "lpp::lpp__value::pointer_t "
              << value_name
              << "(std::make_shared<lpp::lpp__value>("
              << (arg->get_boolean() ? "true" : "false")
              << "));\n";
        break;

    case token_t::TOK_INTEGER:
        f_out << "lpp::lpp__value::pointer_t "
              << value_name
              << "(std::make_shared<lpp::lpp__value>(static_cast<lpp::lpp__integer_t>("
              << arg->get_integer()
              << "LL)));\n";
        break;

    case token_t::TOK_FLOAT:
        f_out << "lpp::lpp__value::pointer_t "
              << value_name
              << "(std::make_shared<lpp::lpp__value>(";
        if(std::isnan(arg->get_float()))
        {
            f_out  << "std::numeric_limits<lpp::lpp__float_t>::quiet_NaN()";
        }
        else
        {
            f_out << arg->get_float();
        }
        f_out << "));\n";
        break;

    case token_t::TOK_WORD:
        f_out << "lpp::lpp__value::pointer_t "
              << value_name
              << "(std::make_shared<lpp::lpp__value>("
                 "std::string("
              << word_to_cpp_string_literal(arg->get_word())
              << ")"
                 ")"
                 ");\n";
        break;

    case token_t::TOK_QUOTED:
        f_out << "lpp::lpp__value::pointer_t "
              << value_name
              << "(std::make_shared<lpp::lpp__value>("
                 "std::string("
              << word_to_cpp_string_literal(arg->get_word())
              << ")"
                 ")"
                 ");\n";
        break;

    case token_t::TOK_THING:
        f_out << "lpp::lpp__value::pointer_t "
              << value_name
              << "(context->get_thing("
              << word_to_cpp_string_literal(arg->get_word())
              << ")->get_value());\n";
        break;

    case token_t::TOK_LIST:
        f_out << "lpp::lpp__value::pointer_t "
              << value_name
              << "(";
        build_list(arg);
        f_out << ");\n";
        break;

    default:
        arg->error("unexpected token type ("
                  + to_string(arg->get_token())
                  + ") in a list of arguments.");
        return;

    }
}



void Parser::build_list(Token::pointer_t list)
{
    f_out << "std::make_shared<lpp::lpp__value>(lpp::lpp__value::vector_t{\n";
    build_list_content(list);
    f_out << "})\n";
}


void Parser::build_list_content(Token::pointer_t list)
{
    auto const max(list->get_list_size());
    for(std::remove_const<decltype(max)>::type l(0); l < max; ++l)
    {
        if(l != 0)
        {
            f_out << ",";
        }
        build_list_item(list->get_list_item(l));
    }
}


void Parser::build_list_item(Token::pointer_t item)
{
    switch(item->get_token())
    {
    case token_t::TOK_BOOLEAN:
        f_out << "std::make_shared<lpp::lpp__value>("
              << item->get_boolean()
              << ")\n";
        break;

    case token_t::TOK_INTEGER:
        f_out << "std::make_shared<lpp::lpp__value>(static_cast<lpp::lpp__integer_t>("
              << item->get_integer()
              << "LL))\n";
        break;

    case token_t::TOK_FLOAT:
        f_out << "std::make_shared<lpp::lpp__value>("
              << item->get_float()
              << ")\n";
        break;

    case token_t::TOK_THING:
        f_out << "std::make_shared<lpp::lpp__value>(':' + std::string("
              << word_to_cpp_string_literal(item->get_word())
              << "))\n";
        break;

    case token_t::TOK_WORD:
    case token_t::TOK_QUOTED:
        f_out << "std::make_shared<lpp::lpp__value>(std::string("
              << word_to_cpp_string_literal(item->get_word())
              << "))\n";
        break;

    case token_t::TOK_FUNCTION_CALL:    // TODO: operators get transformed in this way...
        {
            f_out << "std::make_shared<lpp::lpp__value>(std::string("
                  << word_to_cpp_string_literal(item->get_word())
                  << "))\n";
            auto const fcmax(item->get_list_size());
            if(fcmax > 0)
            {
                // TODO: write the list items, not the list itself
                // (especially for the :rest parameter)
                //
                //for(std::remove_const<decltype(fcmax)>::type fc(0); fc < fcmax; ++fc)
                //{
                //    f_out << ",";
                //    build_list(item->get_list_item(fc));
                //}

                f_out << ",";
                build_list_content(item);
            }
        }
        break;

    case token_t::TOK_LIST:
        build_list(item);
        break;

    default:
        item->error("unexpected token type ("
                  + to_string(item->get_token())
                  + ") in a list literal.");
        return;

    }
}


std::string Parser::logo_to_cpp_name(std::string const & name)
{
    std::stringstream result;

    result << std::hex;
    for(utf8_iterator in(name); in != name.end(); ++in)
    {
        char32_t c(*in);
        if((c >= 'a' && c <= 'z')
        || (c >= 'A' && c <= 'Z')
        || (c >= '0' && c <= '9'))
        {
            result << static_cast<char>(c);
        }
        else
        {
            result << '_'
                   << static_cast<std::int32_t>(c);
        }
    }

    return result.str();
}


std::string Parser::word_to_cpp_string_literal(std::string const & word)
{
    std::stringstream result;

    result << '"';
    for(auto c : word)
    {
        switch(c)
        {
        case '\0':
            result << "\\0";
            break;

        case '\\':
            result << "\\\\";
            break;

        case '"':
            result << "\\\"";
            break;

        default:
            result << c;
            break;

        }
    }
    result << '"';

    return result.str();
}


std::string Parser::get_unique_name()
{
    std::stringstream ss;

    ss << "lpp__tmp"
       << f_unique;

    ++f_unique;

    return ss.str();
}


} // lpp namespace

// vim: ts=4 sw=4 et nocindent
