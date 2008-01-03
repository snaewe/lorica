#
#    Lorica test script. 
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
$debug = '0';

foreach $i (@ARGV) {
    if ($i eq '-debug') {
        $debug = '10';
    }
}

$pidfile = PerlACE::LocalFile("lorica.pid");
$ifrfile = "ifr.ior";

unlink $pidfile;
unlink "client.log";
unlink "proxy.log";
unlink "server.log";

$PR = new PerlACE::Process ("$LORICA_ROOT/bin/lorica_proxy", "-d -f test.conf -l10 -c2");

$IDLC = new PerlACE::Process ("$ACE_ROOT/bin/tao_ifr",
                              " -ORBInitRef InterfaceRepository=file://$ifrfile ".
                              " Test.idl");

$SV = new PerlACE::Process ("server", " -ORBUseSharedProfile 1 -ORBdebuglevel $debug");

print "Unbind test starting\n";

print "Spawning proxy\n";
$proxy = $PR->Spawn();
if (PerlACE::waitforfile_timed ($pidfile,
                        $PerlACE::wait_interval_for_process_creation) == -1) {
    print STDERR "ERROR: cannot find file <$pidfile>\n";
    $PR->Kill (); $PR->TimedWait (1);
    exit 1;
}

print "Spawning idl compiler\n";
$idlc_ret = $IDLC->SpawnWaitKill (60);
if (($idlc_ret != 0) || ($? != 0)) {
  if ($idlc_ret == 0) { $idlc_ret = $?; }
  print STDERR "Error: IDL Compiler returned $idlc_ret\n";
  $status = 1;
}

print "Spawning server\n";
$server = $SV->SpawnWaitKill (30);

print "Stopping server\n";
$server = $SV->Kill ();

print "Stopping proxy\n";
$proxy = $PR->Kill ();

if ($proxy != 0) {
    print STDERR "ERROR: server returned $server\n";
    $status = 1;
}

exit $status;
