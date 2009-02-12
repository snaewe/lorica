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

if LORICA_DEBIAN
loricadocdir=$(prefix)/share/doc/$(LORICA_NAME)
loricadocdeveldir=$(prefix)/share/doc/$(LORICA_NAME)-dev
else
loricadocdir=$(prefix)/share/doc/$(LORICA_NAME)-$(LORICA_VERSION)
loricadocdeveldir=$(prefix)/share/doc/$(LORICA_NAME)-devel-$(LORICA_VERSION)
endif
if LORICA_SUSE
loricadocdir=$(prefix)/share/doc/packages/$(LORICA_NAME)-$(LORICA_VERSION)
loricadocdeveldir=$(prefix)/share/doc/packages/$(LORICA_NAME)-devel-$(LORICA_VERSION)
endif

loricadoc_DATA = \
	lorica_configuration        \
	using_the_generic_evaluator \
	../README                   \
	../COPYING                  \
	../AUTHORS                  \
	../ChangeLog

loricadocdevel_DATA = \
	building_lorica

if THIS_IS_NOT_A_DISTRIBUTION
CLEAN_IN_FILES = Makefile.in
else
CLEAN_IN_FILES =
endif

DISTCLEANFILES = Makefile $(CLEAN_IN_FILES)

