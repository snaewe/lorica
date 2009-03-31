/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 *  Lorica source file.
 *  Copyright (C) 2007-2009 OMC Denmark ApS.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

//**************************************************************************
//
// BASED ON ACE SAMPLE CODE BY:
//    ACE_wrappers/examples/NT_Service and
//    ACE_wrappers/TAO/orbsvcs/ImplRepo_Service
//
//**************************************************************************

#include "defines/build-system-defs.h"

#include <ace/Get_Opt.h>
#include <ace/streams.h>
#include <ace/OS_NS_errno.h>
#include <ace/SString.h>
#include <ace/ACE.h>
#include <ace/OS_NS_unistd.h>
#include <ace/OS_NS_sys_stat.h>
#include <ace/OS_NS_sys_resource.h>
#include <ace/OS_NS_fcntl.h>

#include "proxy.h"
#include "UNIX_svc_loader.h"
#include "lorica/ConfigBase.h"
#include "lorica/FileConfig.h"
#include "lorica/debug.h"
#include "lorica/MapperRegistry.h"

#if !defined (ACE_WIN32)
typedef enum {
	EXIT_DAEMON = 0, /* we are the daemon                  */
	EXIT_OK = 1,	 /* caller must exit with EXIT_SUCCESS */
	EXIT_ERROR = 2	 /* caller must exit with EXIT_FAILURE */
} daemon_exit_t;

static daemon_exit_t
become_daemon(const bool NoFork,
	      const bool Debug)
{
	struct sigaction sig_act;
	pid_t pid = -1;
#if !defined (LORICA_DARWIN)
	struct rlimit rl;
	unsigned int n;
	int fd0;
	int fd1;
	int fd2;
#endif //LORICA_DARWIN
	/* reset the file mode mask */
	ACE_OS::umask(0);

	/*
	 * A process that has terminated, but has not yet been waited
	 * for, is a zombie.  On the other hand, if the parent dies
	 * first, init (process 1) inherits the child and becomes its
	 * parent.
	 *
	 * (*) Citation from <http://www.win.tue.nl/~aeb/linux/lk/lk-10.html>
	 */

	if (NoFork)
		goto fork_done;

	/* fork off the parent process to create the session daemon */
	pid = ACE_OS::fork();
	switch (pid) {
	case -1 :
		return EXIT_ERROR;
	case 0 :
		/* We are the intermediate child.
		 * 
		 * Now fork once more and try to ensure that this
		 * intermediate child exits before the final child so
		 * that the final child is adopted by init and does
		 * not become a zombie. This is racy as the final
		 * child concievably could terminate (and become a
		 * zombie) before this child exits. Knock on wood...
		 */

		if ((ACE_OS::setsid()) < 0)
			return EXIT_ERROR;

		sig_act.sa_handler = SIG_IGN;
		sigemptyset(&sig_act.sa_mask);
		sig_act.sa_flags = 0;
		if (ACE_OS::sigaction(SIGHUP, &sig_act, NULL))
			return EXIT_ERROR;

		pid = fork();
		switch (pid) {
		case -1 :
			return EXIT_ERROR;
		case 0 :
			break;
		default :
			return EXIT_OK;
		}

		/*
		 * (0 == pid) we are the final child
		 */

		/* sleep for 1 second to give the parent plenty of
		   time to exit */
		ACE_OS::sleep(1);

		break;
	default :
		/* wait for intermediate child */
		waitpid(pid, NULL, 0);

		return EXIT_OK;
	}
 fork_done:

	/*
	 * We are now effectively the daemon and must continue
	 * to prep the daemon process for operation
	 */

/* 
 * We should not do any of the good things below when running
 * on darwin. See launchd.plist(5).
 */
#if !defined(LORICA_DARWIN)
	if (!Debug) { // change the working directory
		if ((ACE_OS::chdir("/")) < 0)
			return EXIT_ERROR;
	}

	// close any and all open file descriptors
	if (ACE_OS::getrlimit(RLIMIT_NOFILE, &rl))
		return EXIT_ERROR;
	if (RLIM_INFINITY == rl.rlim_max)
		rl.rlim_max = 1024;
	for (n = 0; n < rl.rlim_max; n++) {
		if (Debug) {
			if (STDOUT_FILENO == n)
				continue;
			if (STDERR_FILENO == n)
				continue;
		}
		if (ACE_OS::close(n) && (EBADF != errno))
			return EXIT_ERROR;
	}

	if (!Debug) { // attach file descriptors STDIN_FILENO(0),
		      // STDOUT_FILENO(1) and STDERR_FILENO(2) to
		      // /dev/null
		fd0 = ACE_OS::open("/dev/null", O_RDWR);
		fd1 = ACE_OS::dup2(fd0, 1);
		fd2 = ACE_OS::dup2(fd0, 2);
	} else
		fd0 = ACE_OS::open("/dev/null", O_RDWR);
	if (0 != fd0)
		return EXIT_ERROR;
#endif // LORICA_DARWIN
	if (!Debug
	    && (ACE_OS::mkdir(LORICA_CACHE_DIR, 0755))
	    && (EEXIST != errno))
		return EXIT_ERROR;

	return EXIT_DAEMON;
}

