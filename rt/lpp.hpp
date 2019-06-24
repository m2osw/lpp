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

// boost lib
// (C++17 will have these)
//
#include <boost/variant.hpp>

// C++ lib
//
#include <map>
#include <memory>
#include <stdexcept>
#include <string>



namespace lpp
{

enum class lpp__special_value_t
{
    LPP__SPECIAL_VALUE_NOT_SET      // a.k.a. the VOID value
};


// WARNING: must match the variant::which() one to one
enum class lpp__value_type_t
{
    LPP__VALUE_TYPE_SPECIAL,
    LPP__VALUE_TYPE_BOOLEAN,
    LPP__VALUE_TYPE_INTEGER,
    LPP__VALUE_TYPE_FLOAT,
    LPP__VALUE_TYPE_WORD,
    LPP__VALUE_TYPE_LIST
};

constexpr int mix_value_types(lpp__value_type_t a, lpp__value_type_t b)
{
    return static_cast<int>(a) * 256 + static_cast<int>(b);
}


typedef std::uint_fast32_t      display_flag_t;

constexpr display_flag_t        DISPLAY_FLAG_BACKSLASHED = 0x0001;  // add backslashes if necessary to re-read the value
constexpr display_flag_t        DISPLAY_FLAG_TYPED       = 0x0002;  // show " for words, [] for lists


typedef std::int_fast64_t       lpp__integer_t;
typedef double                  lpp__float_t;


typedef std::vector<lpp__integer_t>     lpp__vector_integer_t;


class lpp__value
{
public:
    typedef std::shared_ptr<lpp__value>     pointer_t;
    typedef std::vector<pointer_t>          vector_t;

                            lpp__value(lpp__special_value_t value = lpp__special_value_t::LPP__SPECIAL_VALUE_NOT_SET);
                            lpp__value(bool value);
                            lpp__value(lpp__integer_t value);
                            lpp__value(lpp__float_t value);
                            lpp__value(std::string const & value);
                            lpp__value(vector_t const & value);

    lpp__value_type_t       type() const;
    bool                    is_set() const;
    bool                    represents_word() const;
    bool                    represents_integer() const;
    bool                    represents_float() const;

    bool                    get_boolean() const;
    lpp__integer_t          get_integer() const;
    lpp__float_t            get_float() const;
    std::string const &     get_word() const;
    vector_t const &        get_list() const;

    void                    unset();
    void                    set_boolean(bool value);
    void                    set_integer(lpp__integer_t value);
    void                    set_float(lpp__float_t value);
    void                    set_word(std::string const & value);
    void                    set_list(vector_t const & value);

    std::string             to_string(display_flag_t flags = 0, int depth = 0) const;
    std::string             to_word(display_flag_t flags = 0) const;
    lpp__integer_t          to_integer() const;
    lpp__float_t            to_float() const;

    void                    add_prop(std::string const & name
                                   , lpp__value::pointer_t value);

private:
    // WARNING: the order of the types is captured in the lpp__value_type_t
    //          and it must match one to one
    //
    boost::variant<lpp__special_value_t
                 , bool
                 , lpp__integer_t
                 , double
                 , std::string
                 , vector_t>
                            f_value = lpp__special_value_t::LPP__SPECIAL_VALUE_NOT_SET;
};





enum class lpp__thing_type_t
{
    LPP__THING_TYPE_DEFAULT,     // if variable exists anywhere in the context, use that variable, otherwise like GLOBAL
    LPP__THING_TYPE_CONTEXT,     // make variable locally (inside current context)
    LPP__THING_TYPE_PROCEDURE,   // make variable locally (inside current procedure)
    LPP__THING_TYPE_GLOBAL       // access the global variable
};


class lpp__thing
{
public:
    typedef std::shared_ptr<lpp__thing>             pointer_t;
    typedef std::map<std::string, pointer_t>        map_t;

    lpp__thing_type_t       get_type() const;
    void                    set_type(lpp__thing_type_t type);

