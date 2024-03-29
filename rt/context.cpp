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
#include <fstream>
#include <iostream>
#include <sstream>
#include <set>



namespace lpp
{


namespace
{



lpp__thing::map_t       g_properties = lpp__thing::map_t();


typedef std::map<std::string, bool>    trace_t;
trace_t                 g_trace = trace_t();


typedef std::set<lpp__procedure_info_t const *>         procedure_list_t;
typedef std::map<std::string, procedure_list_t>         variable_notification_t;
typedef std::set<std::string>                           processing_notification_t;

variable_notification_t     g_notifications;
procedure_list_t            g_processing_notifications;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
class safe_processing
{
public:
    safe_processing(lpp__procedure_info_t const * proc)
        : f_proc(proc)
    {
        g_processing_notifications.insert(f_proc);
    }

    ~safe_processing()
    {
        g_processing_notifications.erase(f_proc);
    }

private:
    lpp__procedure_info_t const *   f_proc;
};
#pragma GCC diagnostic pop


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


std::string lpp__context::get_primitive_name() const
{
    if(!f_primitive)
    {
        return std::string();
    }
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
    if(thing == nullptr
    || thing->get_value() == nullptr)
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
                if(global == nullptr)
                {
                    throw std::logic_error("could not lock the global pointer.");
                }
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
                        break;
                    }

                    context = context->f_parent;
                }

                if(context == nullptr)
                {
                    throw std::logic_error("could not set thing \""
                                         + name
                                         + "\" in a procedure context.");
                }
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
            if(global == nullptr)
            {
                throw std::logic_error("could not lock the global pointer.");
            }
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

    auto it(g_notifications.find(name));
    if(it == g_notifications.end())
    {
        return;
    }

    // make a copy, just in case one of those was to make a modification
    // to the set while we're going through it
    //
    // TODO: how do we want to handle exceptions? (i.e. is it normal for
    //       a callback not to get called if another earlier one decides
    //       to throw?)
    //
    lpp__value::pointer_t variable_name(std::make_shared<lpp::lpp__value>(name));
    procedure_list_t procedures(it->second);
    for(auto proc : procedures)
    {
        if(g_processing_notifications.find(proc) != g_processing_notifications.end())
        {
            // TODO: generate a loop warning
            continue;
        }

        safe_processing sp(proc);

        lpp__context::pointer_t sub_context(std::make_shared<lpp__context>(std::string(), proc->f_name, 0, false));
        sub_context->set_thing("variablename", variable_name, lpp__thing_type_t::LPP__THING_TYPE_CONTEXT);
        lpp::lpp__value::vector_t rest;
        sub_context->set_thing("rest", std::make_shared<lpp::lpp__value>(rest), lpp__thing_type_t::LPP__THING_TYPE_CONTEXT);
        sub_context->attach(shared_from_this());
        (*proc->f_procedure)(sub_context);
    }
}


void lpp__context::erase_thing(std::string const & name)
{
    // erase global variables
    //
    auto global(f_global.lock());
    if(global == nullptr)
    {
        throw std::logic_error("could not lock the global pointer.");
    }
    auto git(global->f_things.find(name));
    if(git != global->f_things.end())
    {
        global->f_things.erase(git);
    }

    // erase properties
    //
    auto pit(g_properties.find(name));
    if(pit != g_properties.end())
    {
        g_properties.erase(pit);
    }
}


lpp__value::pointer_t lpp__context::get_all_thing_names() const
{
    // use a set so that way we return each name uniquely
    //
    std::set<std::string> s;

    lpp__context::const_pointer_t context(shared_from_this());
    while(context != nullptr)
    {
        for(auto const & thing : context->f_things)
        {
            s.insert(thing.first);
        }

        context = context->f_parent;
    }

    // now convert our set in a vector of names
    //
    lpp__value::vector_t names;
    names.reserve(s.size());
    for(auto const & n : s)
    {
        names.push_back(std::make_shared<lpp__value>(n));
    }

    return std::make_shared<lpp__value>(names);
}


lpp__thing::pointer_t lpp__context::find_property(std::string const & listname) const
{
    auto const & it(g_properties.find(listname));
    if(it != g_properties.end())
    {
        return it->second;
    }

    return lpp__thing::pointer_t();
}


void lpp__context::set_property(
      std::string const & listname
    , std::string const & propname
    , lpp__value::pointer_t value)
{
    lpp__thing::pointer_t thing(find_property(listname));
    if(thing == nullptr)
    {
        if(value == nullptr)
        {
            // do not create if value is nullptr
            //
            return;
        }

        thing = std::make_shared<lpp__thing>();
        lpp__value::pointer_t new_prop(std::make_shared<lpp__value>());
        lpp__value::map_t prop;
        new_prop->set_prop(prop);
        thing->set_value(new_prop);
        g_properties[listname] = thing;
    }

    lpp__value::pointer_t prop_value(thing->get_value());
    if(prop_value == nullptr)
    {
        throw std::logic_error("somehow the prop_value is nullptr.");
    }
    if(value == nullptr)
    {
        if(prop_value != nullptr)
        {
            lpp__value::map_t prop(prop_value->get_prop());
            auto const & it(prop.find(propname));
            if(it != prop.end())
            {
                prop.erase(it);
                if(prop.empty())
                {
                    // once the property is completely empty, remove it
                    // from the property list
                    //
                    auto const & itrem(g_properties.find(listname));
                    g_properties.erase(itrem);
                }
                else
                {
                    prop_value->set_prop(prop);
                }
            }
        }
    }
    else
    {
        lpp__value::map_t prop;
        prop = prop_value->get_prop();
        prop[propname] = value;
        prop_value->set_prop(prop);
    }
}


