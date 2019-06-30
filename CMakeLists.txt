# lpp -- Logo to binary compiler
# Copyright (C) 2019  Made to Order Software Corporation
# https://www.m2osw.com/lpp
# contact@m2osw.com
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.5.1)

project(lpp_compiler)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

find_package(GetVersion REQUIRED)
#find_package(Boost REQUIRED)

GetVersion( LPP ${CMAKE_CURRENT_SOURCE_DIR} )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fdiagnostics-color -g -O0 -Werror -Wall -Wextra -pedantic -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Winit-self -Wlogical-op -Wmissing-include-dirs -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wundef -Wno-unused -Wunused-variable -Wno-variadic-macros -Wno-parentheses -Wno-unknown-pragmas -Wwrite-strings -Wswitch -Wunused-parameter -Wfloat-equal -Wnon-virtual-dtor -Weffc++ -fdiagnostics-show-option")

include_directories(
    ${PROJECT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

#add_subdirectory( cmake     )
add_subdirectory( src       )
add_subdirectory( rt        )
add_subdirectory( doc       )
add_subdirectory( tests     )

# vim: ts=4 sw=4 et nocindent
