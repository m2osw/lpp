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

##
## Logo to C++ compiler
##
project(lpp)

# Put the version in the header file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/version.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/version.hpp
)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(
    ${PROJECT_NAME}
        lpp.cpp

        compiler.cpp
        control_primitives.cpp
        exception.cpp
        inline_primitives.cpp
        lexer.cpp
        parser.cpp
        token.cpp
)


target_link_libraries(${PROJECT_NAME}
)


install(
    TARGETS
        ${PROJECT_NAME}

    RUNTIME DESTINATION
        bin
)


# vim: ts=4 sw=4 et nocindent
