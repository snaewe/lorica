#  
#  Top-level Makefile for Lorica.
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

SUBDIRS = \
	.      \
	src    \
	ebuild \
	deb    \
	docs

if LORICA_MUST_BUILD_ACETAO
export LORICA_ROOT = $(LORICA_top_dir)
export ACE_ROOT = $(LORICA_ROOT)/$(ACE_WRAPPERS_DIR)
export TAO_ROOT = $(ACE_ROOT)/TAO
export PATH = $(LORICA_ROOT)/bin:$(ACE_ROOT)/bin:$(ENV_PATH)

if LORICA_DARWIN
export DYLD_LIBRARY_PATH=$(LORICA_ROOT)/lib:$(ACE_ROOT)/lib:$(ENV_DYLD_LIBRARY_PATH)
else
export LD_LIBRARY_PATH = $(LORICA_ROOT)/lib:$(ACE_ROOT)/lib:$(ENV_LD_LIBRARY_PATH)
endif

AM_CPPFLAGS = -I$(ACETAO_local_includedir) -I$(ACETAO_local_includedir)/TAO -I$(ACETAO_local_includedir)/TAO/orbsvcs

if LORICA_DARWIN
TAO_FILES = \
        $(ACE_ROOT)/TAO/tao/libTAO_IFR_Client.dylib       \
	$(ACE_ROOT)/TAO/tao/libTAO_DynamicInterface.dylib \
        $(ACE_ROOT)/TAO/tao/libTAO_EndpointPolicy.dylib   \
        $(ACE_ROOT)/TAO/tao/libTAO_Messaging.dylib        \
        $(ACE_ROOT)/TAO/tao/libTAO_IORTable.dylib         \
        $(ACE_ROOT)/TAO/tao/libTAO_SSLIOP.dylib           \
        $(ACE_ROOT)/TAO/tao/libTAO_PI_Server.dylib        \
        $(ACE_ROOT)/TAO/tao/libTAO_Security.dylib         \
        $(ACE_ROOT)/TAO/tao/libTAO_PI.dylib               \
        $(ACE_ROOT)/TAO/tao/libTAO_CodecFactory.dylib     \
        $(ACE_ROOT)/TAO/tao/libTAO_Valuetype.dylib        \
        $(ACE_ROOT)/TAO/tao/libTAO_PortableServer.dylib   \
        $(ACE_ROOT)/TAO/tao/libTAO_AnyTypeCode.dylib      \
        $(ACE_ROOT)/TAO/tao/libTAO.dylib                  \
        $(ACE_ROOT)/ace/SSL/libACE_SSL.dylib              \
        $(ACE_ROOT)/ace/libACE.dylib 
else
TAO_FILES = \
        $(ACE_ROOT)/TAO/tao/libTAO_IFR_Client.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)       \
	$(ACE_ROOT)/TAO/tao/libTAO_DynamicInterface.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE) \
        $(ACE_ROOT)/TAO/tao/libTAO_EndpointPolicy.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)   \
        $(ACE_ROOT)/TAO/tao/libTAO_Messaging.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)        \
        $(ACE_ROOT)/TAO/tao/libTAO_IORTable.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)         \
        $(ACE_ROOT)/TAO/tao/libTAO_SSLIOP.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)           \
        $(ACE_ROOT)/TAO/tao/libTAO_PI_Server.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)        \
        $(ACE_ROOT)/TAO/tao/libTAO_Security.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)         \
        $(ACE_ROOT)/TAO/tao/libTAO_PI.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)               \
        $(ACE_ROOT)/TAO/tao/libTAO_CodecFactory.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)     \
        $(ACE_ROOT)/TAO/tao/libTAO_Valuetype.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)        \
        $(ACE_ROOT)/TAO/tao/libTAO_PortableServer.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)   \
        $(ACE_ROOT)/TAO/tao/libTAO_AnyTypeCode.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)      \
        $(ACE_ROOT)/TAO/tao/libTAO.so.$(LIBTAO_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)                  \
        $(ACE_ROOT)/ace/SSL/libACE_SSL.so.$(LIBACE_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)              \
        $(ACE_ROOT)/ace/libACE.so.$(LIBACE_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE)
endif # LORICA_DARWIN

else
TAO_FILES =
endif # LORICA_MUST_BUILD_ACETAO

if THIS_IS_NOT_A_DISTRIBUTION
CLEAN_IN_FILES = Makefile.in
else
CLEAN_IN_FILES =
endif

DISTCLEANFILES = \
	aclocal.m4        \
	intltool-extract  \
	intltool-merge    \
	intltool-update   \
	iconv-detect.c    \
	Makefile          \
	*.tar.gz          \
	$(CLEAN_IN_FILES) \
	$(CWD)/ACE+TAO-$(LIBACE_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE).tar.bz2

if LORICA_MUST_BUILD_ACETAO
DISTCLEANFILES += $(ACE_ROOT)
endif