Lorica::UNIX_Service_Loader::UNIX_Service_Loader()
	: no_fork_(false)
{
}

Lorica::UNIX_Service_Loader::~UNIX_Service_Loader(void)
{
}

void
Lorica::UNIX_Service_Loader::print_usage_and_die(const ACE_TCHAR *prog)
{
	ACE_DEBUG((LM_INFO,
		   ACE_TEXT("Usage: %s")
		   ACE_TEXT(" -V -n -d -f <file> -c <level> -l <level>\n")
		   ACE_TEXT(" -V: Print the version\n")
		   ACE_TEXT(" -n: No fork - Run as a regular application\n")
		   ACE_TEXT(" -d: Debug - Use current directory as working directory\n")
		   ACE_TEXT(" -f: <file> Configuration file, default is \"/etc/lorica.conf\"\n")
		   ACE_TEXT(" -c: <level; default:0> Turn on CORBA debugging\n")
		   ACE_TEXT(" -l: <level; default:0> Turn on Lorica debugging\n"),
		   prog,
		   0));
	ACE_OS::exit(1);
}

int
Lorica::UNIX_Service_Loader::parse_args(int argc,
				   ACE_TCHAR *argv[])
{
	if (!argc)
		return 0;

	ACE_Get_Opt get_opt(argc, argv, ACE_TEXT("Vndf:c:l:"));
	int c;
	const ACE_TCHAR *tmp;

	while ((c = get_opt()) != -1) {
		switch (c) {
		case 'n':
			no_fork_ = true;
			ACE_DEBUG((LM_INFO, ACE_TEXT("(%T) %N:%l - Lorica is in standalone mode\n")));
			break;
		case 'V':
			ACE_OS::printf("Lorica version %s\n", VERSION);
			return 1;
		case 'd':
			debug_ = true;
			ACE_DEBUG((LM_INFO, ACE_TEXT("(%T) %N:%l - Lorica is in debug mode\n")));
			break;
		case 'f':
			config_file_ = get_opt.opt_arg();
			ACE_DEBUG((LM_INFO, ACE_TEXT("(%T) %N:%l - Lorica configuration file = %s\n"), ACE_TEXT(config_file_.c_str())));
			break;
		case 'c':
			tmp = get_opt.opt_arg();
			if (tmp != 0)
				corba_debug_level_ = ACE_OS::atoi(tmp);
			ACE_DEBUG((LM_INFO, ACE_TEXT("(%T) %N:%l - CORBA debug level = %d\n"), corba_debug_level_));
			break;
		case 'l':
			tmp = get_opt.opt_arg();
			if (tmp != 0) 
				Lorica_debug_level = ACE_OS::atoi(tmp);
			ACE_DEBUG((LM_INFO, ACE_TEXT("(%T) %N:%l - Lorica debug level = %d\n"), Lorica_debug_level));
			break;
		default:
			print_usage_and_die(argv[0]);
			break;
		}
	}

	return 0;
}

