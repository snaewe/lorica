#  
#  ACE and TAO Makefile for Lorica.
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

CWD = $(CURDIR)
MKDIR = $(MKDIR_P)
TAR = tar

if LORICA_WGET
WGET = wget --no-cache -nc
endif
if LORICA_CURL
CURL = curl
endif 

if HAVE_ACE_ROOT
export LORICA_ROOT = $(LORICA_top_dir)
export ACE_ROOT = $(CONF_ACE_ROOT)
export TAO_ROOT = $(ACE_ROOT)/TAO
export PATH = $(LORICA_ROOT)/bin:$(ACE_ROOT)/bin:$(ENV_PATH)

if LORICA_DARWIN
export DYLD_LIBRARY_PATH = $(ACE_ROOT)/lib:$(ENV_DYLD_LIBRARY_PATH)
else
export LD_LIBRARY_PATH = $(ACE_ROOT)/lib:$(ENV_LD_LIBRARY_PATH)
endif

AM_CPPFLAGS = $(ACETAO_INCLUDES)

if LORICA_DARWIN
TAO_LIBS = \
	$(ACE_ROOT)/ace/libACE.dylib                            \
	$(ACE_ROOT)/ace/SSL/libACE_SSL.dylib                    \
	$(ACE_ROOT)/TAO/tao/libTAO_Codeset.dylib                \
	$(ACE_ROOT)/TAO/tao/libTAO_AnyTypeCode.dylib            \
	$(ACE_ROOT)/TAO/tao/libTAO_CodecFactory.dylib           \
	$(ACE_ROOT)/TAO/tao/libTAO_DynamicAny.dylib             \
	$(ACE_ROOT)/TAO/tao/libTAO_DynamicInterface.dylib       \
	$(ACE_ROOT)/TAO/tao/libTAO_EndpointPolicy.dylib         \
	$(ACE_ROOT)/TAO/TAO_IDL/libTAO_IDL_BE.dylib             \
	$(ACE_ROOT)/TAO/TAO_IDL/libTAO_IDL_FE.dylib             \
	$(ACE_ROOT)/TAO/orbsvcs/IFR_Service/libTAO_IFR_BE.dylib \
	$(ACE_ROOT)/TAO/tao/libTAO_IFR_Client.dylib             \
	$(ACE_ROOT)/TAO/orbsvcs/orbsvcs/libTAO_IFRService.dylib \
	$(ACE_ROOT)/TAO/tao/libTAO_ImR_Client.dylib             \
	$(ACE_ROOT)/TAO/tao/libTAO_IORTable.dylib               \
	$(ACE_ROOT)/TAO/tao/libTAO_Messaging.dylib              \
	$(ACE_ROOT)/TAO/tao/libTAO_PI_Server.dylib              \
	$(ACE_ROOT)/TAO/tao/libTAO_PI.dylib                     \
	$(ACE_ROOT)/TAO/tao/libTAO_PortableServer.dylib         \
	$(ACE_ROOT)/TAO/orbsvcs/orbsvcs/libTAO_Security.dylib   \
	$(ACE_ROOT)/TAO/tao/libTAO.dylib                        \
	$(ACE_ROOT)/TAO/orbsvcs/orbsvcs/libTAO_SSLIOP.dylib     \
	$(ACE_ROOT)/TAO/orbsvcs/orbsvcs/libTAO_Svc_Utils.dylib  \
	$(ACE_ROOT)/TAO/tao/libTAO_TypeCodeFactory.dylib        \
	$(ACE_ROOT)/TAO/tao/libTAO_Valuetype.dylib
