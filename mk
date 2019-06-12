#!/bin/sh -e
#
# Execute make

case "$1" in
"-l")
	make -C ../BUILD/Debug 2>&1 | less -R
	;;

"-r")
	make -j4 -C ../BUILD/Release
	;;

"-d")
	rm -rf ../BUILD/Debug/doc/lpp-doc-?.*.tar.gz \
	       ../BUILD/Debug/doc/lpp-doc-?.*
	make -C ../BUILD/Debug
	;;

"-t")
	echo "-- Building..."
	make -C ../BUILD/Debug
	echo "-- lpp compiling..."
	../BUILD/Debug/src/lpp tests/suite/syntax-print.logo
	echo "-- g++ compiling..."
	g++ -std=c++14 -I rt l.cpp rt/*.cpp
	;;

*)
	make -j4 -C ../BUILD/Debug
	;;

esac


# From the https://github.com/m2osw/lpp project