Lorica::Proxy *
Lorica::UNIX_Service_Loader::init_proxy(void)
{
	Lorica::FileConfig *config = Lorica::FileConfig::instance();
	try {
		config->init(config_file_, corba_debug_level_);
	}
	catch (const Lorica::FileConfig::InitError &) {
		ACE_ERROR((LM_ERROR, ACE_TEXT("%N:%l - proxy could not read %s.\n"),
			   this->config_file_.c_str()));
		return 0;
	}

	std::auto_ptr<Proxy> proxy(new Proxy(debug_));
	if (debug_) {
		proxy->local_pid_file ("lorica.pid");
		proxy->local_ior_file ("lorica.ior");
	}
	
	try {
		proxy->configure(*config);

		return proxy.release();
	}
	catch (const Lorica::Proxy::InitError &) {
		ACE_ERROR((LM_ERROR, ACE_TEXT("%N:%l - proxy initialization failed.\n")));
	}

	return 0;
}

int
Lorica::UNIX_Service_Loader::run_service(void)
{

	daemon_exit_t dstat = 
		become_daemon(no_fork_, debug_);
	switch (dstat) {
	case EXIT_DAEMON :
		break;
	case EXIT_OK :
		exit(EXIT_SUCCESS);
	case EXIT_ERROR :
	default :
		exit(EXIT_FAILURE);
	}
	{
		if (!this->is_service())
			this->reset_log();

 		ACE_DEBUG((LM_INFO, ACE_TEXT("(%T) Lorica is starting up\n")));

		try {
			std::auto_ptr<Proxy>proxy (this->init_proxy());
			if (!proxy.get()) {
				ACE_ERROR((LM_ERROR, "(%T) %N:%l - could not initialize proxy\n"));
				return -1;
			}

			try {
				proxy->activate();
			}
			catch (CORBA::Exception & ex) {
				ACE_DEBUG((LM_ERROR, ACE_TEXT("(%T) %N:%l - %s\n"), ex._info().c_str()));
				return -1;
			}
			catch (...) {
				ACE_ERROR((LM_ERROR, "(%T) %N:%l - caught an otherwise unknown exception while activating proxy\n"));
				return -1;
			}

			try {
				proxy->wait();
			}
			catch (CORBA::Exception & ex) {
				ACE_DEBUG((LM_ERROR, ACE_TEXT("(%T) %N:%l - %s\n"), ex._info().c_str()));
				return -1;
			}
			catch (...) {
				ACE_ERROR((LM_ERROR, "(%T) %N:%l - caught an otherwise unknown exception while waiting for proxy\n"));
				return -1;
			}
		}
		catch (CORBA::Exception & ex) {
			ACE_DEBUG((LM_ERROR, ACE_TEXT("(%T) %N:%l - %s\n"), ex._info().c_str()));
			return -1;
		}
		catch (...) {
			ACE_ERROR((LM_ERROR, "(%T) %N:%l - caught an otherwise unknown exception while initializing proxy\n"));
			return -1;
		}
	}

	return EXIT_SUCCESS;
}

int
Lorica::UNIX_Service_Loader::run_standalone(void)
{
	std::auto_ptr<Proxy>proxy (this->init_proxy());

	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T) Lorica [%P] running as a standalone application \n")));

	proxy->activate();
	proxy->wait();
	proxy->destroy();
	return 0;
}

int
Lorica::UNIX_Service_Loader::execute (void)
{
	return this->run_service();
}

#endif // ACE_WIN32

