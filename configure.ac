#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(pySchedule, 0.1, [pyyaoer@gmail.com])
AC_CONFIG_SRCDIR([src/node.cc])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Check for baseline language coverage in the compiler for the C++11 standard.
AC_PROG_LIBTOOL
AC_CONFIG_MACRO_DIR(m4)
AX_CXX_COMPILE_STDCXX_17([noext], [mandatory])
#AX_CXX_COMPILE_STDCXX_11([noext], [mandatory])

# Checks for libraries.
#AC_LANG_PUSH([C++])
#AC_CHECK_HEADERS([boost/thread.hpp], [],
#    [AC_MSG_ERROR(You need the Boost libraries.)])
#AC_LANG_POP([C++])
#
BOOST_REQUIRE([1.59])
BOOST_SYSTEM
BOOST_THREAD
BOOST_SERIALIZATION

LIBS="-lboost_system -lpthread -lboost_thread -lboost_serialization"
#LIBS="-lboost_system -lboost_thread -lpthread"

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