    bool                    is_set() const;
    lpp__value::pointer_t   get_value() const;
    void                    set_value(lpp__value::pointer_t value);
    void                    set_value(lpp__value::pointer_t value, lpp__thing_type_t type);

private:
    lpp__thing_type_t       f_type = lpp__thing_type_t::LPP__THING_TYPE_DEFAULT;
    lpp__value::pointer_t   f_value = lpp__value::pointer_t();
};



class lpp__context;


enum class lpp__error_code_t
{
    ERROR_CODE_NONE = -1,                   //      no error
    ERROR_CODE_FATAL,                       //  0   Fatal internal error (can't be caught)
    ERROR_CODE_OUT_OF_MEMORY,               //  1   Out of memory
    ERROR_CODE_STACK_OVERFLOW,              //  2   Stack overflow
    ERROR_CODE_TURTLE_OUT_OF_BOUNDS,        //  3   Turtle out of bounds                             [NOT RELEVANT]
    ERROR_CODE_FATAL_INVALID_DATUM,         //  4   PROC doesn't like DATUM as input (not recoverable)
    ERROR_CODE_OUTPUT_EXPECTED,             //  5   PROC didn't output to PROC
    ERROR_CODE_MISSING_ARGUMENTS,           //  6   Not enough inputs to PROC                        [NOT RELEVANT]
    ERROR_CODE_INVALID_DATUM,               //  7   PROC doesn't like DATUM as input (recoverable)
    ERROR_CODE_TOO_MANY_PARENTHESIS,        //  8   Too much inside ()'s                             [NOT RELEVANT]
    ERROR_CODE_TOO_MANY_INPUTS,             //  9   You don't say what to do with DATUM              [NOT RELEVANT]
    ERROR_CODE_MISSING_CLOSE_PARENTHESIS,   // 10   ')' not found                                    [NOT RELEVANT]
    ERROR_CODE_VARIABLE_NOT_SET,            // 11   VAR has no value
    ERROR_CODE_UNEXPECTED_CLOSE_PARENTHESIS,// 12   Unexpected ')'                                   [NOT RELEVANT]
    ERROR_CODE_UNKNOWN_PROCEDURE,           // 13   I don't know how to PROC (recoverable)           [NOT RELEVANT]
    ERROR_CODE_CATCH_TAG_NOT_FOUND,         // 14   Can't find catch tag for THROWTAG
    ERROR_CODE_PROCEDURE_ALREADY_DEFINED,   // 15   PROC is already defined                          [NOT RELEVANT]
    ERROR_CODE_STOPPED,                     // 16   Stopped                                          [NOT RELEVANT]
    ERROR_CODE_ALREADY_DRIBBLING,           // 17   Already dribbling
    ERROR_CODE_FILE_SYSTEM_ERROR,           // 18   File system error
    ERROR_CODE_ASSUMING_IFELSE,             // 19   Assuming you mean IFELSE, not IF (warning only)
    ERROR_CODE_VARIABLE_SHADOWED,           // 20   VAR shadowed by local in procedure call (warning only)
    ERROR_CODE_THROW,                       // 21   Throw "Error
    ERROR_CODE_PROCEDURE_IS_A_PRIMITIVE,    // 22   PROC is a primitive                              [NOT RELEVANT]
    ERROR_CODE_CANNOT_USE__TO__HERE,        // 23   Can't use TO inside a procedure                  [NOT RELEVANT]
    ERROR_CODE_FATAL_UNKNOWN_PROCEDURE,     // 24   I don't know how to PROC (not recoverable)       [NOT RELEVANT]
    ERROR_CODE_IFTRUE_IFFALSE_WITHOUT_TEST, // 25   IFTRUE/IFFALSE without TEST
    ERROR_CODE_UNEXPECTED_CLOSE_LIST,       // 26   Unexpected ']'                                   [NOT RELEVANT]
    ERROR_CODE_UNEXPECTED_CLOSE_BRACKET,    // 27   Unexpected '}'                                   [NOT RELEVANT]
    ERROR_CODE_NO_GRAPHICS_AVAILABLE,       // 28   Couldn't initialize graphics                     [NOT RELEVANT]
    ERROR_CODE_INVALID_MACRO_RETURNED_VALUE,// 29   Macro returned VALUE instead of a list
    ERROR_CODE_VALUE_ON_ITS_OWN,            // 30   You don't say what to do with VALUE              [NOT RELEVANT]
    ERROR_CODE_STOP_OR_OUTPUT_ONLY,         // 31   Can only use STOP or OUTPUT inside a procedure   [NOT RELEVANT]
    ERROR_CODE_APPLY_DID_NOT_LIKE_BAD_THING,// 32   APPLY doesn't like BADTHING as input
    ERROR_CODE_UNEXPECTED_END,              // 33   END inside multi-line instruction                [NOT RELEVANT]
    ERROR_CODE_REALLY_OUT_OF_MEMORY,        // 34   Really out of memory (can't be caught)
    ERROR_CODE_ARITHMETIC_ERROR,            // 35   --added to lpp--
};


class lpp__error
    : public std::runtime_error
{
public:
                            lpp__error();
                            lpp__error(std::shared_ptr<lpp__context const> context
                                     , lpp__error_code_t code
                                     , std::string const & tag
                                     , std::string const & message
                                     , lpp__value::pointer_t value = lpp__value::pointer_t());

    lpp__error_code_t       code() const;
    std::string const &     tag() const;
    time_t                  timestamp() const;
    lpp__value::pointer_t   value() const;
    std::string const &     filename() const;
    std::string const &     procedure() const;
    lpp__integer_t          line() const;
    std::string const &     primitive() const;
    void                    caught(bool state) const;
    bool                    caught() const;

private:
    lpp__error_code_t       f_code = lpp__error_code_t::ERROR_CODE_NONE;
    std::string             f_tag = std::string();
    time_t                  f_timestamp = time(nullptr);
    lpp__value::pointer_t   f_value = lpp__value::pointer_t();
    std::string             f_filename = std::string();
    std::string             f_procedure = std::string();
    lpp__integer_t          f_line = 0;
    std::string             f_primitive = std::string();
    mutable bool            f_caught = false;
};


enum class test_t : std::int8_t
{
    TEST_UNDEFINED,
    TEST_TRUE,
    TEST_FALSE
};


class lpp__context
    : public std::enable_shared_from_this<lpp__context>
{
public:
    typedef std::shared_ptr<lpp__context>       pointer_t;
    typedef std::weak_ptr<lpp__context>         weak_pointer_t;
    typedef std::shared_ptr<lpp__context const> const_pointer_t;

                            lpp__context(std::string const & filename, std::string const & procedure, lpp__integer_t line, bool primitive);
                            ~lpp__context();

    std::string const &     get_filename() const;
    std::string             get_procedure_name() const;
    lpp__integer_t          get_current_line() const;
    std::string             get_primitive_name() const;

    void                    set_global(pointer_t global);
    pointer_t               get_global();

    lpp__thing::pointer_t   find_thing(std::string const & name) const;
    lpp__thing::pointer_t   get_thing(std::string const & name) const;  // throw if not set
    void                    set_thing(std::string const & name
                                    , lpp__value::pointer_t value
                                    , lpp__thing_type_t type = lpp__thing_type_t::LPP__THING_TYPE_DEFAULT);

    bool                    has_returned_value() const;
    lpp__value::pointer_t   get_returned_value() const; // throw if not set (i.e. last call did a STOP not an OUTPUT)
    void                    set_return_value(lpp__value::pointer_t value);

    void                    set_error(lpp__error const & e) const;
    lpp__error const &      get_error() const;

    void                    add_repeat_count(lpp__integer_t count);
    void                    remove_last_repeat_count();
    lpp__integer_t          get_repeat_count() const;

    void                    set_test(test_t value);
    test_t                  get_test() const;

    void                    attach(pointer_t parent);

private:
    weak_pointer_t          f_global = lpp__context::pointer_t();
    pointer_t               f_parent = lpp__context::pointer_t();
    lpp__thing::map_t       f_things = lpp__thing::map_t();
    lpp__value::pointer_t   f_return_value = lpp__value::pointer_t();
    lpp__vector_integer_t   f_repeat_count = lpp__vector_integer_t();
    std::string             f_filename = std::string();
    std::string             f_procedure = std::string();
    lpp__integer_t          f_line = 0;
    bool                    f_primitive = false;
    test_t                  f_test = test_t::TEST_UNDEFINED;
    mutable lpp__error      f_last_error = lpp__error();
};



class lpp__raii_repeat_count
{
public:
                            lpp__raii_repeat_count(lpp__context::pointer_t context, lpp__integer_t count);
                            ~lpp__raii_repeat_count();

private:
    lpp__context::pointer_t f_context;
};




class lpp__number
{
public:
                                lpp__number(lpp__context::pointer_t context);
                                lpp__number(lpp__context::pointer_t context
                                          , lpp__value::pointer_t number);

