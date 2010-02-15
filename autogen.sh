#!/bin/sh

echo "Generating build information using aclocal, automake and autoconf"
echo "This may take a while ..."

# Regenerate configuration files.

OS=$(uname -s)
aclocal
# autoheader
automake --include-deps --add-missing --copy
if test ${OS} = "Darwin"; then
glibtoolize
else
libtoolize
fi
autoconf

echo "Now you are ready to run ./configure."
echo "You can also run  ./configure --help for extra features to enable/disable."
