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
	make -j4 -C ../BUILD/Debug
	echo "-- Installing..."
	make -C ../BUILD/Debug install >../BUILD/Debug/install.log
	echo "-- lpp compiling..."
	../BUILD/Debug/dist/bin/lpp tests/suite/syntax-print.logo
	echo "-- g++ compiling..."

	# The -rpath is just as a developer, when lpp is installed
	# we can just access the library under /usr/lib so it would
	# not help
	#
	g++ -std=c++14 \
		-g \
		-O0 \
		-I ../BUILD/Debug/dist/include \
		l.cpp \
		../BUILD/Debug/dist/lib/lpp/main.cpp \
		-Xlinker -rpath \
		-Xlinker `cd ../BUILD/Debug/dist/lib; pwd` \
		-L ../BUILD/Debug/dist/lib \
		-llpprt
	;;

*)
	make -j4 -C ../BUILD/Debug
	;;

esac


# From the https://github.com/m2osw/lpp project
