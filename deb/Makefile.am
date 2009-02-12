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
	-rm -f lorica_*
	-rm -f lorica*.deb 
	-rm -f lorica*.tar.gz 
	-rm -rf `find -maxdepth 1 -type d -name 'lorica-*' -printf " %p"`

