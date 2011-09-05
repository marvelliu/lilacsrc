#!/bin/sh
aclocal; libtoolize; autoheader; automake -a; autoconf
cd sshbbsd
./autogen.sh
cd ..
