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
use Env (LORICA_ROOT);
use lib "$ENV{ACE_ROOT}/bin";
use PerlACE::Run_Test;

if ($TAO_ROOT eq "") {
    $TAO_ROOT = "$ACE_ROOT/TAO";
}
if ($LORICA_ROOT eq "") {
    print STDERR "ERROR: Please set the LORICA_ROOT environment variable before running this test\n";
    exit 1;
}

$status = 0;
$debug_level = '0';
$server_ip_addr = "localhost";
$lorica_conf = "lorica-server.conf";
$run_as_client = "NO";

foreach $i (@ARGV) {
    if ($i eq '-debug') {
        $debug_level = '10';
    }
    if ($i eq '-server') {
	$i = shift;
	$i = shift;
        $server_ip_addr = $i;
	$lorica_conf = "lorica-client.conf";
	$run_as_client = "YES";
    }
}

unlink ("ifr.cache");

$pidfile = PerlACE::LocalFile ("lorica.pid");
unlink $pidfile;

$ifrfile = PerlACE::LocalFile ("ifr.ior");
unlink $ifrfile;

$IFR = new PerlACE::Process("$TAO_ROOT/orbsvcs/IFR_Service/IFR_Service", " -o $ifrfile");
$PR = new PerlACE::Process ("$LORICA_ROOT/src/proxy/lorica", "-n -d -f $lorica_conf");

$IDLC = new PerlACE::Process ("$TAO_ROOT/orbsvcs/IFR_Service/tao_ifr", "-ORBInitRef InterfaceRepository=file://$ifrfile Test.idl");

if ($run_as_client eq "YES") {
    print "Running as client\n";
    $CL = new PerlACE::Process ("client", " -k $server_ip_addr");
} else {
    print "Running as server\n";
    $SV = new PerlACE::Process ("server", " -ORBUseSharedProfile 1 -ORBdebuglevel $debug_level");
}

print "3-way callback generic evaluator test is started\n";

$IFR->Spawn ();
if (PerlACE::waitforfile_timed ($ifrfile, 10) == -1) {
    print STDERR "ERROR: cannot find file <$ifrfile>\n";
    $IFR->Kill ();
    exit 1;
}

$proxy = $PR->Spawn ();
if (PerlACE::waitforfile_timed ($pidfile,
                        $PerlACE::wait_interval_for_process_creation) == -1) {
    print STDERR "ERROR: cannot find file <$pidfile>\n";
    $PR->Kill (); $PR->TimedWait (1);
    exit 1;
}

$idlc_ret = $IDLC->SpawnWaitKill (50);
if (($idlc_ret != 0) || ($? != 0)) {
  if ($idlc_ret == 0) { $idlc_ret = $?; }
  print STDERR "Error: IDL Compiler returned $idlc_ret\n";
  $status = 1;
}

if ($run_as_client eq "YES") {
    $client = $CL->Spawn ();
} else {
    $server = $SV->Spawn ();
}

if ($run_as_client eq "YES") {
    $client = $CL->Kill ();
} else {
    $server = $SV->Kill ();
}

$proxy = $PR->Kill ();

if ($proxy != 0) {
    print STDERR "ERROR: server returned $server\n";
    $status = 1;
}

$IFR->Kill ();

unlink $pidfile;
unlink $ifrfile;

exit $status;
