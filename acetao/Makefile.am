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
SVN = svn

if LORICA_WGET
WGET = wget -nc
endif
if LORICA_CURL
CURL = curl
endif 

if HAVE_ACE_ROOT
export LORICA_ROOT = $(LORICA_top_dir)
export ACE_ROOT = $(CONF_ACE_ROOT)
export TAO_ROOT = $(ACE_ROOT)/TAO
export PATH = $(ACE_ROOT)/bin:$(ENV_PATH)

if LORICA_DARWIN
export DYLD_LIBRARY_PATH = $(ACE_ROOT)/lib:$(ENV_DYLD_LIBRARY_PATH)
else
export LD_LIBRARY_PATH = $(ACE_ROOT)/lib:$(ENV_LD_LIBRARY_PATH)
endif

AM_CPPFLAGS = $(ACETAO_CPPFLAGS)

TAO_EXECUTABLES = \
	$(ACE_ROOT)/TAO/orbsvcs/IFR_Service/tao_ifr

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
	$(ACE_ROOT)/TAO/tao/libTAO_Valuetype.dylib              \
	$(ACE_ROOT)/TAO/tao/libTAO_BiDirGIOP.dylib
else # !LORICA_DARWIN
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
	$(ACE_ROOT)/TAO/tao/libTAO_Valuetype.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)              \
	$(ACE_ROOT)/TAO/tao/libTAO_BiDirGIOP.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)
endif # LORICA_DARWIN

else # !HAVE_ACE_ROOT

if HAVE_CONF_PATH
export PATH = $(CONF_PATH):$(ENV_PATH)
endif

TAO_EXECUTABLES =
TAO_LIBS =
endif # HAVE_ACE_ROOT

TAO_FILES = $(TAO_EXECUTABLES) $(TAO_LIBS)

.NOTPARALLEL:

if LORICA_MUST_BUILD_ACETAO

all-local: $(TAO_FILES)

$(TAO_FILES): $(ACE_ROOT) UPDATE_FROM_SVN
	cd $(ACE_ROOT)/ace                 && CXX="$(CXX) $(LORICA_FIX_GXX_BUG_FLAG)" $(MAKE) -f GNUmakefile
	cd $(ACE_ROOT)/apps/gperf/src      && CXX="$(CXX) $(LORICA_FIX_GXX_BUG_FLAG)" $(MAKE) -f GNUmakefile
	cd $(ACE_ROOT)/ace/SSL             && CXX="$(CXX) $(LORICA_FIX_GXX_BUG_FLAG)" $(MAKE) -f GNUmakefile
	cd $(TAO_ROOT)/TAO_IDL             && $(MAKE) -f GNUmakefile
	cd $(TAO_ROOT)/tao                 && CXX="$(CXX) $(LORICA_FIX_GXX_BUG_FLAG)" $(MAKE) -f GNUmakefile.TAO
	cd $(TAO_ROOT)/tao                 && CXX="$(CXX) $(LORICA_FIX_GXX_BUG_FLAG)" $(MAKE) -f GNUmakefile.Codeset
	cd $(TAO_ROOT)/tao                 && CXX="$(CXX) $(LORICA_FIX_GXX_BUG_FLAG)" $(MAKE) -f GNUmakefile.AnyTypeCode
	cd $(TAO_ROOT)/tao                 && CXX="$(CXX) $(LORICA_FIX_GXX_BUG_FLAG)" $(MAKE) -f GNUmakefile.Valuetype
	cd $(TAO_ROOT)/tao                 && CXX="$(CXX) $(LORICA_FIX_GXX_BUG_FLAG)" $(MAKE) -f GNUmakefile.CodecFactory
	cd $(TAO_ROOT)/tao                 && CXX="$(CXX) $(LORICA_FIX_GXX_BUG_FLAG)" $(MAKE) -f GNUmakefile.PI
	cd $(TAO_ROOT)/tao                 && CXX="$(CXX) $(LORICA_FIX_GXX_BUG_FLAG)" $(MAKE) -f GNUmakefile.PortableServer
	cd $(TAO_ROOT)/tao                 && CXX="$(CXX) $(LORICA_FIX_GXX_BUG_FLAG)" $(MAKE) -f GNUmakefile.Messaging
	cd $(TAO_ROOT)/orbsvcs/orbsvcs     && CXX="$(CXX) $(LORICA_FIX_GXX_BUG_FLAG)" $(MAKE) -f GNUmakefile.Svc_Utils
	cd $(TAO_ROOT)/tao                 && CXX="$(CXX) $(LORICA_FIX_GXX_BUG_FLAG)" $(MAKE) -f GNUmakefile.IORTable
	cd $(TAO_ROOT)/tao                 && CXX="$(CXX) $(LORICA_FIX_GXX_BUG_FLAG)" $(MAKE) -f GNUmakefile.PI_Server
	cd $(TAO_ROOT)/tao                 && CXX="$(CXX) $(LORICA_FIX_GXX_BUG_FLAG)" $(MAKE) -f GNUmakefile.IFR_Client
	cd $(TAO_ROOT)/tao                 && CXX="$(CXX) $(LORICA_FIX_GXX_BUG_FLAG)" $(MAKE) -f GNUmakefile.TypeCodeFactory
	cd $(TAO_ROOT)/tao                 && CXX="$(CXX) $(LORICA_FIX_GXX_BUG_FLAG)" $(MAKE) -f GNUmakefile.DynamicInterface
	cd $(TAO_ROOT)/tao                 && CXX="$(CXX) $(LORICA_FIX_GXX_BUG_FLAG)" $(MAKE) -f GNUmakefile.DynamicAny
	cd $(TAO_ROOT)/tao                 && CXX="$(CXX) $(LORICA_FIX_GXX_BUG_FLAG)" $(MAKE) -f GNUmakefile.ImR_Client
	cd $(TAO_ROOT)/orbsvcs/orbsvcs     && CXX="$(CXX) $(LORICA_FIX_GXX_BUG_FLAG)" $(MAKE) -f GNUmakefile.IFRService
	cd $(TAO_ROOT)/orbsvcs/IFR_Service && $(MAKE) -f GNUmakefile
	cd $(TAO_ROOT)/orbsvcs/orbsvcs     && CXX="$(CXX) $(LORICA_FIX_GXX_BUG_FLAG)" $(MAKE) -f GNUmakefile.Security
	cd $(TAO_ROOT)/orbsvcs/orbsvcs     && CXX="$(CXX) $(LORICA_FIX_GXX_BUG_FLAG)" $(MAKE) -f GNUmakefile.SSLIOP
	cd $(TAO_ROOT)/tao                 && CXX="$(CXX) $(LORICA_FIX_GXX_BUG_FLAG)" $(MAKE) -f GNUmakefile.EndpointPolicy
	cd $(TAO_ROOT)/tao                 && CXX="$(CXX) $(LORICA_FIX_GXX_BUG_FLAG)" $(MAKE) -f GNUmakefile.BiDir_GIOP
	cd $(TAO_ROOT)/tao                 && CXX="$(CXX) $(LORICA_FIX_GXX_BUG_FLAG)" $(MAKE) -f GNUmakefile.Strategies

