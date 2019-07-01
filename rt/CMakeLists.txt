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
        file.cpp
        number.cpp
        procedures.cpp
        thing.cpp
        ../common/utf8_iterator.cpp
        value.cpp

        # Primitives
        primitive_allopen.cpp
        primitive_and.cpp
        primitive_arccos.cpp
        primitive_arcsin.cpp
        primitive_arctan.cpp
        primitive_arity.cpp
        primitive_ashift.cpp
        primitive_backslashedp.cpp
        primitive_beforep.cpp
        primitive_bitand.cpp
        primitive_bitnot.cpp
        primitive_bitor.cpp
        primitive_bitxor.cpp
        primitive_butfirst.cpp
        primitive_butlast.cpp
        primitive_bye.cpp
        primitive_byte.cpp
        primitive_char.cpp
        primitive_cleartext.cpp
        primitive_close.cpp
        primitive_closeall.cpp
        primitive_combine.cpp
        primitive_comparablep.cpp
        primitive_cos.cpp
        primitive_count.cpp
        primitive_cursor.cpp
        primitive_difference.cpp
        primitive_definedp.cpp
        primitive_dequeue.cpp
        primitive_emptyp.cpp
        primitive_eofp.cpp
        primitive_equalp.cpp
        primitive_erase.cpp
        primitive_erasefile.cpp
        primitive_error.cpp
        primitive_errorp.cpp
        primitive_exp.cpp
        primitive_fileinfo.cpp
        primitive_filep.cpp
        primitive_first.cpp
        primitive_firstbyte.cpp
        primitive_floatp.cpp
        primitive_fput.cpp
        primitive_gensym.cpp
        primitive_gprop.cpp
        primitive_greaterequalp.cpp
        primitive_greaterp.cpp
        primitive_int.cpp
        primitive_integerp.cpp
        primitive_iseq.cpp
        primitive_item.cpp
        primitive_last.cpp
        primitive_lastbyte.cpp
        primitive_lessequalp.cpp
        primitive_lessp.cpp
        primitive_list.cpp
        primitive_listp.cpp
        primitive_ln.cpp
        primitive_local.cpp
        primitive_localmake.cpp
        primitive_log.cpp
        primitive_lowercase.cpp
        primitive_lput.cpp
        primitive_lshift.cpp
        primitive_make.cpp
        primitive_mditem.cpp
        primitive_mdsetitem.cpp
        primitive_member.cpp
        primitive_memberp.cpp
        primitive_minus.cpp
        primitive_modulo.cpp
        primitive_namep.cpp
        primitive_names.cpp
        primitive_nanp.cpp
        primitive_not.cpp
        primitive_notequalp.cpp
        primitive_notify.cpp
        primitive_numberp.cpp
        primitive_openappend.cpp
        primitive_openread.cpp
        primitive_openupdate.cpp
        primitive_openwrite.cpp
        primitive_or.cpp
        primitive_pick.cpp
        primitive_plus.cpp
        primitive_plist.cpp
        primitive_plistp.cpp
        primitive_plists.cpp
        primitive_prefix.cpp
        primitive_pop.cpp
        primitive_power.cpp
        primitive_pprop.cpp
        primitive_primitivep.cpp
        primitive_primitives.cpp
        primitive_print.cpp
        primitive_procedurep.cpp
        primitive_procedures.cpp
        primitive_product.cpp
        primitive_push.cpp
        primitive_queue.cpp
        primitive_quoted.cpp
        primitive_quotient.cpp
        primitive_radarccos.cpp
        primitive_radarcsin.cpp
        primitive_radarctan.cpp
        primitive_radcos.cpp
        primitive_radsin.cpp
        primitive_radtan.cpp
        primitive_random.cpp
        primitive_readbyte.cpp
        primitive_readbytes.cpp
        primitive_readchar.cpp
        primitive_readchars.cpp
        primitive_reader.cpp
        primitive_readlist.cpp
        primitive_readpos.cpp
        primitive_readrawline.cpp
        primitive_readword.cpp
        primitive_remainder.cpp
        primitive_remdup.cpp
        primitive_remove.cpp
        primitive_remprop.cpp
        primitive_repcount.cpp
        primitive_rerandom.cpp
        primitive_reverse.cpp
        primitive_round.cpp
        primitive_rseq.cpp
        primitive_sentence.cpp
        primitive_setcursor.cpp
        primitive_setitem.cpp
        primitive_setprefix.cpp
        primitive_setread.cpp
        primitive_setreadpos.cpp
        primitive_settextcolor.cpp
        primitive_setwrite.cpp
        primitive_setwritepos.cpp
        primitive_shell.cpp
        primitive_show.cpp
        primitive_sin.cpp
        primitive_sqrt.cpp
        primitive_standout.cpp
        primitive_substringp.cpp
        primitive_sum.cpp
        primitive_tan.cpp
        primitive_test.cpp
        primitive_time.cpp
        primitive_trace.cpp
        primitive_tracedp.cpp
        primitive_ttyp.cpp
        primitive_type.cpp
        primitive_unicode.cpp
        primitive_unnotify.cpp
        primitive_unorderedp.cpp
        primitive_untrace.cpp
        primitive_uppercase.cpp
        primitive_word.cpp
        primitive_wordp.cpp
        primitive_write.cpp
        primitive_writepos.cpp
        primitive_writer.cpp
        primitive_xor.cpp
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
