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



namespace lpp
{


namespace
{



} // no name namespace




lpp__context::~lpp__context()
{
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
        throw lpp__error("thing named \"" + name + " not found");
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

    case lpp__thing_type_t::LPP__THING_TYPE_LOCAL:
        {
            auto it(f_things.find(name));
            if(it == f_things.end())
            {
                lpp__thing::pointer_t thing(std::make_shared<lpp__thing>());
                thing->set_value(value, lpp__thing_type_t::LPP__THING_TYPE_LOCAL);
                f_things[name] = thing;
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


lpp__value::pointer_t lpp__context::get_returned_value() const
{
    return f_return_value;
}


void lpp__context::set_return_value(lpp__value::pointer_t value)
{
    f_return_value = value;
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



} // lpp namespace
// vim: ts=4 sw=4 et nocindent
