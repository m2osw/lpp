#!/bin/sh
#
# Execute make


case "$1" in
"-l")
	make -C ../BUILD/Debug 2>&1 | less -R
	;;

"-r")
	make -C ../BUILD/Release
	;;

"-d")
	rm -rf ../BUILD/Debug/doc/lpp-doc-?.*.tar.gz \
	       ../BUILD/Debug/doc/lpp-doc-?.*
	make -C ../BUILD/Debug
	;;

*)
	make -C ../BUILD/Debug
	;;

esac


