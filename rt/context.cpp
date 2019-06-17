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



} // no name namespace




lpp__context::lpp__context(std::string const & filename, std::string const & procedure, lpp__integer_t line, bool primitive)
    : f_filename(filename)
    , f_procedure(procedure)
    , f_line(line)
    , f_primitive(primitive)
{
}


lpp__context::~lpp__context()
{
}


std::string const & lpp__context::get_filename() const
{
    return f_filename;
}


std::string lpp__context::get_procedure_name() const
{
    lpp__context::const_pointer_t context(shared_from_this());
    while(context->f_primitive)
    {
        context = context->f_parent;
        if(context == nullptr)
        {
            return std::string();
        }
    }
    return context->f_procedure;
}


lpp__integer_t lpp__context::get_current_line() const
{
    return f_line;
}


std::string const & lpp__context::get_primitive_name() const
{
    return f_procedure;
}


void lpp__context::set_global(pointer_t global)
{
    f_global = global;
}


lpp__context::pointer_t lpp__context::get_global()
{
    return f_global.lock();
}


lpp__thing::pointer_t lpp__context::find_thing(std::string const & name) const
{
    lpp__context::const_pointer_t context(shared_from_this());
    while(context != nullptr)
    {
        auto it(context->f_things.find(name));
        if(it != context->f_things.end())
        {
            return it->second;
        }
        context = context->f_parent;
    }

    return lpp__thing::pointer_t();
}


lpp__thing::pointer_t lpp__context::get_thing(std::string const & name) const
{
    lpp__thing::pointer_t thing(find_thing(name));
    if(thing == nullptr)
    {
        throw lpp__error(shared_from_this()
                       , lpp__error_code_t::ERROR_CODE_VARIABLE_NOT_SET
                       , "error"
                       , "thing named \"" + name + " not found.");
    }
    return thing;
}


void lpp__context::set_thing(std::string const & name, lpp__value::pointer_t value, lpp__thing_type_t type)
{
    switch(type)
    {
    case lpp__thing_type_t::LPP__THING_TYPE_DEFAULT:
        {
            lpp__thing::pointer_t thing(find_thing(name));
            if(thing == nullptr)
            {
                thing = std::make_shared<lpp__thing>();
                thing->set_value(value, lpp__thing_type_t::LPP__THING_TYPE_GLOBAL);
                auto global(f_global.lock());
                global->f_things[name] = thing;
            }
            else
            {
                thing->set_value(value);
            }
        }
        break;

    case lpp__thing_type_t::LPP__THING_TYPE_CONTEXT:
        {
            auto it(f_things.find(name));
            if(it == f_things.end())
            {
                lpp__thing::pointer_t thing(std::make_shared<lpp__thing>());
                thing->set_value(value, lpp__thing_type_t::LPP__THING_TYPE_CONTEXT);
                f_things[name] = thing;
            }
            else
            {
                it->second->set_value(value);
            }
        }
        break;

    case lpp__thing_type_t::LPP__THING_TYPE_PROCEDURE:
        {
            auto it(f_things.find(name));
            if(it == f_things.end())
            {
                lpp__thing::pointer_t thing(std::make_shared<lpp__thing>());
                thing->set_value(value, lpp__thing_type_t::LPP__THING_TYPE_PROCEDURE);

                // make sure to save it in a procedure context, not primitive
                //
                lpp__context::pointer_t context(shared_from_this());
                while(context != nullptr)
                {
                    if(!context->f_primitive)
                    {
                        context->f_things[name] = thing;
                        return;
                    }

                    context = context->f_parent;
                }

                throw std::logic_error("could not set thing \""
                                     + name
                                     + " in a procedure context.");
            }
            else
            {
                it->second->set_value(value);
            }
        }
        break;

    case lpp__thing_type_t::LPP__THING_TYPE_GLOBAL:
        {
            auto global(f_global.lock());
            auto it(global->f_things.find(name));
            if(it == global->f_things.end())
            {
                lpp__thing::pointer_t thing(std::make_shared<lpp__thing>());
                thing->set_value(value, lpp__thing_type_t::LPP__THING_TYPE_GLOBAL);
                global->f_things[name] = thing;
            }
            else
            {
                it->second->set_value(value);
            }
        }
        break;

    }
}


bool lpp__context::has_returned_value() const
{
    return f_return_value != nullptr;
}


