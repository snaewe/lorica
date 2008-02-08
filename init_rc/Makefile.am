#  
#  Makefile for Lorica.
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


if THIS_IS_NOT_A_DISTRIBUTION
CLEAN_IN_FILES = Makefile.in
else
CLEAN_IN_FILES =
endif

all-local: gentoo.confd

gentoo.confd: Makefile
	echo '# $(sysconfdir)/conf.d/$(LORICA_NAME): config file for $(sysconfdir)/init.d/$(LORICA_NAME)"' >$@
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

DISTCLEANFILES = $(CLEAN_IN_FILES) Makefile

CLEANFILES = gentoo.confd