if ACETAO_FROM_SVN
.PHONY: UPDATE_FROM_SVN

UPDATE_FROM_SVN:
	$(CWD)/acetao_svn_update.sh "$(SVN)" "$(ACE_ROOT)"
else
UPDATE_FROM_SVN:

endif

$(ACE_ROOT):
if ACETAO_FROM_SVN
	cd $(ACETAO_SVN_ROOT) && $(SVN) co svn://svn.dre.vanderbilt.edu/DOC/Middleware/sets-anon/ACE+TAO .
else
if LORICA_WGET
	$(WGET) $(ACETAO)
endif
if LORICA_CURL
	$(CURL) $(ACETAO) > $(CWD)/ACE+TAO-$(LIBACE_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE).tar.bz2
endif
	$(TAR) --overwrite -xjmf $(CWD)/ACE+TAO-$(LIBACE_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE).tar.bz2
	-mv -f ACE_wrappers $(CONF_ACE_ROOT)
endif # ACETAO_FROM_SVN
	@echo 'ssl=1' > $(ACE_ROOT)/include/makeinclude/platform_macros.GNU
	@echo 'ssl=1' > $(ACE_ROOT)/bin/MakeProjectCreator/config/default.features
if ACETAO_ENABLE_IPV6
	@echo 'ipv6=1' >> $(ACE_ROOT)/bin/MakeProjectCreator/config/default.features
endif
if LORICA_STATIC
	@echo 'static_libs_only=1' >> $(ACE_ROOT)/include/makeinclude/platform_macros.GNU
	@echo 'optimize=0' >> $(ACE_ROOT)/include/makeinclude/platform_macros.GNU
	@echo 'inline=0' >> $(ACE_ROOT)/include/makeinclude/platform_macros.GNU
	@echo 'DEPGEN=$$(ACE_ROOT)/bin/depgen.pl' >> $(ACE_ROOT)/include/makeinclude/platform_macros.GNU
endif
	@echo 'debug=1' >> $(ACE_ROOT)/include/makeinclude/platform_macros.GNU
if LORICA_DEBUG
	@echo 'optimize=0' >> $(ACE_ROOT)/include/makeinclude/platform_macros.GNU
else
	@echo 'optimize=1' >> $(ACE_ROOT)/include/makeinclude/platform_macros.GNU
endif
if LORICA_DARWIN_LEOPARD
	@echo '#include "ace/config-macosx-leopard.h"' > $(ACE_ROOT)/ace/config.h
	@echo 'include $$(ACE_ROOT)/include/makeinclude/platform_macosx_leopard.GNU' >> $(ACE_ROOT)/include/makeinclude/platform_macros.GNU
endif
if LORICA_DARWIN_TIGER
	@echo '#include "ace/config-macosx-tiger.h"' > $(ACE_ROOT)/ace/config.h
	@echo 'include $$(ACE_ROOT)/include/makeinclude/platform_macosx_tiger.GNU' >> $(ACE_ROOT)/include/makeinclude/platform_macros.GNU
endif
if !LORICA_DARWIN
	@echo '#include "ace/config-linux.h"' > $(ACE_ROOT)/ace/config.h
	@echo 'include $$(ACE_ROOT)/include/makeinclude/platform_linux.GNU' >> $(ACE_ROOT)/include/makeinclude/platform_macros.GNU
endif
if !LORICA_HAVE_STREAMS
	@echo '#undef ACE_HAS_STRBUF_T' >> $(ACE_ROOT)/ace/config.h
	@echo '#define ACE_LACKS_STRRECVFD' >> $(ACE_ROOT)/ace/config.h
endif

endif # LORICA_MUST_BUILD_ACETAO

if THIS_IS_NOT_A_DISTRIBUTION
CLEAN_IN_FILES = Makefile.in
else
CLEAN_IN_FILES =
endif

DISTCLEANFILES = $(CLEAN_IN_FILES) Makefile $(CWD)/ACE+TAO-$(LIBACE_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE).tar.bz2