    bool                        from_string(std::string const & number);
    bool                        from_value(lpp__value::pointer_t number);

    bool                        is_integer() const;
    lpp__integer_t              get_integer() const;
    lpp__float_t                get_float() const;
    lpp__value::pointer_t       to_value() const;
    void                        save_as_return_value();

    void                        apply_unary(
                                          std::function<lpp__integer_t(lpp__context::pointer_t context, lpp__integer_t lhs, lpp__integer_t rhs)> i
                                        , std::function<lpp__float_t(lpp__context::pointer_t context, lpp__float_t lhs, lpp__float_t rhs)> f);
    void                        apply_binary(
                                          lpp__value::pointer_t number
                                        , std::function<lpp__integer_t(lpp__context::pointer_t context, lpp__integer_t lhs, lpp__integer_t rhs)> i
                                        , std::function<lpp__float_t(lpp__context::pointer_t context, lpp__float_t lhs, lpp__float_t rhs)> f);

    static void                 compute(
                                          lpp__context::pointer_t context
                                        , std::string const & lhs_name
                                        , std::string const & rhs_name
                                        , std::string const & rest_name
                                        , std::function<lpp__integer_t(lpp__context::pointer_t context, lpp__integer_t lhs, lpp__integer_t rhs)> i
                                        , std::function<lpp__float_t(lpp__context::pointer_t context, lpp__float_t lhs, lpp__float_t rhs)> f);

