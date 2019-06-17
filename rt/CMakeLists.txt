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
## Logo Runtime Library
##
project(lpprt)

add_library(
    ${PROJECT_NAME} SHARED
        # Utility Classes
        context.cpp
        error.cpp
        number.cpp
        thing.cpp
        value.cpp

        # Primitives
        primitive_arccos.cpp
        primitive_arcsin.cpp
        primitive_arctan.cpp
        primitive_ascii.cpp
        primitive_ashift.cpp
        primitive_beforep.cpp
        primitive_bitand.cpp
        primitive_bitnot.cpp
        primitive_bitor.cpp
        primitive_bitxor.cpp
        primitive_butfirst.cpp
        primitive_butlast.cpp
        primitive_bye.cpp
        primitive_cos.cpp
        primitive_count.cpp
        primitive_difference.cpp
        primitive_emptyp.cpp
        primitive_equalp.cpp
        primitive_error.cpp
        primitive_exp.cpp
        primitive_first.cpp
        primitive_floatp.cpp
        primitive_greaterequalp.cpp
        primitive_greaterp.cpp
        primitive_int.cpp
        primitive_integerp.cpp
        primitive_iseq.cpp
        primitive_item.cpp
        primitive_last.cpp
        primitive_lessequalp.cpp
        primitive_lessp.cpp
        primitive_ln.cpp
        primitive_log.cpp
        primitive_lshift.cpp
        primitive_make.cpp
        primitive_minus.cpp
        primitive_modulo.cpp
        primitive_not.cpp
        primitive_notequalp.cpp
        primitive_numberp.cpp
        primitive_plus.cpp
        primitive_power.cpp
        primitive_print.cpp
        primitive_product.cpp
        primitive_quotient.cpp
        primitive_radarccos.cpp
        primitive_radarcsin.cpp
        primitive_radarctan.cpp
        primitive_radcos.cpp
        primitive_radsin.cpp
        primitive_radtan.cpp
        primitive_random.cpp
        primitive_remainder.cpp
        primitive_repcount.cpp
        primitive_rerandom.cpp
        primitive_round.cpp
        primitive_rseq.cpp
        primitive_show.cpp
        primitive_sin.cpp
        primitive_sqrt.cpp
        primitive_sum.cpp
        primitive_tan.cpp
        primitive_test.cpp
        primitive_type.cpp
        primitive_word.cpp
        primitive_wordp.cpp
)


target_link_libraries(${PROJECT_NAME}
)


install(
    TARGETS
        ${PROJECT_NAME}

    LIBRARY DESTINATION
        lib
)

install(
    FILES
        lpp.hpp

    DESTINATION
        include/lpp
)

install(
    FILES
        main.cpp

    DESTINATION
        lib/lpp
)

# This may be nice at some point, but re-compiling the
# main.cpp is very fast anyway
#install(
#    FILES
#        ${CMAKE_BINARY_PATH}/main.cpp.o
#
#    DESTINATION
#        lib/lpp
#
#    RENAME
#        rt.o
#)


# vim: ts=4 sw=4 et nocindent
