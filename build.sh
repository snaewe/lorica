#!/bin/bash
#
#    Lorica build script for Linux and friends
#    Copyright (C) 2008 OMC Denmark ApS.
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

ACETAO=ACE+TAO-5.6.2.tar.bz2

export LORICA_ROOT="$(pwd)"
export ACE_ROOT="$LORICA_ROOT/ACE_wrappers"
export TAO_ROOT="$ACE_ROOT/TAO"
export PATH="$LORICA_ROOT/bin:$ACE_ROOT/bin:$PATH"

cmd=$(which tar)
if [ 0 -eq $? ]; then
    TAR=$(which tar)
else
    echo "You need tar to run this script"
    exit 1
fi

#
# get and build TAO
#

# determine download command and execute it
DOWNLOAD=""
if [ ! -f "$ACETAO" ]; then
    cmd=$(which wget)
    if [ 0 -eq $? ]; then
	$(which wget) -nc http://download.dre.vanderbilt.edu/previous_versions/$ACETAO
	    if [ 0 -eq $? ]; then
		DOWNLOAD=OK
	    fi
    fi
    if [ -z "$DOWNLOAD" ]; then
	cmd=$(which curl)
	if [ 0 -eq $? ]; then
	    $(which curl) http://download.dre.vanderbilt.edu/previous_versions/$ACETAO > $ACETAO
	    if [ 0 -eq $? ]; then
		DOWNLOAD=OK
	    fi
	fi
    fi
    if [ -z "$DOWNLOAD" ]; then
	echo "You need a working internet connection and curl(1) or wget(1) to run this script"
	exit 1
    fi

fi

# extract TAO
if [ ! -d "$TAO_ROOT" ]; then
    $TAR xjf $ACETAO
fi
if [ 0 -ne $? ]; then
    echo "\'tar xjf\' didn't work - bzip2 not supported?"
    exit 1
fi

# prepare for build
echo -n "Building ACE and TAO - "
MAKE_TARGET=""
case "$1" in
    leopard)
	echo "assuming MacOS Leopard"
	echo '#include "ace/config-macosx-leopard.h"' > $ACE_ROOT/ace/config.h
	echo 'ssl=1' > $ACE_ROOT/include/makeinclude/platform_macros.GNU
	echo 'include $(ACE_ROOT)/include/makeinclude/platform_macosx_tiger.GNU' >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
	export DYLD_LIBRARY_PATH="$LORICA_ROOT/lib:$ACE_ROOT/lib:$DYLD_LIBRARY_PATH"	
	;;
    tiger)
	echo "assuming MacOS Tiger"
	echo '#include "ace/config-macosx-tiger.h"' > $ACE_ROOT/ace/config.h
	echo 'ssl=1' > $ACE_ROOT/include/makeinclude/platform_macros.GNU
	echo 'include $(ACE_ROOT)/include/makeinclude/platform_macosx_tiger.GNU' >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
	export DYLD_LIBRARY_PATH="$LORICA_ROOT/lib:$ACE_ROOT/lib:$DYLD_LIBRARY_PATH"	
	;;
    linux)
	echo "assuming Linux"
	echo '#include "ace/config-linux.h"' > $ACE_ROOT/ace/config.h
	echo 'ssl=1' > $ACE_ROOT/include/makeinclude/platform_macros.GNU
	echo 'include $(ACE_ROOT)/include/makeinclude/platform_linux.GNU' >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
	export LD_LIBRARY_PATH="$LORICA_ROOT/lib:$ACE_ROOT/lib:$LD_LIBRARY_PATH"	
	;;
    *)
	MAKE_TARGET="$1"
	echo " WARNING, no platform given"
	echo "   Usage: build.sh <PLATFORM>"
	echo ""
	echo "   Supported platforms: linux, leopard, tiger"
	echo ""
	echo " Assuming request to build make target \"$MAKE_TARGET\". Proceeding..."
esac

echo ""
echo " *******************************************************"
echo " *              Building ACE Library                   *" 
echo " *******************************************************"
echo ""
cd $ACE_ROOT/ace && make -f GNUmakefile $MAKE_TARGET


echo ""
echo " *******************************************************"
echo " *                Building gperf                       *" 
echo " *******************************************************"
echo ""
cd $ACE_ROOT/apps/gperf/src && make -f GNUmakefile $MAKE_TARGET


echo ""
echo " *******************************************************"
echo " *            Building ACE SSL Library                 *" 
echo " *******************************************************"
echo ""
cd $ACE_ROOT/ace/SSL && make -f GNUmakefile $MAKE_TARGET


echo ""
echo " *******************************************************"
echo " *                 Building TAO                        *" 
echo " *******************************************************"
echo ""
cd $TAO_ROOT/TAO_IDL             && make -f GNUmakefile                  $MAKE_TARGET
cd $TAO_ROOT/tao                 && make -f GNUmakefile.TAO              $MAKE_TARGET
cd $TAO_ROOT/tao                 && make -f GNUmakefile.AnyTypeCode      $MAKE_TARGET
cd $TAO_ROOT/tao                 && make -f GNUmakefile.Valuetype        $MAKE_TARGET
cd $TAO_ROOT/tao                 && make -f GNUmakefile.CodecFactory     $MAKE_TARGET
cd $TAO_ROOT/tao                 && make -f GNUmakefile.PI               $MAKE_TARGET
cd $TAO_ROOT/tao                 && make -f GNUmakefile.PortableServer   $MAKE_TARGET
cd $TAO_ROOT/tao                 && make -f GNUmakefile.Messaging        $MAKE_TARGET
cd $TAO_ROOT/orbsvcs/orbsvcs     && make -f GNUmakefile.Svc_Utils        $MAKE_TARGET
cd $TAO_ROOT/tao                 && make -f GNUmakefile.IORTable         $MAKE_TARGET
cd $TAO_ROOT/tao                 && make -f GNUmakefile.PI_Server        $MAKE_TARGET
cd $TAO_ROOT/tao                 && make -f GNUmakefile.IFR_Client       $MAKE_TARGET
cd $TAO_ROOT/tao                 && make -f GNUmakefile.TypeCodeFactory  $MAKE_TARGET
cd $TAO_ROOT/tao                 && make -f GNUmakefile.DynamicInterface $MAKE_TARGET
cd $TAO_ROOT/tao                 && make -f GNUmakefile.DynamicAny       $MAKE_TARGET
cd $TAO_ROOT/tao                 && make -f GNUmakefile.ImR_Client       $MAKE_TARGET
cd $TAO_ROOT/orbsvcs/orbsvcs     && make -f GNUmakefile.IFRService       $MAKE_TARGET
cd $TAO_ROOT/orbsvcs/IFR_Service && make -f GNUmakefile                  $MAKE_TARGET
cd $TAO_ROOT/orbsvcs/orbsvcs     && make -f GNUmakefile.Security         $MAKE_TARGET
cd $TAO_ROOT/orbsvcs/orbsvcs     && make -f GNUmakefile.SSLIOP           $MAKE_TARGET
cd $TAO_ROOT/tao                 && make -f GNUmakefile.EndpointPolicy   $MAKE_TARGET

#
# build Lorica
#

echo ""
echo " *******************************************************"
echo " *                Building Lorica                      *" 
echo " *******************************************************"
echo ""
cd $LORICA_ROOT
$ACE_ROOT/bin/mwc.pl -type gnuace lorica.mwc
make $MAKE_TARGET

