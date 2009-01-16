#!/bin/bash

#  
#  Build script for Lorica.
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

SVN="$1"
DIR_ROOT="$2"

#cd "$DIR_ROOT"     && "$SVN" diff >/Users/colding/ifname.patch
cd "$DIR_ROOT"     && "$SVN" revert -R .
cd "$DIR_ROOT"     && "$SVN" up
cd "$DIR_ROOT"/TAO && "$SVN" up
cd "$DIR_ROOT"/MPC && "$SVN" up
#cd "$DIR_ROOT"     && patch -p0 </Users/colding/ifname.patch
cd "$DIR_ROOT"/TAO && "$DIR_ROOT/bin/mwc.pl" TAO_ACE.mwc -type gnuace

exit $?

