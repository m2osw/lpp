#!/bin/sh

DIR=`pwd`
BASENAME=`basename $DIR`
if test "$BASENAME" != "lpp"
then
    echo "error: you are expected to run this script from within the lpp source directory."
    exit 1
fi

if test "$1" = "-h" -o "$1" = "--help"
then
    echo
    echo "Usage: $0"
    echo
    exit 1
fi

MODULE=lpp
TMP=lpp/tmp

# Verify that version is 4 numbers separated by 3 periods
# The "~xenial" part has to be updated for each platform
# (add (xenial|<another>|<another>|...) to support multiple versions.)
#
VERSION=`dpkg-parsechangelog -S version`
VALID=`echo "$VERSION" | sed -e 's/[0-9]\+\(\.[0-9]\+\)\+~xenial//'`
if test -n "$VALID"
then
    echo "error: invalid version \"$VERSION\": must be 2 or more numbers separated by periods."
    exit 1
fi

debuild -S -sa

# To send the source to Launchpad, we need to be at the same level as those
# files
cd ..

echo dput ppa:alexis-m2osw/lpp ${MODULE}_${VERSION}_source.changes

# Get rid of those files from our source tree
mkdir -p $TMP/sources
mv ${MODULE}_${VERSION}* $TMP/sources/.

# vim: ts=4 sw=4 et
