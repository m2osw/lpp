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
    { "count",        primitive_count,        1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE },
//    "primitive [function] cursor end\n"                                             // MISSING
//    // D
//    "primitive [function] definedp&defined? :name end\n"                            // external
//    "primitive [function arithmetic] difference :number1 :number2 [:rest] end\n"    // external
//    "primitive [function] dequeue :queue end\n"                                     // external
//    "primitive [procedure control inline] do.until :boolean :once_and_if_false end\n"   // inline
//    "primitive [procedure control inline] do.while :boolean :once_and_if_true end\n"    // inline
//    // E
//    "primitive [function] emptyp&empty? :thing end\n"                               // external
//    "primitive [function] equalp&equal? :thing1 :thing2 end\n"                      // external
//    "primitive [function] eofp end\n"                                               // MISSING
//    "primitive [procedure] erasefile&erf :filename end\n"                           // MISSING
//    "primitive [function] error end\n"                                              // external
//    "primitive [function] errorp&error? end\n"                                      // external
//    // F
//    "primitive [function] filep&file? :filename end\n"                              // MISSING
//    "primitive [function] first :thing end\n"                                       // external
//    "primitive [function] floatp&float? :thing end\n"                               // external
//    "primitive [procedure control inline] for :control :instructions end\n"         // inline
//    "primitive [procedure control inline] forever :instructions end\n"              // inline
//    "primitive [function] fput :item :thing end\n"                                  // external
//    // G
//    "primitive [procedure] gc [:flag void] end\n"                                   // MISSING
//    "primitive [function] gensym end\n"                                             // MISSING
//    "primitive [procedure] global :name [:rest] end\n"                              // MISSING
//    "primitive [procedure control inline] goto :tag end\n"                          // inline
//    "primitive [function] gprop :plistname :propname end\n"                         // MISSING
//    "primitive [function] greaterequalp&greaterequal? :thing1 :thing2 [:rest] end\n"    // external
//    "primitive [function] greaterp&greater? :thing1 :thing2 [:rest] end\n"          // external
//    // I
//    "primitive [procedure control inline] if :boolean :if_true [:if_false void] 3 end\n"    // inline
//    "primitive [procedure control inline] ifelse :boolean :if_true :if_false end\n" // inline
//    "primitive [procedure control inline] iffalse&iff :if_false end\n"              // inline
//    "primitive [procedure control inline] iftrue&ift :if_true end\n"                // inline
//    "primitive [procedure] ignore :thing [:rest] end\n"                             // inline (special)
//    "primitive [function] int :number end\n"                                        // external
//    "primitive [function] integerp&integer? :thing end\n"                           // external
//    "primitive [function] item :number :thing end\n"                                // external
//    // K
//    "primitive [function] keyp&key? end\n"                                          // MISSING
//    // L
//    "primitive [function] last :list end\n"                                         // external
//    "primitive [function] lessequalp&lessequal? :thing1 :thing2 [:rest] end\n"      // external
//    "primitive [function] lessp&less? :thing1 :thing2 [:rest] end\n"                // external
//    "primitive [function] list :thing [:rest] 2 end\n"                              // external
//    "primitive [function] listp&list? :thing end\n"                                 // external
//    "primitive [procedure] local :name [:rest] end\n"                               // MISSING
//    "primitive [procedure] localmake :name :value end\n"                            // MISSING
//    "primitive [function] lowercase :word end\n"                                    // MISSING
//    "primitive [function] lput :item :thing end\n"                                  // external
//    "primitive [function arithmetic] lshift :number :shift end\n"                   // external
//    // M
//    "primitive [procedure] make :name :thing end\n"                                 // external
//    "primitive [procedure] mdsetitem :list_of_positions :list :value end\n"         // MISSING
//    "primitive [function] member :thing1 :thing2 end\n"                             // MISSING
//    "primitive [function] memberp&member? :thing end\n"                             // MISSING
//    "primitive [function arithmetic] minus :number end\n"                           // external
//    "primitive [function arithmetic] modulo :number1 :number2 end\n"                // external
//    // N
//    "primitive [procedure] name :thing :name end\n"                                 // external
//    "primitive [function] namedp&named? :name end\n"                                // MISSING
//    "primitive [function] names end\n"                                              // MISSING
    { "nan?",        primitive_nanp,        1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "nanp",        primitive_nanp,        1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
//    "primitive [function] nodes end\n" // return free memory?                       // MISSING
//    "primitive [function] not :boolean end\n"                                       // external
//    "primitive [function] notequalp&notequal? :thing end\n"                         // external
//    "primitive [function] numberp&number? :thing end\n"                             // external
//    // O
//    "primitive [procedure] openappend :filename end\n"                              // MISSING
//    "primitive [procedure] openread :filename end\n"                                // MISSING
//    "primitive [procedure] openupdate :filename end\n"                              // MISSING
//    "primitive [procedure] openwrite :filename end\n"                               // MISSING
//    "primitive [function logic] or :boolean1 :boolean2 [:rest] end\n"               // external
//    "primitive [function arithmetic] orderedp&ordered? :number1 :number2 end\n"     // MISSING
//    "primitive [procedure control inline] output&op :thing end\n"                   // inline
//    // P
//    "primitive [function] pick :list end\n"                                         // MISSING
//    "primitive [function] plistp&plist? :name end\n"                                // MISSING
//    "primitive [function] plists end\n"                                             // MISSING
//    "primitive [function arithmetic] plus :number end\n"                            // external
//    "primitive [function] pop :stack end\n"                                         // MISSING
//    "primitive [procedure] pprop :plistname :propname :value end\n"                 // MISSING
//    "primitive [function] prefix end\n"                                             // MISSING
    { "primitive?",        primitive_primitivep,        1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "primitivep",        primitive_primitivep,        1UL, 1UL,          1UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
//    "primitive [function] primitives end\n"                                         // MISSING
//    "primitive [procedure] print&pr :thing [:rest] end\n"                           // external
//    "primitive [function] procedurep&procedure? :thing end\n"                       // MISSING
//    "primitive [function] procedures end\n"                                         // MISSING
//    "primitive [function arithmetic] product :number1 :number2 [:rest] end\n"       // external
//    "primitive [procedure] push :stack :thing end\n"                                // MISSING
//    // Q
//    "primitive [procedure] queue :queue :thing end\n"                               // MISSING
//    "primitive [function] quoted :thing end\n"                                      // MISSING
//    "primitive [function arithmetic] quotient :number1 :number2 [:rest] end\n"      // external
//    // R
//    "primitive [function arithmetic] radarccos :number1 end\n"                      // external
//    "primitive [function arithmetic] radarctan :number1 [:number2] end\n"           // external
//    "primitive [function arithmetic] radarcsin :number1 end\n"                      // external
//    "primitive [function arithmetic] radcos :number1 end\n"                         // external
//    "primitive [function arithmetic] radtan :number1 [:number2] end\n"              // external
//    "primitive [function arithmetic] radsin :number1 end\n"                         // external
//    "primitive [function] random :number [:rest] end\n"                             // external
//    "primitive [function] rawascii :char end\n"                                     // MISSING
//    "primitive [function] readchar&rc end\n"                                        // MISSING
//    "primitive [function] readchars&rcs :number end\n"                              // MISSING
//    "primitive [function] reader end\n"                                             // MISSING
//    "primitive [function] readlist&rl end\n"                                        // MISSING
//    "primitive [function] readpos end\n"                                            // MISSING
//    "primitive [function] readrawline end\n"                                        // MISSING
//    "primitive [function] readword&rw end\n"                                        // MISSING
//    "primitive [function arithmetic] remainder :number1 :number2 end\n"             // external
//    "primitive [function] remdup :list end\n"                                       // MISSING
//    "primitive [function] remove :thing :list end\n"                                // MISSING
//    "primitive [procedure] remprop :plistname :propname end\n"                      // MISSING
//    "primitive [function] repcount end\n"                                           // external
//    "primitive [procedure control inline] repeat :number :instructions end\n"       // inline
//    "primitive [function] rerandom [:rest] end\n"                                   // external
//    "primitive [function] reverse :list end\n"                                      // MISSING
//    // S
//    "primitive [function] sentence&se :thing1 :thing2 [:rest] end\n"                // MISSING
//    "primitive [procedure] setcursor :position end\n"                               // MISSING
//    "primitive [inline] .setfirst :list :value end\n"                               // ?
//    "primitive [function ] .setbf :list :value end\n"                               // ?
//    "primitive [procedure] setitem :position :list :value end\n"                    // MISSING
//    "primitive [procedure] .setitem :position :list :value end\n"                   // ?
//    "primitive [procedure] setprefix :prefix end\n"                                 // MISSING
//    "primitive [procedure] setread :filename end\n"                                 // MISSING
//    "primitive [procedure] setreadpos :number end\n"                                // MISSING
//    "primitive [procedure] settextcolor :foreground :background end\n"              // MISSING
//    "primitive [procedure] setwrite :filename end\n"                                // MISSING
//    "primitive [procedure] setwritepos :number end\n"                               // MISSING
//    "primitive [function] shell :command [:wordflag] end\n"                         // MISSING
//    "primitive [procedure] show :thing [:rest] end\n"                               // external
//    "primitive [function arithmetic] sqrt :number end\n"                            // external
//    "primitive [procedure] standout :thing end\n"                                   // MISSING
//    "primitive [procedure control inline] stop end\n"                               // inline
//    "primitive [function] substringp&substring? :thing end\n"                       // MISSING
//    "primitive [function arithmetic] sum :number1 :number2 [:rest] end\n"           // external
//    // T
//    "primitive [procedure control inline] tag :tag end\n"                           // inline
//    "primitive [procedure] test :boolean end\n"                                     // external
//    "primitive [function inline] thing :name end\n"                                 // inline
//    "primitive [procedure control inline] throw :tag [:value void] 2 end\n"         // inline
//    "primitive [function] time end\n"                                               // external
//    "primitive [procedure] trace :list end\n"                                       // MISSING
//    "primitive [function] tracedp&traced? :list end\n"                              // MISSING
//    "primitive [procedure] type :thing [:rest] end\n"                               // external
//    // U
    { "unordered?",   primitive_unorderedp,  2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
    { "unorderedp",   primitive_unorderedp,  2UL, 2UL,          2UL, PROCEDURE_FLAG_PRIMITIVE | PROCEDURE_FLAG_FUNCTION },
//    "primitive [procedure control inline] until :boolean :if_false end\n"           // inline
//    "primitive [procedure] untrace :list end\n"                                     // MISSING
//    "primitive [function] uppercase :word end\n"                                    // MISSING
//    // W
//    "primitive [procedure inline] wait :seconds end\n"                              // inline
//    "primitive [procedure control inline] while :boolean :if_true end\n"            // inline
//    "primitive [function] word :word1 :word2 [:rest] end\n"                         // external
//    "primitive [function] wordp&word? :thing end\n"                                 // external
//    "primitive [function] writepos end \"writepos\n"                                // MISSING
//    "primitive [function] writer end\n"                                             // MISSING
//    // X
//    "primitive [function logic] xor :boolean1 :boolean2 [:rest] end\n"              // external
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
