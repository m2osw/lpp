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


project(lpp-doc)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.doxy.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.doxy @ONLY
)

set(DOCUMENTATION_OUTPUT ${PROJECT_NAME}-${LPP_VERSION_MAJOR}.${LPP_VERSION_MINOR})

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/${DOCUMENTATION_OUTPUT}.tar.gz
        ${CMAKE_CURRENT_BINARY_DIR}/${DOCUMENTATION_OUTPUT}
    COMMAND
        doxygen ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.doxy
                            1> ${PROJECT_NAME}-doxy.log
                            2> ${PROJECT_NAME}-doxy.err
    COMMAND
        echo Compacting as ${DOCUMENTATION_OUTPUT}.tar.gz
    COMMAND
        rm -rf ${DOCUMENTATION_OUTPUT}*
    COMMAND
        mv html ${DOCUMENTATION_OUTPUT}
    COMMAND
        tar czf ${DOCUMENTATION_OUTPUT}.tar.gz ${DOCUMENTATION_OUTPUT}
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}
)


add_custom_target(
    ${PROJECT_NAME} ALL
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/${DOCUMENTATION_OUTPUT}.tar.gz
    COMMENT
        "Generate the ${PROJECT_NAME} documentation."
        VERBATIM
)


install(
    DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}/${DOCUMENTATION_OUTPUT}
    DESTINATION
        share/doc/${PROJECT_NAME}/html
)



# vim: ts=4 sw=4 et
