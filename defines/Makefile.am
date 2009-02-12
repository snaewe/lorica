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
	build-system-defs.h \
	hardcoded-defs.h

all-local: pathdefs.h

pathdefs.h: Makefile
	echo '#define LORICA_PID_FILE        "/var/run/$(LORICA_NAME).pid"'             >$@
	echo '#define LORICA_CONF_FILE       "$(sysconfdir)/$(LORICA_NAME).conf"'      >>$@
	echo '#define LORICA_EXE_PATH        "$(sbindir)/$(LORICA_NAME)"'              >>$@
	echo '#define LORICA_EXE_NAME        "$(LORICA_NAME)"'                         >>$@
	echo '#define LORICA_CACHE_DIR       "$(LORICA_CACHE_DIR)"'                    >>$@
	echo '#define LORICA_IOR_FILE        "$(LORICA_CACHE_DIR)/$(LORICA_NAME).ior"' >>$@
	echo '#define IFR_SERVICE_IOR_FILE   "$(LORICA_CACHE_DIR)/ifr.ior"'            >>$@
	echo '#define IFR_SERVICE_CACHE_FILE "$(LORICA_CACHE_DIR)/ifr.cache"'          >>$@
	echo '#define IFR_SERVICE_EXE_NAME   "IFR_Service"'                            >>$@

if THIS_IS_NOT_A_DISTRIBUTION
CLEAN_IN_FILES = Makefile.in
else
CLEAN_IN_FILES =
endif

DISTCLEANFILES = $(CLEAN_IN_FILES) Makefile

CLEANFILES = pathdefs.h

