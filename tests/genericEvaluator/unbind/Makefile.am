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
	TestC.cpp \
	TestC.h   \
	TestC.inl \
	TestS.cpp \
	TestS.h   \
	TestS.inl

CLEANFILES = \
	Test-stamp \
	TestC.cpp  \
	TestC.h    \
	TestC.inl  \
	TestS.cpp  \
	TestS.h    \
	TestS.inl

TestC.cpp TestC.h TestC.inl TestS.cpp TestS.h TestS.inl: Test-stamp

Test-stamp: $(srcdir)/Test.idl
	$(TAO_IDL) $(TAO_IDLFLAGS) -Sa -St $(srcdir)/Test.idl
	@touch "$@"

noinst_HEADERS = \
	Test.idl   \
	test.conf  \
	unbind.mpc \
	run_test.pl 

#
# Server
#
noinst_PROGRAMS = server

server_CPPFLAGS = \
	$(LORICA_CXXFLAGS) \
	$(LORICA_CPPFLAGS)

server_SOURCES = \
	Servants.cpp \
	TestC.cpp    \
	TestS.cpp    \
	server.cpp   \
	Servants.h   \
	TestC.h      \
	TestC.inl    \
	TestS.h      \
	TestS.inl

server_LDADD = \
	$(LORICA_top_dir)/src/lorica/liblorica_Client.la \
	-lTAO_PortableServer                            \
	-lTAO_AnyTypeCode                               \
	-lTAO                                           \
	-lACE

if THIS_IS_NOT_A_DISTRIBUTION
CLEAN_IN_FILES = Makefile.in
else
CLEAN_IN_FILES =
endif

DISTCLEANFILES = *.log *.ior *.pid *.cache $(CLEAN_IN_FILES)
