#!/bin/sh -e
#
# Execute make

MODE=Debug
if test "$1" = "-r"
then
	MODE=Release
	shift
fi

case "$1" in
"-l")
	make -C ../BUILD/${MODE} 2>&1 | less -R
	;;

"-i")
	make -j4 -C ../BUILD/${MODE} install
	;;

"-d")
	rm -rf ../BUILD/${MODE}/doc/lpp-doc-?.*
	make -C ../BUILD/${MODE}
	;;

"-t")
	echo "-- Building..."
	make -j4 -C ../BUILD/${MODE}
	echo "-- Installing..."
	make -C ../BUILD/${MODE} install >../BUILD/${MODE}/install.log

	# The -rpath is just as a developer, when lpp is installed
	# we can just access the library under /usr/lib so it would
	# not help
	#
	echo "-- lpp compiling..."
	../BUILD/${MODE}/dist/bin/lpp \
		-v \
		-g \
		--keep-l-cpp \
		--main-cpp "../BUILD/${MODE}/dist/lib/lpp/main.cpp" \
		--rpath "`cd ../BUILD/${MODE}/dist/lib; pwd`" \
		-I ../BUILD/${MODE}/dist/include \
		-L ../BUILD/${MODE}/dist/lib \
		tests/suite/syntax-print.logo
	;;

*)
	make -j4 -C ../BUILD/${MODE}
	;;

esac


# gdb ../BUILD/Debug/dist/bin/lpp
# run -v --main-cpp "../BUILD/Debug/dist/lib/lpp/main.cpp" --rpath "`cd ../BUILD/Debug/dist/lib; pwd`" -I ../BUILD/Debug/dist/include -L ../BUILD/Debug/dist/lib tests/suite/syntax-print.logo

# From the https://github.com/m2osw/lpp project