lpp__value::pointer_t lpp__context::list_properties() const
{
    lpp__value::vector_t result;

    for(auto const & p : g_properties)
    {
        result.push_back(std::make_shared<lpp__value>(p.first));
    }

    return std::make_shared<lpp__value>(result);
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


void lpp__context::end_of_function_reached()
{
    throw lpp__error(shared_from_this()
                   , lpp::lpp__error_code_t::ERROR_CODE_OUTPUT_EXPECTED
                   , "error"
                   , "function must return, end of procedure reached instead.");
}


void lpp__context::set_trace(std::string const & name, bool trace)
{
    if(trace)
    {
        g_trace[name] = true;
    }
    else
    {
        auto it(g_trace.find(name));
        if(it != g_trace.end())
        {
            g_trace.erase(it);
        }
    }
}


bool lpp__context::is_traced(std::string const & name) const
{
    return g_trace.find(name) != g_trace.end();
}


void lpp__context::trace_procedure(trace_mode_t action, lpp__value::pointer_t data)
{
    // is this procedure being traced?
    //
    if(!is_traced(f_procedure))
    {
        return;
    }

    std::stringstream ss;
    switch(action)
    {
    case trace_mode_t::TRACE_MODE_ENTER:
        {
            ss << "CALL \""
               << f_procedure;
            if(!f_things.empty())
            {
                ss << " [";
                bool first(true);
                for(auto t : f_things)
                {
                    if(!first)
                    {
                        ss << " ";
                    }
                    else
                    {
                        first = false;
                    }
                    ss << "[:"
                       << t.first
                       << " "
                       << t.second->get_value()->to_string(DISPLAY_FLAG_BACKSLASHED | DISPLAY_FLAG_TYPED)
                       << "]";
                }
                ss << "]";
            }
        }
        break;

    case trace_mode_t::TRACE_MODE_EXIT:
        ss << "RETURN \""
           << f_procedure
           << " with END";
        break;

    case trace_mode_t::TRACE_MODE_STOP:
        ss << "RETURN \""
           << f_procedure
           << " with STOP";
        break;

    case trace_mode_t::TRACE_MODE_OUTPUT:
        ss << "RETURN \""
           << f_procedure
           << " with OUTPUT "
           << data->to_string(DISPLAY_FLAG_BACKSLASHED | DISPLAY_FLAG_TYPED);
        break;

    }

    // output to terminal
    //
    ss << std::endl;
    lpp__write_file(shared_from_this(), std::string(), ss.str());
}


void lpp__context::set_notify(std::string const & variable_name, std::string const & procedure_name)
{
    // name of procedure to call whenever a given variable changes
    //
    lpp__procedure_info_t const * info(find_procedure(procedure_name));
    if(info == nullptr)
    {
        throw lpp__error(shared_from_this()
                       , lpp__error_code_t::ERROR_CODE_UNKNOWN_PROCEDURE
                       , "error"
                       , "procedure named \"" + procedure_name + "\" not found.");
    }

    auto it(g_notifications.find(variable_name));
    if(it == g_notifications.end())
    {
        procedure_list_t list{info};
        g_notifications[variable_name] = list;
    }
    else
    {
        it->second.insert(info);
    }
}


void lpp__context::remove_notify(std::string const & variable_name, std::string const & procedure_name)
{
    auto it(g_notifications.find(variable_name));
    if(it != g_notifications.end())
    {
        if(procedure_name.empty())
        {
            g_notifications.erase(it);
        }
        else
        {
            lpp__procedure_info_t const * info(find_procedure(procedure_name));
            if(info == nullptr)
            {
                throw lpp__error(shared_from_this()
                               , lpp__error_code_t::ERROR_CODE_UNKNOWN_PROCEDURE
                               , "error"
                               , "procedure named \"" + procedure_name + "\" not found.");
            }

            auto proc(it->second.find(info));
            if(proc != it->second.end())
            {
                it->second.erase(proc);
                if(it->second.empty())
                {
                    g_notifications.erase(it);
                }
            }
        }
    }
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

    case lpp::lpp__value_type_t::LPP__VALUE_TYPE_PROPERTY_LIST:
        out << "LPP__VALUE_TYPE_PROPERTY_LIST";
        break;

    }

    return out;
}


// vim: ts=4 sw=4 et nocindent
