#
#  A part of Lorica.
#  Copyright (C) 2007-2009 OMC Denmark ApS.
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Affero General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU Affero General Public License for more details.
#
#  You should have received a copy of the GNU Affero General Public License
#  along with this program. If not, see <http://www.gnu.org/licenses/>.

##################################################################
CWD = $(CURDIR)

if HAVE_ACE_ROOT
LORICA_ROOT = $(LORICA_top_dir)
ACE_ROOT = $(CONF_ACE_ROOT)
TAO_ROOT = $(ACE_ROOT)/TAO
PATH = $(LORICA_ROOT)/bin:$(ACE_ROOT)/bin:$(ENV_PATH)

if LORICA_DARWIN
export DYLD_LIBRARY_PATH = $(ACE_ROOT)/lib:$(ENV_DYLD_LIBRARY_PATH)
else
export LD_LIBRARY_PATH = $(ACE_ROOT)/lib:$(ENV_LD_LIBRARY_PATH)
endif

TAO_IDL = $(ACE_ROOT)/TAO/TAO_IDL/tao_idl
GPERF = $(ACE_ROOT)/apps/gperf/src/ace_gperf

else # !HAVE_ACE_ROOT

TAO_IDL = tao_idl
GPERF = ace_gperf

endif # HAVE_ACE_ROOT

include $(CONF_ACE_ROOT)/include/makeinclude/macros.GNU
include $(CONF_ACE_ROOT)/include/makeinclude/wrapper_macros.GNU

TAO_IDLFLAGS = -I$(TAO_ROOT) -g $(GPERF)

if LORICA_DARWIN
TESTS_ENVIRONMENT = env DYLD_LIBRARY_PATH=$(ACE_ROOT)/lib:$(ENV_DYLD_LIBRARY_PATH) LORICA_ROOT=$(LORICA_top_dir) TAO_ROOT=$(TAO_ROOT)
else
TESTS_ENVIRONMENT = env LD_LIBRARY_PATH=$(ACE_ROOT)/lib:$(ENV_DYLD_LIBRARY_PATH) LORICA_ROOT=$(LORICA_top_dir) TAO_ROOT=$(TAO_ROOT)
endif
TESTS = run_test.pl

##################################################################

#
# IDL
#
BUILT_SOURCES = \
	testC.cpp \
	testC.h   \
	testC.inl \
	testS.cpp \
	testS.h   \
	testS.inl

CLEANFILES = \
	test-stamp \
	testC.cpp  \
	testC.h    \
	testC.inl  \
	testS.cpp  \
	testS.h    \
	testS.inl

testC.cpp testC.h testC.inl testS.cpp testS.h testS.inl: test-stamp

test-stamp: $(srcdir)/test.idl
	$(TAO_IDL) $(TAO_IDLFLAGS) $(srcdir)/test.idl
	@touch "$@"

noinst_HEADERS = \
	test.idl    \
	test.conf   \
	basic.mpc   \
	run_test.pl 

#
# Client
#
noinst_PROGRAMS = client

client_CPPFLAGS = \
	$(LORICA_CXXFLAGS) \
	$(LORICA_CPPFLAGS)

client_SOURCES = \
	client.cpp \
	testC.cpp  \
	testC.h    \
	testC.inl  \
	testS.h    \
	testS.inl

client_LDADD = \
	-lTAO_DynamicInterface \
	-lTAO_Messaging        \
	-lTAO_PI               \
	-lTAO_CodecFactory     \
	-lTAO_PortableServer   \
	-lTAO_Valuetype        \
	-lTAO_AnyTypeCode      \
	-lTAO                  \
	-lACE

#
# Server
#
noinst_PROGRAMS += server

server_CPPFLAGS = \
	$(LORICA_CXXFLAGS) \
	$(LORICA_CPPFLAGS)

server_SOURCES = \
	server.cpp \
	testC.cpp  \
	testS.cpp  \
	test_i.cpp \
	testC.h    \
	testC.inl  \
	testS.h    \
	testS.inl  \
	test_i.h

server_LDADD = \
	$(LORICA_top_dir)/src/lorica/liblorica_Client.la \
	-lTAO_DynamicInterface                           \
	-lTAO_Messaging                                  \
	-lTAO_PI                                         \
	-lTAO_CodecFactory                               \
	-lTAO_PortableServer                             \
	-lTAO_Valuetype                                  \
	-lTAO_AnyTypeCode                                \
	-lTAO                                            \
	-lACE

if THIS_IS_NOT_A_DISTRIBUTION
CLEAN_IN_FILES = Makefile.in
else
CLEAN_IN_FILES =
endif

DISTCLEANFILES = *.log *.ior *.pid *.cache $(CLEAN_IN_FILES)
