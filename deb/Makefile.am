#  
#  Makefile for Brutus Keyring.
#  Copyright (C) 2007 OMC Denmark ApS
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

SUBDIRS = 

if THIS_IS_NOT_A_DISTRIBUTION
CLEAN_IN_FILES = Makefile.in
else
CLEAN_IN_FILES =
endif

DISTCLEANFILES = Makefile $(CLEAN_IN_FILES) *.install changelog version.sed watch

all-local:
	sed -f version.sed changelog.pre > changelog

clean-local: 
	-rm -f brutus-keyring_*
	-rm -f brutus-keyring*.deb 
	-rm -f brutus-keyring*.tar.gz 
	-rm -rf `find -maxdepth 1 -type d -name 'brutus-keyring-*' -printf " %p"`

