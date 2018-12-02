ACLOCAL_AMFLAGS= -I m4
LDFLAGS_BOOST = \
    $(BOOST_SYSTEM_LDFLAGS) \
    $(BOOST_PROGRAM_OPTIONS_LDFLAGS)

LIBS_BOOST = \
    $(BOOST_SYSTEM_LIBS) \
    $(BOOST_PROGRAM_OPTIONS_LIBS) \
    $(BOOST_FILESYSTEM_LIBS)

AM_CPPFLAGS = $(BOOST_CPPFLAGS)
AM_LDFLAGS = $(LDFLAGS_BOOST)
LIBS += $(LIBS_BOOST)

include_HEADERS = include/lib_include.h \
                   include/node.h \
                   include/macro.h \
                   include/message.h \
                   include/safequeue.h

bin_PROGRAMS = bin/test_1to1 \
                bin/test_nto1 \
                bin/algo1

bin_test_1to1_SOURCES = src/node.cc \
                         src/message.cc \
                         src/test_1to1/nodes.cc \
                         src/test_1to1/main.cc

bin_test_nto1_SOURCES = src/node.cc \
                         src/message.cc \
                         src/test_nto1/nodes.cc \
                         src/test_nto1/main.cc

bin_algo1_SOURCES = src/node.cc \
                     src/message.cc \
                     src/algo1/nodes.cc \
                     src/algo1/main.cc
