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


eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}'
     & eval 'exec perl -S $0 $argv:q'
     if 0;

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

$iorfile = PerlACE::LocalFile ("server.ior");
unlink $iorfile;

$pidfile = PerlACE::LocalFile ("lorica.pid");
unlink $pidfile;

$PR = new PerlACE::Process ("$LORICA_ROOT/src/proxy/lorica", "-n -d -f test.conf");
$SV = new PerlACE::Process ("server", "-ORBListenEndpoints iiop://localhost:20951 -ORBdebuglevel $debug_level");
$CL = new PerlACE::Process ("client", "");

$proxy = $PR->Spawn ();

if (PerlACE::waitforfile_timed ($pidfile,
                        $PerlACE::wait_interval_for_process_creation) == -1) {
    print STDERR "ERROR: cannot find file <$pidfile>\n";
    $PR->Kill (); $PR->TimedWait (1);
    exit 1;
}

$server = $SV->Spawn ();

sleep(3);

if ($server != 0) {
    print STDERR "ERROR: server returned $server\n";
    exit 1;
}


$client = $CL->SpawnWaitKill (300);

if ($client != 0) {
    print STDERR "ERROR: client returned $client\n";
    $status = 1;
}

$server = $SV->WaitKill (10);

if ($server != 0) {
    print STDERR "ERROR: server returned $server\n";
    $status = 1;
}

$proxy = $PR->Kill ();

if ($proxy != 0) {
    print STDERR "ERROR: server returned $server\n";
    $status = 1;
}

unlink $iorfile;

unlink $pidfile;

exit $status;