else
TAO_LIBS = \
	$(ACE_ROOT)/ace/libACE.so.$(LIBACE_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)                            \
	$(ACE_ROOT)/ace/SSL/libACE_SSL.so.$(LIBACE_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)                    \
	$(ACE_ROOT)/TAO/tao/libTAO_Codeset.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)                \
	$(ACE_ROOT)/TAO/tao/libTAO_AnyTypeCode.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)            \
	$(ACE_ROOT)/TAO/tao/libTAO_CodecFactory.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)           \
	$(ACE_ROOT)/TAO/tao/libTAO_DynamicAny.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)             \
	$(ACE_ROOT)/TAO/tao/libTAO_DynamicInterface.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)       \
	$(ACE_ROOT)/TAO/tao/libTAO_EndpointPolicy.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)         \
	$(ACE_ROOT)/TAO/TAO_IDL/libTAO_IDL_BE.so.5.$(LIBACETAO_REVISION).$(LIBACETAO_AGE)                             \
	$(ACE_ROOT)/TAO/TAO_IDL/libTAO_IDL_FE.so.5.$(LIBACETAO_REVISION).$(LIBACETAO_AGE)                             \
	$(ACE_ROOT)/TAO/orbsvcs/IFR_Service/libTAO_IFR_BE.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE) \
	$(ACE_ROOT)/TAO/tao/libTAO_IFR_Client.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)             \
	$(ACE_ROOT)/TAO/orbsvcs/orbsvcs/libTAO_IFRService.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE) \
	$(ACE_ROOT)/TAO/tao/libTAO_ImR_Client.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)             \
	$(ACE_ROOT)/TAO/tao/libTAO_IORTable.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)               \
	$(ACE_ROOT)/TAO/tao/libTAO_Messaging.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)              \
	$(ACE_ROOT)/TAO/tao/libTAO_PI_Server.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)              \
	$(ACE_ROOT)/TAO/tao/libTAO_PI.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)                     \
	$(ACE_ROOT)/TAO/tao/libTAO_PortableServer.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)         \
	$(ACE_ROOT)/TAO/orbsvcs/orbsvcs/libTAO_Security.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)   \
	$(ACE_ROOT)/TAO/tao/libTAO.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)                        \
	$(ACE_ROOT)/TAO/orbsvcs/orbsvcs/libTAO_SSLIOP.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)     \
	$(ACE_ROOT)/TAO/orbsvcs/orbsvcs/libTAO_Svc_Utils.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)  \
	$(ACE_ROOT)/TAO/tao/libTAO_TypeCodeFactory.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)        \
	$(ACE_ROOT)/TAO/tao/libTAO_Valuetype.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)
endif # LORICA_DARWIN

else # !HAVE_ACE_ROOT

if HAVE_CONF_PATH
export PATH = $(CONF_PATH):$(ENV_PATH)
endif

if HAVE_CONF_LD_PATH
if LORICA_DARWIN
export DYLD_LIBRARY_PATH = $(CONF_LD_PATH):$(ENV_DYLD_LIBRARY_PATH)
else
export LD_LIBRARY_PATH = $(CONF_LD_PATH):$(ENV_LD_LIBRARY_PATH)
endif
endif

TAO_LIBS =
endif # HAVE_ACE_ROOT

.NOTPARALLEL:

if LORICA_MUST_BUILD_ACETAO

all-local: $(TAO_LIBS)

$(TAO_LIBS): $(ACE_ROOT)
if LORICA_DARWIN_TIGER
	@echo '#include "ace/config-macosx-tiger.h"' > $(ACE_ROOT)/ace/config.h
	@echo 'ssl=1' > $(ACE_ROOT)/include/makeinclude/platform_macros.GNU
	@echo 'include $$(ACE_ROOT)/include/makeinclude/platform_macosx_tiger.GNU' >> $(ACE_ROOT)/include/makeinclude/platform_macros.GNU
endif
if LORICA_DARWIN_LEOPARD
	@echo '#include "ace/config-macosx-leopard.h"' > $(ACE_ROOT)/ace/config.h
	@echo 'ssl=1' > $(ACE_ROOT)/include/makeinclude/platform_macros.GNU
	@echo 'include $$(ACE_ROOT)/include/makeinclude/platform_macosx_tiger.GNU' >> $(ACE_ROOT)/include/makeinclude/platform_macros.GNU
