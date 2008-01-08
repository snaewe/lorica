#!/bin/bash

WGET=/usr/bin/wget
TAR=/bin/tar
ACETAO=ACE+TAO-5.6.2.tar.bz2

export LORICA_ROOT="$(pwd)"
export ACE_ROOT="$LORICA_ROOT/ACE_wrappers"
export TAO_ROOT="$ACE_ROOT/TAO"

export LD_LIBRARY_PATH="$LORICA_ROOT/lib:$ACE_ROOT/lib:$LD_LIBRARY_PATH"	
export PATH="$LORICA_ROOT/bin:$ACE_ROOT/bin:$PATH"


#
# build TAO
#

# get the source
cd $LORICA_ROOT
$WGET -nc http://download.dre.vanderbilt.edu/previous_versions/$ACETAO
if [ ! -d "$TAO_ROOT" ]; then
    $TAR xjf $ACETAO
fi

# prepare for build
echo '#include "ace/config-linux.h"' > $ACE_ROOT/ace/config.h
echo 'ssl=1' > $ACE_ROOT/include/makeinclude/platform_macros.GNU
echo 'include $(ACE_ROOT)/include/makeinclude/platform_linux.GNU' >> $ACE_ROOT/include/makeinclude/platform_macros.GNU

# just build the ACE library
echo ""
echo " *******************************************************"
echo " *                 Building ACE                        *" 
echo " *******************************************************"
echo ""
cd $ACE_ROOT/ace
make

# used by TAO
echo ""
echo " *******************************************************"
echo " *                Building gperf                       *" 
echo " *******************************************************"
echo ""
cd $ACE_ROOT/apps/gperf/src
make

echo ""
echo " *******************************************************"
echo " *                 Building TAO                        *" 
echo " *******************************************************"
echo ""
cd $ACE_ROOT/TAO
make


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
make

