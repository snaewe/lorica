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

if LORICA_DARWIN
CACHE_DIR = /var/db
else
CACHE_DIR = /var/cache
endif

all-local: pathdefs.h

pathdefs.h: Makefile
	echo '#define LORICA_EXE_NAME        "$(LORICA_NAME)"' >$@
	echo '#define LORICA_EXE_PATH        "$(sbindir)/$(LORICA_NAME)"' >>$@
	echo '#define LORICA_CACHE_DIR       "$(CACHE_DIR)/$(LORICA_NAME)"' >>$@
	echo '#define LORICA_IOR_FILE        "$(CACHE_DIR)/$(LORICA_NAME)/$(LORICA_NAME).ior"' >>$@
	echo '#define LORICA_PID_FILE        "/var/run/$(LORICA_NAME).pid"' >>$@
	echo '#define LORICA_CONF_FILE       "$(sysconfdir)/$(LORICA_NAME).conf"' >>$@
	echo '#define IFR_SERVICE_IOR_FILE   "$(CACHE_DIR)/$(LORICA_NAME)/ifr.ior"' >>$@
	echo '#define IFR_SERVICE_CACHE_FILE "$(CACHE_DIR)/$(LORICA_NAME)/ifr.cache"' >>$@
	echo '#define IFR_SERVICE_EXE_NAME   "ifr_service"' >>$@
	echo '#define IFR_SERVICE_EXE_PATH   "$(bindir)/ifr_service"' >>$@
	echo '#define TAO_IDL_EXE_PATH       "$(bindir)/tao_idl"' >>$@
	echo '#define TAO_GPERF_EXE_PATH     "$(bindir)/tao_gperf"' >>$@


DISTCLEANFILES = $(CLEAN_IN_FILES) Makefile

CLEANFILES = pathdefs.h

