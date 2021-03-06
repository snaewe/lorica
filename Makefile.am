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

ACLOCAL_AMFLAGS = -I m4

CWD = $(CURDIR)
MKDIR_P ?= $(mkdir_p)
MKDIR = $(MKDIR_P) 

if DO_BUILD_TESTS
SUBDIRS = \
	init_rc \
	defines \
	acetao  \
	src     \
	ebuild  \
	deb     \
	docs    \
	tests
else
SUBDIRS = \
	init_rc \
	defines \
	acetao  \
	src     \
	ebuild  \
	deb     \
	docs
endif

if THIS_IS_NOT_A_DISTRIBUTION
CLEAN_IN_FILES = Makefile.in
else
CLEAN_IN_FILES =
endif

DISTCLEANFILES = \
	lorica_codeset.conf \
	aclocal.m4          \
	intltool-extract    \
	intltool-merge      \
	intltool-update     \
	iconv-detect.c      \
	Makefile            \
	*.tar*              \
	ifr.cache           \
	$(CLEAN_IN_FILES)

EXTRA_DIST = \
	docs                                    \
	apple                                   \
	ebuild/$(LORICA_NAME).ebuild.in         \
	ebuild/ChangeLog                        \
	ebuild/metadata.xml                     \
	deb/compat                              \
	deb/changelog                           \
	deb/changelog.pre                       \
	deb/control.in                          \
	deb/control                             \
	deb/copyright.in                        \
	deb/copyright                           \
	deb/rules                               \
	deb/version.sed                         \
	deb/version.sed.in                      \
	deb/watch                               \
	deb/watch.in                            \
	deb/$(LORICA_NAME).install              \
	deb/$(LORICA_NAME).install.in           \
	deb/$(LORICA_NAME)-dev.install.in       \
	deb/$(LORICA_NAME)-dev.install          \
	bootstrap                               \
	$(LORICA_NAME).spec                     \
	$(LORICA_NAME).spec.in                  \
	$(LORICA_NAME).mwc                      \
	init_rc/gentoo.initd                    \
	init_rc/ubuntu.initd                    \
	src/proxy/$(LORICA_NAME).conf           \
	src/proxy/$(LORICA_NAME).ssliop         \
	src/lorica/lorica_Client.mpc            \
	src/lorica/lorica_EvaluatorBase.mpc     \
	src/lorica/lorica_GenericEvaluator.mpc  \
	src/OCI/AsynchProxyTools.mpc            \
	src/proxy/proxy.mpc

dist-hook:
	-rm -f $(distdir)/defines/pathdefs.h
	-rm -f $(distdir)/init_rc/gentoo.confd
	-rm -f $(distdir)/src/lorica/*S.*
	-rm -f $(distdir)/src/lorica/*C.*
	-rm -f $(distdir)/apple/*.dmg
	-chmod -R a+w  $(distdir)/apple/Lorica.build
	-rm -rf $(distdir)/apple/Lorica.build

release:
	$(CWD)/release.sh $(LORICA_DIST_RELEASE)

distfiles:
	$(CWD)/release.sh $(LORICA_DIST_RELEASE) no_push

tarball:
	$(CWD)/release.sh tarball

if LORICA_DARWIN
dist-dmg:
	cd $(CWD)/apple && ./build
endif

if LORICA_SUSE
dist-rpm:
	make
	make dist
	-rm -rf $(HOME)/suse_build/*
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
dist-rpm: dist-svnrev
if LORICA_RPMDEV_SETUPTREE
	rpmdev-setuptree
else
	mkdir -p $(HOME)/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS}
	echo '%home '$(HOME) > $(HOME)/.rpmmacros
	echo '%_topdir     %{home}/rpmbuild' >> $(HOME)/.rpmmacros
endif
if LORICA_RPMDEV_WIPETREE
	rpmdev-wipetree
else
	-rm -rf $(HOME)/rpmbuild/BUILD/*
	-rm -rf $(HOME)/rpmbuild/RPMS/*
	-rm -rf $(HOME)/rpmbuild/SOURCES/*
	-rm -rf $(HOME)/rpmbuild/SPECS/*
	-rm -rf $(HOME)/rpmbuild/SRPMS/*
endif
	cp $(CWD)/$(LORICA_NAME).spec $(HOME)/rpmbuild/SPECS/
	cp $(CWD)/$(LORICA_NAME)-rev$(SVN_REV).tar.gz $(HOME)/rpmbuild/SOURCES/
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
	cd $(CWD)/deb && tar xzf $(LORICA_NAME)_$(LORICA_VERSION).orig.tar.gz
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

if LORICA_SVNREV
release-svnrev: dist-svnrev
	scp -i $(HOME)/.ssh/no_passphrase_id_rsa lorica-rev$(SVN_REV).tar.gz colding@42tools.com:/var/www/omc/htdocs/sites/default/files/downloads/dist/$(LORICA_NAME)/SOURCES/

dist-svnrev:
	-rm -rf ./lorica-$(LORICA_VERSION)
	svn export . ./lorica-$(LORICA_VERSION)
	find ./lorica-$(LORICA_VERSION) -exec touch {} \;
	tar cf lorica-rev$(SVN_REV).tar ./lorica-$(LORICA_VERSION)
	gzip lorica-rev$(SVN_REV).tar
endif

clean-local: 
	-rm -rf lorica-rev*
