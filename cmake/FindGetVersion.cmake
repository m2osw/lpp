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



# Use this function to parse the version from the debian/changelog.
# It will create four variables in the parent scope, each with ${PACKAGE_NAME}_
# prepended.
#
# You must have dpkg-dev installed first!
#
function( GetVersion PACKAGE_NAME WORKING_DIRECTORY )
    execute_process(
        COMMAND head -1 debian/changelog
        COMMAND cut -f 2 -d "("
        COMMAND cut -f 1 -d ")"
        WORKING_DIRECTORY ${WORKING_DIRECTORY}
        OUTPUT_VARIABLE VERSION
    )

    string( REPLACE "\n" ""  ORIG_VERSION ${VERSION}      )
    string( REPLACE "."  " " VERSION      ${ORIG_VERSION} )

    separate_arguments( VERSION )

    list( LENGTH VERSION LEN )
    if( NOT ${LEN} GREATER 1 )
        message( FATAL_ERROR "Package name '${PACKAGE_NAME}' does not have a valid version number: '${ORIG_VERSION}'. It must have at least 2 parts: major.minor.patch.build!" )
    endif()

    list( GET VERSION 0 major_version  )
    list( GET VERSION 1 minor_version  )
    if( ${LEN} GREATER 2 )
        list( GET VERSION 2 patch_version  )
    else()
        set( patch_version 0 )
    endif()
    if( ${LEN} GREATER 3 )
        list( GET VERSION 3 _build_version )
    else()
        set( _build_version 0 )
    endif()

    string( REPLACE "-"  " " temp_ver ${_build_version} )
    list( LENGTH temp_ver temp_ver_len )
    if( ${temp_ver_len} EQUAL 2 )
        #
        # Throw away the debian build number if it is present (e.g. 1.2.3.4-5, where we throw out the '-5').
        #
        list( GET temp_ver 0 build_version )
    else()
        set( build_version ${_build_version} )
    endif()

    set( ${PACKAGE_NAME}_VERSION_MAJOR ${major_version} PARENT_SCOPE )
    set( ${PACKAGE_NAME}_VERSION_MINOR ${minor_version} PARENT_SCOPE )
    set( ${PACKAGE_NAME}_VERSION_PATCH ${patch_version} PARENT_SCOPE )
    set( ${PACKAGE_NAME}_VERSION_BUILD ${build_version} PARENT_SCOPE )

endfunction()

# vim: ts=4 sw=4 expandtab wrap
