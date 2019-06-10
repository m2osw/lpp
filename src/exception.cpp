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

#include "exception.hpp"

namespace lpp
{



lpp_error::lpp_error(std::string const & message, std::string const & filename, line_t line)
    : runtime_error("error: "
            + (filename.empty() ? std::string() : "in " + filename)
            + (line == 0 ? std::string() : " on line #" + std::to_string(line))
            + (filename.empty() && line == 0 ? std::string() : std::string(": "))
            + message)
{
}



} // lpp namespace

// vim: ts=4 sw=4 et nocindent
