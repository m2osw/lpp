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

# Note that if the user does not have Catch2 then this does not get included
# in the final output
project(unittest)

message("Tests not built yet... (Catch.cmake missing)")
#find_package( Catch )
#if( CATCH_FOUND )
#
#    add_definitions(
#        -DCATCH_CONFIG_PREFIX_ALL
#    )
#
#    include_directories(
#        ${CMAKE_BINARY_DIR}
#        ${PROJECT_SOURCE_DIR}
#        ${CATCH_INCLUDE_DIR}
#        ${LIBEXCEPT_INCLUDE_DIRS}
#    )
#
#    add_executable( ${PROJECT_NAME}
#        main.cpp
#
#        lexer.cpp
#        parser.cpp
#        token.cpp
#    )
#
#    set( TMPDIR "${CMAKE_BINARY_DIR}/tmp" )
#    if( NOT EXISTS ${TMPDIR} )
#        file( MAKE_DIRECTORY ${TMPDIR} )
#    endif()
#
#    add_test( unittests ${PROJECT_NAME} --tmp ${TMPDIR} )
#
#endif( CATCH_FOUND )

# vim: ts=4 sw=4 et
