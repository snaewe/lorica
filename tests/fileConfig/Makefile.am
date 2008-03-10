#  
#  Lorica makefile for test program
#  Copyright (C) 2008 OMC Denmark ApS
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
#  MA 02111-1307 USA

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
GPERF = $(ACE_ROOT)/apps/gperf/src/gperf

else # !HAVE_ACE_ROOT

TAO_IDL = tao_idl
GPERF = gperf

endif # HAVE_ACE_ROOT

include $(CONF_ACE_ROOT)/include/makeinclude/macros.GNU
include $(CONF_ACE_ROOT)/include/makeinclude/wrapper_macros.GNU

TAO_IDLFLAGS = -I$(TAO_ROOT) -g $(GPERF)

#if LORICA_DARWIN
#TESTS_ENVIRONMENT = env DYLD_LIBRARY_PATH=$(ACE_ROOT)/lib:$(ENV_DYLD_LIBRARY_PATH) LORICA_ROOT=$(LORICA_top_dir) TAO_ROOT=$(TAO_ROOT)
#else
#TESTS_ENVIRONMENT = env LD_LIBRARY_PATH=$(ACE_ROOT)/lib:$(ENV_DYLD_LIBRARY_PATH) LORICA_ROOT=$(LORICA_top_dir) TAO_ROOT=$(TAO_ROOT)
#endif
#TESTS = run_test.pl

##################################################################

noinst_PROGRAMS = test

test_CPPFLAGS = \
	$(LORICA_CXXFLAGS) \
	$(LORICA_CPPFLAGS) \
	-DACE_HAS_SSL=1

test_SOURCES = \
	main.cpp

test_LDFLAGS = 

test_LDADD = \
	$(LORICA_top_dir)/src/lorica/liblorica_EvaluatorBase.la \
	-lTAO_SSLIOP                                            \
	-lTAO_PI_Server                                         \
	-lACE_SSL                                               \
	-lTAO_Security                                          \
	-lTAO_PI                                                \
	-lTAO_CodecFactory                                      \
	-lTAO_Valuetype                                         \
	-lTAO_PortableServer                                    \
	-lTAO_AnyTypeCode                                       \
	-lTAO                                                   \
	-lACE
