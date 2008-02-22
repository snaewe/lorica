#  
#  Top-level Makefile for Lorica.
#  Copyright (C) 2007-2008 OMC Denmark ApS
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

ACLOCAL_AMFLAGS = -I m4

CWD = $(CURDIR)
MKDIR = $(MKDIR_P)

SUBDIRS = \
	init_rc \
	defines \
	acetao  \
	src     \
	ebuild  \
	deb     \
	docs

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
	$(CLEAN_IN_FILES)

EXTRA_DIST = \
	docs                              \
	ebuild/$(LORICA_NAME).ebuild.in   \
	ebuild/ChangeLog                  \
	ebuild/metadata.xml               \
	deb/compat                        \
	deb/changelog                     \
	deb/changelog.pre                 \
	deb/control.in                    \
	deb/control                       \
	deb/copyright.in                  \
	deb/copyright                     \
	deb/rules                         \
	deb/version.sed                   \
	deb/version.sed.in                \
	deb/watch                         \
	deb/watch.in                      \
	deb/$(LORICA_NAME).install        \
	deb/$(LORICA_NAME).install.in     \
	deb/$(LORICA_NAME)-dev.install.in \
	deb/$(LORICA_NAME)-dev.install    \
	bootstrap                         \
	$(LORICA_NAME).spec               \
	$(LORICA_NAME).spec.in            \
	init_rc/gentoo.initd              \
	src/proxy/$(LORICA_NAME).conf     \
	src/proxy/$(LORICA_NAME).ssliop

dist-hook:
	rm -f $(distdir)/defines/pathdefs.h
	rm -f $(distdir)/init_rc/gentoo.confd

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
	cp $(CWD)/init_rc/generic.confd $(CWD)/deb/$(LORICA_NAME)-$(LORICA_VERSION)/debian/$(LORICA_NAME).default
	cp $(CWD)/init_rc/ubuntu.initd $(CWD)/deb/$(LORICA_NAME)-$(LORICA_VERSION)/debian/$(LORICA_NAME).init
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


