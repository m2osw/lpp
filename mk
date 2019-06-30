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

	# The -rpath is just as a developer, when lpp is installed
	# we can just access the library under /usr/lib so it would
	# not help
	#
	echo "-- lpp compiling..."
	../BUILD/Debug/dist/bin/lpp \
		-v \
		--keep-l-cpp \
		--main-cpp "../BUILD/Debug/dist/lib/lpp/main.cpp" \
		--rpath "`cd ../BUILD/Debug/dist/lib; pwd`" \
		-I ../BUILD/Debug/dist/include \
		-L ../BUILD/Debug/dist/lib \
		tests/suite/syntax-print.logo
	;;

*)
	make -j4 -C ../BUILD/Debug
	;;

esac


# gdb ../BUILD/Debug/dist/bin/lpp
# run -v --main-cpp "../BUILD/Debug/dist/lib/lpp/main.cpp" --rpath "`cd ../BUILD/Debug/dist/lib; pwd`" -I ../BUILD/Debug/dist/include -L ../BUILD/Debug/dist/lib tests/suite/syntax-print.logo

# From the https://github.com/m2osw/lpp project
