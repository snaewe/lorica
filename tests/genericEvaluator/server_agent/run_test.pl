#
#    Lorica test script. 
#    Copyright (C) 2007 OMC Denmark ApS.
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
#

eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}'
     & eval 'exec perl -S $0 $argv:q'
     if 0;

use Env (ACE_ROOT);
use Env (TAO_ROOT);
use Env (LORICA_ROOT);

use lib "$ENV{ACE_ROOT}/bin";
use PerlACE::Run_Test;

$status = 0;
$debug_level = '0';

foreach $i (@ARGV) {
    if ($i eq '-debug') {
        $debug_level = '10';
    }
}

$mappedfile = PerlACE::LocalFile ("mapped.ior");
unlink $mappedfile;

$origfile = PerlACE::LocalFile ("orig.ior");
unlink $origfile;

$pidfile = PerlACE::LocalFile ("lorica.pid");
unlink $pidfile;

$ifrfile = "ifr.ior";

$PR = new PerlACE::Process ("$LORICA_ROOT/bin/lorica_proxy", "-f test.conf");

$IDLC = new PerlACE::Process ("$TAO_ROOT/orbsvcs/IFR_Service/tao_ifr",
                              " -ORBInitRef InterfaceRepository=file://$ifrfile ".
                              " test.idl");

$SV = new PerlACE::Process ("server", " -ORBUseSharedProfile 1 -ORBdebuglevel $debug_level -o $origfile -m $mappedfile");

$CL = new PerlACE::Process ("client", " -k file://$mappedfile -x");

print "Basic generic evaluator test is started\n";

$proxy = $PR->Spawn ();
if (PerlACE::waitforfile_timed ($pidfile,
                        $PerlACE::wait_interval_for_process_creation) == -1) {
    print STDERR "ERROR: cannot find file <$pidfile>\n";
    $PR->Kill (); $PR->TimedWait (1);
    exit 1;
}

$idlc_ret = $IDLC->SpawnWaitKill (60);
if (($idlc_ret != 0) || ($? != 0)) {
  if ($idlc_ret == 0) { $idlc_ret = $?; }
  print STDERR "Error: IDL Compiler returned $idlc_ret\n";
  $status = 1;
}

$server = $SV->Spawn ();
if (PerlACE::waitforfile_timed ($mappedfile,
             $PerlACE::wait_interval_for_process_creation) == -1) {
    print STDERR "ERROR: cannot find file <$pidfile>\n";
    $SV->Kill (); $SV->TimedWait (1);
    $PR->Kill (); $PR->TimedWait (1);
    exit 1;
}

$client = $CL->SpawnWaitKill (30);

$server = $SV->Kill ();

$proxy = $PR->Kill ();

if ($proxy != 0) {
    print STDERR "ERROR: server returned $server\n";
    $status = 1;
}

unlink $mappedfile;
unlink $origfile;
unlink $pidfile;

exit $status;
