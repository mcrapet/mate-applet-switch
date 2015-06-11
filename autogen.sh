#!/bin/sh -e
# FIXME: See generic way using `. mate-autogen`

test -d m4 || mkdir m4
autoreconf -fi
rm -Rf autom4te.cache
