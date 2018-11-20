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

include_HEADERS = include/lib_includes.h \
                   include/node.h \
                   include/macros.h \
                   include/message.h

bin_PROGRAMS = bin/node_test
bin_node_test_SOURCES = src/node.cc \
                         src/node_test.cc

#bin_PROGRAMS = bin/node_test bin/basic_test
#bin_basic_test_SOURCES = src/node.cc \
#                          src/basic/basic.h \
#                          src/basic/usernode.h \
#                          src/basic/usernode.cc \
#                          src/basic/gatenode.h \
#                          src/basic/gatenode.cc \
#                          src/basic/basic_test.cc