lpp__value::pointer_t lpp__context::get_returned_value() const
{
    if(f_return_value == nullptr)
    {
        throw lpp__error(shared_from_this()
                       , lpp__error_code_t::ERROR_CODE_OUTPUT_EXPECTED
                       , "error"
                       , "nothing was returned.");
    }

    return f_return_value;
}


void lpp__context::set_return_value(lpp__value::pointer_t value)
{
    f_return_value = value;
}


void lpp__context::set_error(lpp__error const & e) const
{
    lpp__context::const_pointer_t context(shared_from_this());
    while(context != nullptr)
    {
        if(!context->f_primitive)
        {
            context->f_last_error = e;
            return;
        }

        context = context->f_parent;
    }
}


lpp__error const & lpp__context::get_error() const
{
    lpp__context::const_pointer_t context(shared_from_this());
    while(context != nullptr)
    {
        if(!context->f_primitive)
        {
            // for error a sub-procedure cannot see an error of a
            // super-procedure
            //
            return context->f_last_error;
        }

        context = context->f_parent;
    }

    // this one will be empty but that way we can return a reference
    //
    return f_last_error;
}


void lpp__context::add_repeat_count(lpp__integer_t count)
{
    f_repeat_count.push_back(count + 1); // logo expect counter to be 1, 2, 3 ...
}


void lpp__context::remove_last_repeat_count()
{
    if(f_repeat_count.empty())
    {
        throw std::logic_error("this context has no repeat count to remove.");
    }

    f_repeat_count.pop_back();
}


lpp__integer_t lpp__context::get_repeat_count() const
{
    lpp__context::const_pointer_t context(shared_from_this());
    while(context != nullptr)
    {
        if(!context->f_repeat_count.empty())
        {
            return context->f_repeat_count.back();
        }

        context = context->f_parent;
    }

    throw lpp__error(shared_from_this()
                   , lpp__error_code_t::ERROR_CODE_VARIABLE_NOT_SET
                   , "error"
                   , "no repeat count found.");
}


void lpp__context::set_test(test_t value)
{
    if(value == test_t::TEST_UNDEFINED)
    {
        throw std::logic_error("set_test() called with TEST_UNDEFINED.");
    }

    lpp__context::pointer_t context(shared_from_this());
    while(context != nullptr)
    {
        if(!context->f_primitive)
        {
            context->f_test = value;
            return;
        }

        context = context->f_parent;
    }

    throw std::logic_error("set_test() no procedure found.");
}


test_t lpp__context::get_test() const
{
    lpp__context::const_pointer_t context(shared_from_this());
    while(context != nullptr)
    {
        if(context->f_test != test_t::TEST_UNDEFINED)
        {
            return context->f_test;
        }

        context = context->f_parent;
    }

    throw lpp__error(shared_from_this()
                   , lpp__error_code_t::ERROR_CODE_IFTRUE_IFFALSE_WITHOUT_TEST
                   , "error"
                   , "TEST never called, you cannot use IFTRUE or IFFALSE.");
}


void lpp__context::attach(pointer_t parent)
{
    if(f_parent != nullptr)
    {
        throw std::logic_error("this context is being attached more than once?");
    }

    f_parent = parent;
    f_global = parent->f_global;
}




lpp__raii_repeat_count::lpp__raii_repeat_count(lpp__context::pointer_t context, lpp__integer_t count)
    : f_context(context)
{
    f_context->add_repeat_count(count);
}


lpp__raii_repeat_count::~lpp__raii_repeat_count()
{
    f_context->remove_last_repeat_count();
}


} // lpp namespace


std::ostream & operator << (std::ostream & out, lpp::lpp__value_type_t value_type)
{
    switch(value_type)
    {
    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_SPECIAL:
        out << "LPP__VALUE_TYPE_SPECIAL";
        break;

    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_BOOLEAN:
        out << "_VALUE_TYPE_BOOLEAN";
        break;

    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_INTEGER:
        out << "LPP__VALUE_TYPE_INTEGER";
        break;

    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_FLOAT:
        out << "LPP__VALUE_TYPE_FLOAT";
        break;

    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_WORD:
        out << "LPP__VALUE_TYPE_WORD";
        break;

    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_LIST:
        out << "LPP__VALUE_TYPE_LIST";
        break;

    }

    return out;
}


// vim: ts=4 sw=4 et nocindent
