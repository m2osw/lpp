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
#include "lpp.hpp"

// C++ lib
//
#include <iostream>


namespace lpp
{


namespace
{


typedef std::map<std::string, lpp__procedure_info_t const *>    map_t;

map_t                   g_procedures = map_t();


constexpr std::uint32_t     MAX_MAX_ARGS = 4294967295UL;

lpp__procedure_info_t g_primitives[]
{
    // A
    { "allopen", nullptr,          0UL, 0UL,          0UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "and",     primitive_and,    2UL, 2UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "arccos",  primitive_arccos, 1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "arcsin",  primitive_arcsin, 1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "arctan",  primitive_arctan, 1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "arity",   primitive_arity,  1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "ascii",   primitive_ascii,  1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "ashift",  primitive_ashift, 2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },

    // B
    { "backslashed?", primitive_backslashedp, 1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "backslashedp", primitive_backslashedp, 1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "before?",      primitive_beforep,      2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "beforep",      primitive_beforep,      2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "bitand",       primitive_bitand,       2UL, 2UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "bitnot",       primitive_bitnot,       1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "bitor",        primitive_bitor,        2UL, 2UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "bitxor",       primitive_bitxor,       2UL, 2UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "butfirst",     primitive_butfirst,     1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "butlast",      primitive_butlast,      1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "bye",          primitive_bye,          0UL, 0UL,          1UL, PROCEDURE_FLAG_PRIMITIVE },

    // C
//    "primitive [procedure control inline] case :value :clauses end\n"               // MISSING
    { "catch",        nullptr,                2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE },
    { "char",         primitive_char,         1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "cleartext",    primitive_cleartext,    0UL, 0UL,          0UL, PROCEDURE_FLAG_PRIMITIVE },
//    "primitive [procedure] close :filename end\n"                                   // MISSING
//    "primitive [procedure] closeall end\n"                                          // MISSING
    { "combine",      primitive_combine,      2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "comparable?",  primitive_comparablep,  2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "comparablep",  primitive_comparablep,  2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
//    "primitive [procedure control inline] cond :clauses end\n"                      // MISSING
    { "cos",          primitive_cos,          1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "count",        primitive_count,        1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE },
    { "cursor",       primitive_cursor,       0UL, 0UL,          0UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },

    // D
    { "defined?",     primitive_definedp,    1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "definedp",     primitive_definedp,    1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "difference",   primitive_difference,  2UL, 2UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "dequeue",      primitive_dequeue,     1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "do.until",     nullptr,               2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE },
    { "do.while",     nullptr,               2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE },

    // E
    { "empty?",       primitive_emptyp,      1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "emptyp",       primitive_emptyp,      1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "equal?",       primitive_equalp,      2UL, 2UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "equalp",       primitive_equalp,      2UL, 2UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
//    "primitive [function] eofp end\n"                                               // MISSING
//    "primitive [procedure] erasefile&erf :filename end\n"                           // MISSING
    { "error",        primitive_error,       0UL, 0UL,          0UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "error?",       primitive_errorp,      0UL, 0UL,          0UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "errorp",       primitive_errorp,      0UL, 0UL,          0UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },

    // F
//    "primitive [function] filep&file? :filename end\n"                              // MISSING
    { "first",        primitive_first,       1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "floatp",       primitive_floatp,      1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "for",          nullptr,               2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE },
    { "forever",      nullptr,               1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE },
    { "fput",         primitive_fput,        2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },

    // G
    { "gensym",        primitive_gensym,        0UL, 0UL,          0UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
//    "primitive [procedure] global :name [:rest] end\n"                              // MISSING
    { "goto",          nullptr,                 1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE },
//    "primitive [function] gprop :plistname :propname end\n"                         // MISSING
    { "greaterequal?", primitive_greaterequalp, 2UL, 2UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "greaterequalp", primitive_greaterequalp, 2UL, 2UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "greater?",      primitive_greaterp,      2UL, 2UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "greaterp",      primitive_greaterp,      2UL, 2UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },

    // I
    { "if",           nullptr,                2UL, 3UL,          3UL, PROCEDURE_FLAG_PRIMITIVE },
    { "ifelse",       nullptr,                2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE },
    { "iff",          nullptr,                1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE },
    { "iffalse",      nullptr,                1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE },
    { "ift",          nullptr,                1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE },
    { "iftrue",       nullptr,                1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE },
    { "ignore",       nullptr,                1UL, 1UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE },
    { "int",          primitive_int,          1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "integer?",     primitive_integerp,     1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "integerp",     primitive_integerp,     1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "item",         primitive_item,         2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },

    // K
//    "primitive [function] keyp&key? end\n"                                          // MISSING

    // L
    { "last",         primitive_last,         1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "lessequal?",   primitive_lessequalp,   2UL, 2UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "lessequalp",   primitive_lessequalp,   2UL, 2UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "less?",        primitive_lessp,        2UL, 2UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "lessp",        primitive_lessp,        2UL, 2UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "list",         primitive_list,         1UL, 2UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "list?",        primitive_listp,        1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "listp",        primitive_listp,        1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
//    "primitive [procedure] local :name [:rest] end\n"                               // MISSING
//    "primitive [procedure] localmake :name :value end\n"                            // MISSING
//    "primitive [function] lowercase :word end\n"                                    // MISSING
    { "lput",         primitive_lput,         2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "lshift",       primitive_lshift,       2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },

    // M
    { "make",         primitive_make,         2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE },
    { "mditem",       primitive_mditem,       2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "mdsetitem",    primitive_mdsetitem,    3UL, 3UL,          3UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
//    "primitive [function] member :thing1 :thing2 end\n"                             // MISSING
//    "primitive [function] memberp&member? :thing end\n"                             // MISSING
    { "minus",        primitive_minus,        1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "modulo",       primitive_modulo,       2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },

    // N
    { "name",         primitive_name,         2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE },
    { "name?",        primitive_namep,        1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "namep",        primitive_namep,        1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
//    "primitive [function] names end\n"                                              // MISSING
    { "nan?",         primitive_nanp,         1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "nanp",         primitive_nanp,         1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
//    "primitive [function] nodes end\n" // return free memory?                       // MISSING
    { "not",          primitive_not,          1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "notequal?",    primitive_notequalp,    2UL, 2UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "notequalp",    primitive_notequalp,    2UL, 2UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "number?",      primitive_numberp,      2UL, 2UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "numberp",      primitive_numberp,      1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },

    // O
//    "primitive [procedure] openappend :filename end\n"                              // MISSING
//    "primitive [procedure] openread :filename end\n"                                // MISSING
//    "primitive [procedure] openupdate :filename end\n"                              // MISSING
//    "primitive [procedure] openwrite :filename end\n"                               // MISSING
//    "primitive [function logic] or :boolean1 :boolean2 [:rest] end\n"               // external
//    "primitive [function arithmetic] orderedp&ordered? :number1 :number2 end\n"     // MISSING
//    "primitive [procedure control inline] output&op :thing end\n"                   // inline

    // P
    { "pick",              primitive_pick,              1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
//    "primitive [function] plistp&plist? :name end\n"                                // MISSING
//    "primitive [function] plists end\n"                                             // MISSING
//    "primitive [function arithmetic] plus :number end\n"                            // external
//    "primitive [function] pop :stack end\n"                                         // MISSING
//    "primitive [procedure] pprop :plistname :propname :value end\n"                 // MISSING
//    "primitive [function] prefix end\n"                                             // MISSING
    { "primitive?",        primitive_primitivep,        1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "primitivep",        primitive_primitivep,        1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "primitives",        primitive_primitives,        0UL, 0UL,          0UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "pr",                primitive_print,             1UL, 1UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE },
    { "print",             primitive_print,             1UL, 1UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE },
    { "procedure?",        primitive_procedurep,        1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "procedurep",        primitive_procedurep,        1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "procedures",        primitive_procedures,        0UL, 0UL,          0UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "product",           primitive_product,           2UL, 2UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
//    "primitive [procedure] push :stack :thing end\n"                                // MISSING

    // Q
    { "queue",             primitive_queue,             2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE },
    { "quoted",            primitive_quoted,            1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "quotient",          primitive_quotient,          2UL, 2UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },

    // R
    { "radarccos",     primitive_radarccos,   1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "radarcsin",     primitive_radarcsin,   1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "radarctan",     primitive_radarctan,   1UL, 1UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "radcos",        primitive_radcos,      1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "radsin",        primitive_radsin,      1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "radtan",        primitive_radtan,      1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "random",        primitive_random,      1UL, 1UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
//    "primitive [function] rawascii :char end\n"                                     // MISSING
//    "primitive [function] readchar&rc end\n"                                        // MISSING
//    "primitive [function] readchars&rcs :number end\n"                              // MISSING
//    "primitive [function] reader end\n"                                             // MISSING
//    "primitive [function] readlist&rl end\n"                                        // MISSING
//    "primitive [function] readpos end\n"                                            // MISSING
//    "primitive [function] readrawline end\n"                                        // MISSING
//    "primitive [function] readword&rw end\n"                                        // MISSING
    { "remainder",     primitive_remainder,   2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "remdup",        primitive_remdup,      2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "remove",        primitive_remove,      2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
//    "primitive [procedure] remprop :plistname :propname end\n"                      // MISSING
    { "repcount",      primitive_repcount,    0UL, 0UL,          0UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "repeat",        nullptr,               2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE },
    { "rerandom",      primitive_rerandom,    0UL, 0UL,          1UL, PROCEDURE_FLAG_PRIMITIVE },
//    "primitive [function] reverse :list end\n"                                      // MISSING

    // S
//    "primitive [function] sentence&se :thing1 :thing2 [:rest] end\n"                // MISSING
    { "setcursor",      primitive_setcursor,     1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE },
//    "primitive [procedure] setcursor :position end\n"                               // MISSING
    { "setitem",        primitive_setitem,       3UL, 3UL,          3UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
//    "primitive [procedure] setprefix :prefix end\n"                                 // MISSING
//    "primitive [procedure] setread :filename end\n"                                 // MISSING
//    "primitive [procedure] setreadpos :number end\n"                                // MISSING
    { "settextcolor",   primitive_settextcolor,  2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE },
//    "primitive [procedure] setwrite :filename end\n"                                // MISSING
//    "primitive [procedure] setwritepos :number end\n"                               // MISSING
//    "primitive [function] shell :command [:wordflag] end\n"                         // MISSING
    { "show",         primitive_show,        1UL, 1UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE },
    { "sin",          primitive_sin,         1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "sqrt",         primitive_sqrt,        1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
//    "primitive [procedure] standout :thing end\n"                                   // MISSING
    { "stop",         nullptr,               0UL, 0UL,          0UL, PROCEDURE_FLAG_PRIMITIVE },
//    "primitive [function] substringp&substring? :thing end\n"                       // MISSING
    { "sum",          primitive_sum,         2UL, 2UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    // T
    { "tag",          nullptr,               1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE },
    { "tan",          primitive_tan,         1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "test",         primitive_test,        1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE },
    { "thing",        nullptr,               1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "throw",        nullptr,               1UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE },
    { "time",         primitive_time,        0UL, 0UL,          0UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
//    "primitive [procedure] trace :list end\n"                                       // MISSING
//    "primitive [function] tracedp&traced? :list end\n"                              // MISSING
    { "tty?",         primitive_ttyp,        0UL, 0UL,          0UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "ttyp",         primitive_ttyp,        0UL, 0UL,          0UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "type",         primitive_type,        1UL, 1UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE },
    // U
    { "unordered?",   primitive_unorderedp,  2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "unorderedp",   primitive_unorderedp,  2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "until",        nullptr,               2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE },
//    "primitive [procedure] untrace :list end\n"                                     // MISSING
//    "primitive [function] uppercase :word end\n"                                    // MISSING
    // W
    { "wait",         nullptr,               1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE },
    { "while",        nullptr,               2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE },
    { "word",         primitive_word,        2UL, 2UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "word?",        primitive_wordp,       1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "wordp",        primitive_wordp,       1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
//    "primitive [function] writepos end \"writepos\n"                                // MISSING
//    "primitive [function] writer end\n"                                             // MISSING
    // X
    { "xor",          primitive_xor,         2UL, 2UL, MAX_MAX_ARGS, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION }
};



} // no name namespace




lpp__auto_register_procedures::lpp__auto_register_procedures(lpp__procedure_info_t const * procedures, size_t count)
{
    if(procedures == nullptr
    && count == 0)
    {
        procedures = g_primitives;
        count = sizeof(g_primitives) / sizeof(g_primitives[0]);
    }

    for(size_t idx(0); idx < count; ++idx)
    {
        std::string const & name(procedures[idx].f_name);
        if(g_procedures.find(name) != g_procedures.end())
        {
            // this should never happen since the linker should complain
            //
            throw std::logic_error("found two procedures named \""
                                 + name
                                 + "\".");
        }

        g_procedures[name] = procedures + idx;
    }
}


lpp__procedure_info_t const * find_procedure(std::string const & name)
{
    auto const it(g_procedures.find(name));
    if(it == g_procedures.end())
    {
        return nullptr;
    }

    return it->second;
}


lpp__value::vector_t get_procedures(procedure_flag_t flag)
{
    lpp__value::vector_t list;

    for(auto it(g_procedures.begin()); it != g_procedures.end(); ++it)
    {
        if((it->second->f_flags & flag) != 0)
        {
            list.push_back(std::make_shared<lpp__value>(it->first));
        }
    }

    return list;
}



} // lpp namespace
// vim: ts=4 sw=4 et nocindent
