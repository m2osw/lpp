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


typedef std::uint_fast32_t      display_flag_t;

constexpr display_flag_t        DISPLAY_FLAG_BACKSLASHED = 0x0001;  // add backslashes if necessary to re-read the value
constexpr display_flag_t        DISPLAY_FLAG_TYPED       = 0x0002;  // show " for words, [] for lists


typedef std::int_fast64_t       lpp__integer_t;
typedef double                  lpp__float_t;


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

    std::string             to_string(display_flag_t flags = 0, int depth = 0);

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



class lpp__error
    : public std::runtime_error
{
public:
                            lpp__error(std::string const & tag
                                     , std::string const & message
                                     , lpp__value::pointer_t value = lpp__value::pointer_t());

    std::string const &     tag() const;
    lpp__value::pointer_t   value() const;

private:
    std::string             f_tag = std::string();
    lpp__value::pointer_t   f_value = lpp__value::pointer_t();
};



enum class lpp__thing_type_t
{
    LPP__THING_TYPE_DEFAULT,     // if variable exists anywhere in the context, use that variable
    LPP__THING_TYPE_LOCAL,       // make variable locally (inside current procedure)
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


class lpp__context
    : public std::enable_shared_from_this<lpp__context>
{
public:
    typedef std::shared_ptr<lpp__context>       pointer_t;
    typedef std::weak_ptr<lpp__context>         weak_pointer_t;
    typedef std::shared_ptr<lpp__context const> const_pointer_t;

                            ~lpp__context();

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

    void                    attach(pointer_t parent);

private:
    weak_pointer_t          f_global = lpp__context::pointer_t();
    pointer_t               f_parent = lpp__context::pointer_t();
    lpp__thing::map_t       f_things = lpp__thing::map_t();
    lpp__value::pointer_t   f_return_value = lpp__value::pointer_t();
};



} // lpp namespace

// primitives

void primitive_first(lpp::lpp__context::pointer_t context);
void primitive_minus(lpp::lpp__context::pointer_t context);
void primitive_print(lpp::lpp__context::pointer_t context);
void primitive_product(lpp::lpp__context::pointer_t context);
void primitive_show(lpp::lpp__context::pointer_t context);
void primitive_sum(lpp::lpp__context::pointer_t context);
void primitive_throw(lpp::lpp__context::pointer_t context);
void primitive_type(lpp::lpp__context::pointer_t context);


// vim: ts=4 sw=4 et nocindent