else
	@echo '#include "ace/config-linux.h"' > $(ACE_ROOT)/ace/config.h
	@echo 'ssl=1' > $(ACE_ROOT)/include/makeinclude/platform_macros.GNU
	@echo 'include $$(ACE_ROOT)/include/makeinclude/platform_linux.GNU' >> $(ACE_ROOT)/include/makeinclude/platform_macros.GNU
endif
	cd $(ACE_ROOT)/ace                 && $(MAKE) -f GNUmakefile
	cd $(ACE_ROOT)/apps/gperf/src      && $(MAKE) -f GNUmakefile
	cd $(ACE_ROOT)/ace/SSL             && $(MAKE) -f GNUmakefile
	cd $(TAO_ROOT)/TAO_IDL             && $(MAKE) -f GNUmakefile
	cd $(TAO_ROOT)/tao                 && $(MAKE) -f GNUmakefile.TAO
	cd $(TAO_ROOT)/tao                 && $(MAKE) -f GNUmakefile.Codeset
	cd $(TAO_ROOT)/tao                 && $(MAKE) -f GNUmakefile.AnyTypeCode
	cd $(TAO_ROOT)/tao                 && $(MAKE) -f GNUmakefile.Valuetype
	cd $(TAO_ROOT)/tao                 && $(MAKE) -f GNUmakefile.CodecFactory
	cd $(TAO_ROOT)/tao                 && $(MAKE) -f GNUmakefile.PI
	cd $(TAO_ROOT)/tao                 && $(MAKE) -f GNUmakefile.PortableServer
	cd $(TAO_ROOT)/tao                 && $(MAKE) -f GNUmakefile.Messaging
	cd $(TAO_ROOT)/orbsvcs/orbsvcs     && $(MAKE) -f GNUmakefile.Svc_Utils
	cd $(TAO_ROOT)/tao                 && $(MAKE) -f GNUmakefile.IORTable
	cd $(TAO_ROOT)/tao                 && $(MAKE) -f GNUmakefile.PI_Server
	cd $(TAO_ROOT)/tao                 && $(MAKE) -f GNUmakefile.IFR_Client
	cd $(TAO_ROOT)/tao                 && $(MAKE) -f GNUmakefile.TypeCodeFactory
	cd $(TAO_ROOT)/tao                 && $(MAKE) -f GNUmakefile.DynamicInterface
	cd $(TAO_ROOT)/tao                 && $(MAKE) -f GNUmakefile.DynamicAny
	cd $(TAO_ROOT)/tao                 && $(MAKE) -f GNUmakefile.ImR_Client
	cd $(TAO_ROOT)/orbsvcs/orbsvcs     && $(MAKE) -f GNUmakefile.IFRService
	cd $(TAO_ROOT)/orbsvcs/IFR_Service && $(MAKE) -f GNUmakefile
	cd $(TAO_ROOT)/orbsvcs/orbsvcs     && $(MAKE) -f GNUmakefile.Security
	cd $(TAO_ROOT)/orbsvcs/orbsvcs     && $(MAKE) -f GNUmakefile.SSLIOP
	cd $(TAO_ROOT)/tao                 && $(MAKE) -f GNUmakefile.EndpointPolicy

$(ACE_ROOT):
if LORICA_WGET
	$(WGET) $(ACETAO)
endif
if LORICA_CURL
	$(CURL) $(ACETAO) > $(CWD)/ACE+TAO-$(LIBACE_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE).tar.bz2
endif
	$(TAR) --overwrite -xjf $(CWD)/ACE+TAO-$(LIBACE_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE).tar.bz2
	-mv -f $(ACE_WRAPPERS_DIR) $(CONF_ACE_ROOT)
endif # LORICA_MUST_BUILD_ACETAO

if THIS_IS_NOT_A_DISTRIBUTION
CLEAN_IN_FILES = Makefile.in
else
CLEAN_IN_FILES =
endif

DISTCLEANFILES = $(CLEAN_IN_FILES) Makefile $(CWD)/ACE+TAO-$(LIBACE_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE).tar.bz2

