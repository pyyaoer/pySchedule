#!/bin/sh

quit=0

(autoheader --version) < /dev/null > /dev/null 2>&1 || { echo "You must have autoheader install"; quit=1; }
(autoconf --version) < /dev/null > /dev/null 2>&1 || { echo "You must have autoconf install"; quit=1; }
(automake --version) < /dev/null > /dev/null 2>&1 || { echo "You must have automake install"; quit=1; }

if test "$quit" -eq 1; then
    exit 1
fi

rm -rf autom4te.cache

aclocal
autoheader
automake --force-missing --add-missing
autoconf