    static void                 compare(
                                          lpp__context::pointer_t context
                                        , std::string const & lhs_name
                                        , std::string const & rhs_name
                                        , std::string const & rest_name
                                        , std::function<bool(lpp__integer_t compare)> f
                                        , bool characters = false);

    static void                 logical(
                                          lpp__context::pointer_t context
                                        , std::string const & lhs_name
                                        , std::string const & rhs_name
                                        , std::string const & rest_name
                                        , std::function<bool(bool lhs, bool rhs)> b);

private:
    lpp__context::pointer_t     f_context;
    bool                        f_is_integer = true;
    lpp__integer_t              f_integer = 0;
    lpp__float_t                f_float = 0.0;
};




typedef std::uint_fast32_t      procedure_flag_t;

constexpr procedure_flag_t      PROCEDURE_FLAG_PRIMITIVE = 0x00000001;  // it's a language/system primitive
constexpr procedure_flag_t      PROCEDURE_FLAG_PROCEDURE = 0x00000002;  // it's a user defined procedure
constexpr procedure_flag_t      PROCEDURE_FLAG_FUNCTION  = 0x00000004;  // it returns something

typedef void (*lpp__procedure_t)(lpp::lpp__context::pointer_t context);

struct lpp__procedure_info_t
{
    char const *                f_name = nullptr;
    lpp__procedure_t            f_procedure = nullptr;
    std::uint32_t               f_min_args = 0;
    std::uint32_t               f_def_args = 0;
    std::uint32_t               f_max_args = 0;
    procedure_flag_t            f_flags = 0;
};

class lpp__auto_register_procedures
{
public:
    lpp__auto_register_procedures(lpp__procedure_info_t const * procedures, size_t count);
};

lpp__procedure_info_t const *           find_procedure(std::string const & name);
lpp__value::vector_t                    get_procedures(procedure_flag_t flag);






} // lpp namespace

// primitives

// A
void primitive_and(lpp::lpp__context::pointer_t context);
void primitive_arccos(lpp::lpp__context::pointer_t context);
void primitive_arcsin(lpp::lpp__context::pointer_t context);
void primitive_arctan(lpp::lpp__context::pointer_t context);
void primitive_arity(lpp::lpp__context::pointer_t context);
void primitive_ascii(lpp::lpp__context::pointer_t context);
void primitive_ashift(lpp::lpp__context::pointer_t context);

// B
void primitive_backslashedp(lpp::lpp__context::pointer_t context);
void primitive_beforep(lpp::lpp__context::pointer_t context);
void primitive_bitand(lpp::lpp__context::pointer_t context);
void primitive_bitnot(lpp::lpp__context::pointer_t context);
void primitive_bitor(lpp::lpp__context::pointer_t context);
void primitive_bitxor(lpp::lpp__context::pointer_t context);
void primitive_butfirst(lpp::lpp__context::pointer_t context);
void primitive_butlast(lpp::lpp__context::pointer_t context);
void primitive_bye(lpp::lpp__context::pointer_t context);

// C
void primitive_char(lpp::lpp__context::pointer_t context);
void primitive_cleartext(lpp::lpp__context::pointer_t context);
void primitive_combine(lpp::lpp__context::pointer_t context);
void primitive_comparablep(lpp::lpp__context::pointer_t context);
void primitive_cos(lpp::lpp__context::pointer_t context);
void primitive_count(lpp::lpp__context::pointer_t context);

// D
void primitive_difference(lpp::lpp__context::pointer_t context);
void primitive_definedp(lpp::lpp__context::pointer_t context);
void primitive_dequeue(lpp::lpp__context::pointer_t context);

