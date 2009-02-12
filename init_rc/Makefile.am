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

noinst_HEADERS = \
	gentoo.initd \
	rhel.initd   \
	ubuntu.initd

all-local: generic.confd

generic.confd: Makefile
	echo '# Config file for $(sysconfdir)/init.d/$(LORICA_NAME)"' >$@
	echo '' >>$@
	echo '# Lorica startup options' >>$@
	echo 'LORICA_OPTIONS=""' >>$@
	echo '' >>$@
	echo '# PID file' >>$@
	echo 'LORICA_PID_FILE="/var/run/$(LORICA_NAME).pid"' >>$@
	echo '' >>$@
	echo '# Full path to Lorica executable' >>$@
	echo 'LORICA_EXE_PATH="$(sbindir)/$(LORICA_NAME)"' >>$@
	echo '' >>$@
if LORICA_DEBIAN
	-cp $@ ../debian/$(LORICA_NAME).default
	-cp ubuntu.initd ../debian/$(LORICA_NAME).init
endif

if THIS_IS_NOT_A_DISTRIBUTION
CLEAN_IN_FILES = Makefile.in
else
CLEAN_IN_FILES =
endif

DISTCLEANFILES = $(CLEAN_IN_FILES) Makefile

CLEANFILES = generic.confd

