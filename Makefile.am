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
                   include/safequeue.h \
                   include/safemap.h \
                   include/safelist.h \
                   include/benchmark.h


bin_PROGRAMS = bin/base \
                bin/batch

#bin_PROGRAMS = bin/test_1to1 \
#                bin/test_nto1 \
#                bin/zero \
#                bin/base

#bin_test_1to1_SOURCES = src/node.cc \
#                         src/message.cc \
#                         test/test_1to1/nodes.cc \
#                         test/test_1to1/main.cc
#
#bin_test_nto1_SOURCES = src/node.cc \
#                         src/message.cc \
#                         test/test_nto1/nodes.cc \
#                         test/test_nto1/main.cc
#
#bin_zero_SOURCES = src/node.cc \
#                    src/benchmark.cc \
#                    src/message.cc \
#                    src/algo/zero/nodes.cc \
#                    src/algo/zero/main.cc

bin_base_SOURCES = src/node.cc \
                    src/benchmark.cc \
                    src/algo/base/nodes.cc \
                    src/algo/base/main.cc

bin_batch_SOURCES = src/node.cc \
                     src/benchmark.cc \
                     src/algo/batch/nodes.cc \
                     src/algo/batch/main.cc