// E
void primitive_emptyp(lpp::lpp__context::pointer_t context);
void primitive_equalp(lpp::lpp__context::pointer_t context);
void primitive_error(lpp::lpp__context::pointer_t context);
void primitive_errorp(lpp::lpp__context::pointer_t context);
void primitive_exp(lpp::lpp__context::pointer_t context);

// F
void primitive_first(lpp::lpp__context::pointer_t context);
void primitive_floatp(lpp::lpp__context::pointer_t context);
void primitive_fput(lpp::lpp__context::pointer_t context);

// G
void primitive_greaterequalp(lpp::lpp__context::pointer_t context);
void primitive_greaterp(lpp::lpp__context::pointer_t context);

// I
void primitive_int(lpp::lpp__context::pointer_t context);
void primitive_integerp(lpp::lpp__context::pointer_t context);
void primitive_iseq(lpp::lpp__context::pointer_t context);
void primitive_item(lpp::lpp__context::pointer_t context);

// L
void primitive_last(lpp::lpp__context::pointer_t context);
void primitive_lessequalp(lpp::lpp__context::pointer_t context);
void primitive_lessp(lpp::lpp__context::pointer_t context);
void primitive_list(lpp::lpp__context::pointer_t context);
void primitive_listp(lpp::lpp__context::pointer_t context);
void primitive_ln(lpp::lpp__context::pointer_t context);
void primitive_log(lpp::lpp__context::pointer_t context);
void primitive_lput(lpp::lpp__context::pointer_t context);
void primitive_lshift(lpp::lpp__context::pointer_t context);

// M
void primitive_make(lpp::lpp__context::pointer_t context);
void primitive_minus(lpp::lpp__context::pointer_t context);
void primitive_modulo(lpp::lpp__context::pointer_t context);

// N
inline void primitive_name(lpp::lpp__context::pointer_t context) { primitive_make(context); }
void primitive_nanp(lpp::lpp__context::pointer_t context);
void primitive_numberp(lpp::lpp__context::pointer_t context);

// O
void primitive_or(lpp::lpp__context::pointer_t context);

// P
void primitive_plus(lpp::lpp__context::pointer_t context);
void primitive_power(lpp::lpp__context::pointer_t context);
void primitive_primitivep(lpp::lpp__context::pointer_t context);
void primitive_primitives(lpp::lpp__context::pointer_t context);
void primitive_print(lpp::lpp__context::pointer_t context);
void primitive_procedures(lpp::lpp__context::pointer_t context);
void primitive_product(lpp::lpp__context::pointer_t context);

// Q
void primitive_quotient(lpp::lpp__context::pointer_t context);

// R
void primitive_radarccos(lpp::lpp__context::pointer_t context);
void primitive_radarcsin(lpp::lpp__context::pointer_t context);
void primitive_radarctan(lpp::lpp__context::pointer_t context);
void primitive_radcos(lpp::lpp__context::pointer_t context);
void primitive_radsin(lpp::lpp__context::pointer_t context);
void primitive_radtan(lpp::lpp__context::pointer_t context);
void primitive_random(lpp::lpp__context::pointer_t context);
void primitive_remainder(lpp::lpp__context::pointer_t context);
void primitive_repcount(lpp::lpp__context::pointer_t context);
void primitive_rerandom(lpp::lpp__context::pointer_t context);
void primitive_round(lpp::lpp__context::pointer_t context);
void primitive_rseq(lpp::lpp__context::pointer_t context);

// S
void primitive_show(lpp::lpp__context::pointer_t context);
void primitive_sin(lpp::lpp__context::pointer_t context);
void primitive_sqrt(lpp::lpp__context::pointer_t context);
void primitive_sum(lpp::lpp__context::pointer_t context);

// T
void primitive_tan(lpp::lpp__context::pointer_t context);
void primitive_test(lpp::lpp__context::pointer_t context);
void primitive_time(lpp::lpp__context::pointer_t context);
void primitive_type(lpp::lpp__context::pointer_t context);

// U
void primitive_unorderedp(lpp::lpp__context::pointer_t context);

// W
void primitive_word(lpp::lpp__context::pointer_t context);
void primitive_wordp(lpp::lpp__context::pointer_t context);

// X
void primitive_xor(lpp::lpp__context::pointer_t context);


// helpers
//

std::ostream & operator << (std::ostream & out, lpp::lpp__value_type_t value_type);

// vim: ts=4 sw=4 et nocindent
