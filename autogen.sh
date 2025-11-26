#!/bin/sh

# Detect the system type to choose the correct libtoolize command.
# - On Darwin (macOS), Homebrew-installed GNU libtool uses 'glibtoolize'.
# - On Linux and FreeBSD, the standard command is 'libtoolize'.
if [ "$(uname -s)" = "Darwin" ]; then
  LIBTOOLIZE=glibtoolize
else
  LIBTOOLIZE=libtoolize
fi

# Check if the required libtoolize command is available.
if ! command -v "$LIBTOOLIZE" &> /dev/null; then
  echo "Error: $LIBTOOLIZE not found. Please install libtool first."
  echo "macOS installation: brew install libtool"
  echo "Linux (Debian/Ubuntu) installation: sudo apt-get install libtool"
  echo "FreeBSD installation: sudo pkg install libtool"
  exit 1
fi

# Standard sequence of commands to generate autotools configuration files.
aclocal         # Process macro definitions and generate aclocal.m4.
autoheader      # Generate config.h.in from configure.ac.
$LIBTOOLIZE --force  # Prepare libtool infrastructure (overwrite existing files).
autoconf -f -i  # Generate the configure script from configure.ac.
automake --add-missing --force  # Generate Makefile.in from Makefile.am, adding missing files.