EXTRA_DIST = \
	docs                               \
	ebuild/$(LORICA_NAME).ebuild.in   \
	ebuild/ChangeLog                   \
	ebuild/metadata.xml                \
	deb/compat                         \
	deb/changelog                      \
	deb/changelog.pre                  \
	deb/control.in                     \
	deb/control                        \
	deb/copyright.in                   \
	deb/copyright                      \
	deb/rules                          \
	deb/version.sed                    \
	deb/version.sed.in                 \
	deb/watch                          \
	deb/watch.in                       \
	deb/$(LORICA_NAME).install        \
	deb/$(LORICA_NAME).install.in     \
	deb/$(LORICA_NAME)-dev.install.in \
	deb/$(LORICA_NAME)-dev.install    \
	autogen.sh                         \
	$(LORICA_NAME).spec               \
	$(LORICA_NAME).spec.in

all-local: tao

#dist-hook:
#	rm -rf $(ACE_ROOT)

release:
	$(CWD)/release.sh $(LORICA_DIST_RELEASE)

distfiles:
	$(CWD)/release.sh $(LORICA_DIST_RELEASE) no_push

tarball:
	$(CWD)/release.sh tarball

if LORICA_SUSE
dist-rpm:
	make
	make dist
	rm -rf $(HOME)/suse_build/*
	$(MKDIR) $(HOME)/suse_build
	cp -f $(CWD)/$(LORICA_NAME).spec $(HOME)/suse_build/
	cp -f $(CWD)/$(LORICA_NAME)-$(LORICA_VERSION).tar.gz $(HOME)/suse_build/
	@echo ""
	@echo "*"
	@echo "*  Now cd to $(HOME)/suse_build/ and execute build as root"
	@echo "*"
	@echo ""
else 
if LORICA_RPMBUILD
dist-rpm:
if LORICA_RPMDEV_SETUPTREE
	rpmdev-setuptree
endif
if LORICA_RPMDEV_WIPETREE
	rpmdev-wipetree
endif
	make
	make dist
	cp $(CWD)/$(LORICA_NAME).spec $(HOME)/rpmbuild/SPECS/
	cp $(CWD)/$(LORICA_NAME)-$(LORICA_VERSION).tar.gz $(HOME)/rpmbuild/SOURCES/
	cd $(HOME)/rpmbuild/SPECS && rpmbuild -ba $(LORICA_NAME).spec
endif
endif

dist-ebuild:
	make
	make dist
	cp $(CWD)/ebuild/$(LORICA_NAME).ebuild $(CWD)/ebuild/$(LORICA_NAME)-$(LORICA_VERSION).ebuild

if LORICA_DPKG_BUILDPACKAGE
dist-deb-bin: dist-deb-prep
	cd $(CWD)/deb/$(LORICA_NAME)-$(LORICA_VERSION) && dpkg-buildpackage -rfakeroot

dist-deb-source: dist-deb-prep
	cd $(CWD)/deb/$(LORICA_NAME)-$(LORICA_VERSION) && dpkg-buildpackage -S -sa -rfakeroot
endif

dist-deb-prep:
	make
	make dist
	cp $(CWD)/$(LORICA_NAME)-$(LORICA_VERSION).tar.gz $(CWD)/deb/$(LORICA_NAME)_$(LORICA_VERSION).orig.tar.gz
	cd $(CWD)/deb && $(TAR) xzf $(LORICA_NAME)_$(LORICA_VERSION).orig.tar.gz
	cd $(CWD)/deb/$(LORICA_NAME)-$(LORICA_VERSION) && $(MKDIR) debian
	cp $(CWD)/deb/compat $(CWD)/deb/$(LORICA_NAME)-$(LORICA_VERSION)/debian/
	cp $(CWD)/deb/changelog $(CWD)/deb/$(LORICA_NAME)-$(LORICA_VERSION)/debian/
	cp $(CWD)/deb/control $(CWD)/deb/$(LORICA_NAME)-$(LORICA_VERSION)/debian/
	cp $(CWD)/deb/control.in $(CWD)/deb/$(LORICA_NAME)-$(LORICA_VERSION)/debian/
	cp $(CWD)/deb/copyright $(CWD)/deb/$(LORICA_NAME)-$(LORICA_VERSION)/debian/
	cp $(CWD)/deb/copyright.in $(CWD)/deb/$(LORICA_NAME)-$(LORICA_VERSION)/debian/
	cp $(CWD)/deb/rules $(CWD)/deb/$(LORICA_NAME)-$(LORICA_VERSION)/debian/
	cp $(CWD)/deb/watch $(CWD)/deb/$(LORICA_NAME)-$(LORICA_VERSION)/debian/
	cp $(CWD)/deb/$(LORICA_NAME).install $(CWD)/deb/$(LORICA_NAME)-$(LORICA_VERSION)/debian/
	cp $(CWD)/deb/$(LORICA_NAME).install.in $(CWD)/deb/$(LORICA_NAME)-$(LORICA_VERSION)/debian/
	cp $(CWD)/deb/$(LORICA_NAME)-dev.install $(CWD)/deb/$(LORICA_NAME)-$(LORICA_VERSION)/debian/
	cp $(CWD)/deb/$(LORICA_NAME)-dev.install.in $(CWD)/deb/$(LORICA_NAME)-$(LORICA_VERSION)/debian/

tao: $(TAO_FILES)

if LORICA_MUST_BUILD_ACETAO
$(TAO_FILES): $(ACE_ROOT)
if LORICA_DARWIN_TIGER
	echo '#include "ace/config-macosx-tiger.h"' > $(ACE_ROOT)/ace/config.h
	echo 'ssl=1' > $(ACE_ROOT)/include/makeinclude/platform_macros.GNU
	echo 'include $$(ACE_ROOT)/include/makeinclude/platform_macosx_tiger.GNU' >> $(ACE_ROOT)/include/makeinclude/platform_macros.GNU
endif
if LORICA_DARWIN_LEOPARD
	echo '#include "ace/config-macosx-leopard.h"' > $(ACE_ROOT)/ace/config.h
	echo 'ssl=1' > $(ACE_ROOT)/include/makeinclude/platform_macros.GNU
	echo 'include $$(ACE_ROOT)/include/makeinclude/platform_macosx_tiger.GNU' >> $(ACE_ROOT)/include/makeinclude/platform_macros.GNU
else
	@echo '#include "ace/config-linux.h"' > $(ACE_ROOT)/ace/config.h
	@echo 'ssl=1' > $(ACE_ROOT)/include/makeinclude/platform_macros.GNU
	@echo 'include $$(ACE_ROOT)/include/makeinclude/platform_linux.GNU' >> $(ACE_ROOT)/include/makeinclude/platform_macros.GNU
endif
	cd $(ACE_ROOT)/ace && $(MAKE) -f GNUmakefile
	cd $(ACE_ROOT)/apps/gperf/src && $(MAKE) -f GNUmakefile
	cd $(ACE_ROOT)/ace/SSL && $(MAKE) -f GNUmakefile
	cd $(TAO_ROOT)/TAO_IDL && $(MAKE) -f GNUmakefile
	cd $(TAO_ROOT)/tao && $(MAKE) -f GNUmakefile.TAO
	cd $(TAO_ROOT)/tao && $(MAKE) -f GNUmakefile.AnyTypeCode
	cd $(TAO_ROOT)/tao && $(MAKE) -f GNUmakefile.Valuetype
	cd $(TAO_ROOT)/tao && $(MAKE) -f GNUmakefile.CodecFactory
	cd $(TAO_ROOT)/tao && $(MAKE) -f GNUmakefile.PI
	cd $(TAO_ROOT)/tao && $(MAKE) -f GNUmakefile.PortableServer
	cd $(TAO_ROOT)/tao && $(MAKE) -f GNUmakefile.Messaging
	cd $(TAO_ROOT)/orbsvcs/orbsvcs && $(MAKE) -f GNUmakefile.Svc_Utils
	cd $(TAO_ROOT)/tao && $(MAKE) -f GNUmakefile.IORTable
	cd $(TAO_ROOT)/tao && $(MAKE) -f GNUmakefile.PI_Server
	cd $(TAO_ROOT)/tao && $(MAKE) -f GNUmakefile.IFR_Client
	cd $(TAO_ROOT)/tao && $(MAKE) -f GNUmakefile.TypeCodeFactory
	cd $(TAO_ROOT)/tao && $(MAKE) -f GNUmakefile.DynamicInterface
	cd $(TAO_ROOT)/tao && $(MAKE) -f GNUmakefile.DynamicAny
	cd $(TAO_ROOT)/tao && $(MAKE) -f GNUmakefile.ImR_Client
	cd $(TAO_ROOT)/orbsvcs/orbsvcs && $(MAKE) -f GNUmakefile.IFRService
	cd $(TAO_ROOT)/orbsvcs/IFR_Service && $(MAKE) -f GNUmakefile
	cd $(TAO_ROOT)/orbsvcs/orbsvcs && $(MAKE) -f GNUmakefile.Security
	cd $(TAO_ROOT)/orbsvcs/orbsvcs && $(MAKE) -f GNUmakefile.SSLIOP
	cd $(TAO_ROOT)/tao && $(MAKE) -f GNUmakefile.EndpointPolicy 

$(ACE_ROOT):
if LORICA_WGET
	$(WGET) $(ACETAO)
endif
if LORICA_CURL
	$(CURL) $(ACETAO) > $(CWD)/ACE+TAO-$(LIBACE_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE).tar.bz2
endif
	$(TAR) xjf $(CWD)/ACE+TAO-$(LIBACE_CURRENT).$(LIBACETAO_REVISION).$(LIBACETAO_AGE).tar.bz2
endif # LORICA_MUST_BUILD_ACETAO
