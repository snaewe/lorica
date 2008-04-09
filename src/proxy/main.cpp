/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 *    Lorica source file.
 *    Copyright (C) 2007-2008 OMC Denmark ApS.
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
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

#include "proxy.h"
#include "ntsvc.h"
#include "lorica/ConfigBase.h"
#include "lorica/FileConfig.h"
#include "lorica/debug.h"
#include "lorica/MapperRegistry.h"

#ifndef ACE_WIN32
#include <unistd.h>
#endif

#ifndef ACE_WIN32
typedef enum {
	EXIT_DAEMON = 0, /* we are the daemon                  */
	EXIT_OK = 1,	 /* caller must exit with EXIT_SUCCESS */
	EXIT_ERROR = 2,	 /* caller must exit with EXIT_FAILURE */
} daemon_exit_t;

static daemon_exit_t
become_daemon(const bool NoFork,
	      const bool Debug)
{
	struct sigaction sig_act;
	struct rlimit rl;
	unsigned int n;
	pid_t pid = -1;
	int fd0;
	int fd1;
	int fd2;

	/* reset the file mode mask */
	umask(0);

	/*
	 * A process that has terminated but has not yet been waited for is a zombie.
	 * On the other hand, if the parent dies first, init (process 1) inherits the
	 * child and becomes its parent.
	 *
	 * (*) Citation from <http://www.win.tue.nl/~aeb/linux/lk/lk-10.html>
	 */

	if (NoFork)
		goto fork_done;

	/* fork off the parent process to create the session daemon */
	pid = fork();
	switch (pid) {
	case -1 :
		return EXIT_ERROR;
	case 0 :
		/* We are the intermediate child.
		 * Now fork once more and try to ensure that this intermediate child
		 * exits before the final child so that the final child is adopted
		 * by init and does not become a zombie. This is racy as the final
		 * child concievably could terminate (and become a zombie) before
		 * this child exits. Knock on wood...
		 */

		if ((setsid()) < 0)
			return EXIT_ERROR;

		sig_act.sa_handler = SIG_IGN;
		sigemptyset(&sig_act.sa_mask);
		sig_act.sa_flags = 0;
		if (sigaction(SIGHUP, &sig_act, NULL))
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

		/* sleep for 1 second to give the parent plenty of time to exit */
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

	if (!Debug) { // change the working directory
		if ((chdir("/")) < 0)
			return EXIT_ERROR;
	}

	// close any and all open file descriptors
	if (getrlimit(RLIMIT_NOFILE, &rl))
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
		if (close(n) && (EBADF != errno))
			return EXIT_ERROR;
	}

	if (!Debug) { // attach file descriptors STDIN_FILENO(0), STDOUT_FILENO(1) and STDERR_FILENO(2) to /dev/null
		fd0 = open("/dev/null", O_RDWR);
		fd1 = dup2(fd0, 1);
		fd2 = dup2(fd0, 2);
	} else
		fd0 = open("/dev/null", O_RDWR);
	if (0 != fd0)
		return EXIT_ERROR;

	if (!Debug
	    && (mkdir(LORICA_CACHE_DIR, 0755))
	    && (EEXIST != errno))
		return EXIT_ERROR;

	return EXIT_DAEMON;
}
#endif // !ACE_WIN32

namespace Lorica
{
	class Service_Loader {
	public:
		enum SERVICE_COMMAND {
			SC_NONE,
			SC_INSTALL,
			SC_REMOVE,
			SC_START,
			SC_STOP
		};

		Service_Loader(const char *progname);

		~Service_Loader(void);

		int run(int argc,
			ACE_TCHAR *argv[]);

		int parse_args(int argc,
			       ACE_TCHAR *argv[]);

		int run_service_command(void);

		int run_service(void);

		int run_standalone(void);

		void print_usage_and_die(void);

		bool is_service(void);

		Proxy *init_proxy(void);

	private:
		ACE_CString program_name;

		/// SC_NONE, SC_INSTALL, SC_REMOVE, ...
		SERVICE_COMMAND service_command_;
		bool no_fork_;
		bool debug_;
		std::string config_file_;
		int corba_debug_level_;
	};
}

Lorica::Service_Loader::Service_Loader(const char *progname)
	: program_name(progname),
	  service_command_(SC_NONE),
	  no_fork_(false),
	  debug_(false),
	  config_file_(LORICA_CONF_FILE),
	  corba_debug_level_(0)
{
}

Lorica::Service_Loader::~Service_Loader(void)
{
}

void
Lorica::Service_Loader::print_usage_and_die(void)
{
#if defined (ACE_WIN32)
	ACE_DEBUG((LM_INFO,
		   ACE_TEXT("Usage: %s")
		   ACE_TEXT(" -i -r -t -k -d -f -c -l\n")
		   ACE_TEXT(" -i: Install this program as an NT service\n")
		   ACE_TEXT(" -r: Remove this program from the Service Manager\n")
		   ACE_TEXT(" -s: Start the service\n")
		   ACE_TEXT(" -k: Stop the service\n")
		   ACE_TEXT(" -d: Debug; run as a regular application\n")
		   ACE_TEXT(" -f: <file> Configuration file, default is \"lorica.conf\"\n")
		   ACE_TEXT(" -c: <level> Turn on CORBA debugging, default 0\n")
		   ACE_TEXT(" -l: <level> Turn on Lorica debugging, default 0\n"),
		   program_name.c_str(),
		   0));
#else
	ACE_DEBUG((LM_INFO,
		   ACE_TEXT("Usage: %s")
		   ACE_TEXT(" -V -n -d -f <file> -c <level> -l <level> -n\n")
		   ACE_TEXT(" -V: Print the version\n")
		   ACE_TEXT(" -n: No fork - Run as a regular application\n")
		   ACE_TEXT(" -d: Debug - Use current directory as working directory\n")
		   ACE_TEXT(" -f: <file> Configuration file, default is \"/etc/lorica.conf\"\n")
		   ACE_TEXT(" -c: <level; default:0> Turn on CORBA debugging\n")
		   ACE_TEXT(" -l: <level; default:0> Turn on Lorica debugging\n"),
		   program_name.c_str(),
		   0));
#endif /* ACE_WIN32 */
	ACE_OS::exit(1);
}

int
Lorica::Service_Loader::parse_args(int argc,
				   ACE_TCHAR *argv[])
{
	if (!argc)
		return 0;

#if defined (ACE_WIN32)
	ACE_Get_Opt get_opt(argc, argv, ACE_TEXT("irtkdf:c:l:"));
#else
	ACE_Get_Opt get_opt(argc, argv, ACE_TEXT("Vndf:c:l:"));
#endif

	int c;
	const ACE_TCHAR *tmp;

	while ((c = get_opt()) != -1) {
		switch (c) {
#if defined (ACE_WIN32)
		case 'i':
			service_command_ = SC_INSTALL;
			break;
		case 'r':
			service_command_ = SC_REMOVE;
			break;
		case 't':
			service_command_ = SC_START;
			break;
		case 'k':
			service_command_ = SC_STOP;
			break;
#else /* !ACE_WIN32 */
		case 'V':
			ACE_DEBUG((LM_INFO,
				   ACE_TEXT("Lorica version %s\n"),
				   ACE_TEXT(VERSION),
				   0));
		case 'n':
			no_fork_ = true;
			break;
#endif
		case 'd':
			debug_ = true;
			break;
		case 'f':
			config_file_ = get_opt.opt_arg();
			break;
		case 'c':
			tmp = get_opt.opt_arg();
			if (tmp != 0)
				corba_debug_level_ = ACE_OS::atoi(tmp);
			break;
		case 'l':
			tmp = get_opt.opt_arg();
			if (tmp != 0)
				Lorica_debug_level = ACE_OS::atoi(tmp);
			break;
		default:
			print_usage_and_die();
			break;
		}
	}

	return 0;
}

int
Lorica::Service_Loader::run_service_command(void)
{
#if defined (ACE_WIN32)
	Lorica::SERVICE::instance()->name(ACE_TEXT("Lorica"),
					  ACE_TEXT("Lorica IIOP Firewall"));

	if (service_command_ == SC_NONE)
		return 0;

	int result = 1;
	switch (service_command_) {
	case SC_INSTALL:
		{
			char pathname[_MAX_PATH] = { '\0' };
			DWORD length = ACE_TEXT_GetModuleFileName(NULL,
								  pathname,
								  _MAX_PATH);

			// Append the command used for running as a service
			ACE_OS::strcat(pathname, ACE_TEXT(" -s"));
			if (-1 == Lorica::SERVICE::instance()->insert(SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, pathname)) {
				ACE_ERROR ((LM_ERROR, ACE_TEXT("%s\n"), ACE_TEXT("Error inserting service")));
				result = -1;
			}

			break;
		}
	case SC_REMOVE :
		if (-1 == Lorica::SERVICE::instance()->remove()) {
			ACE_ERROR ((LM_ERROR, ACE_TEXT("%s\n"), ACE_TEXT("remove")));
			result = -1;
		}
		break;
	case SC_START :
		if (-1 == Lorica::SERVICE::instance()->start_svc()) {
			ACE_ERROR ((LM_ERROR, ACE_TEXT("%s\n"), ACE_TEXT("start")));
			result = -1;
		}
		break;
	case SC_STOP :
		if (-1 == Lorica::SERVICE::instance()->stop_svc()) {
			ACE_ERROR ((LM_ERROR, ACE_TEXT("%s\n"), ACE_TEXT("stop")));
			result = -1;
		}
		break;
	default:
		result = -1;
	}

	return result;;
#else /* !ACE_WIN32 */
	return 0;
#endif /* ACE_WIN32 */
}

#if defined (ACE_WIN32)
// Define a function to handle Ctrl+C to cleanly shut this down.
static BOOL __stdcall
ConsoleHandler(DWORD ctrlType)
{
	ACE_UNUSED_ARG(ctrlType);
	Lorica::SERVICE::instance()->handle_control(SERVICE_CONTROL_STOP);
	return TRUE;
}

ACE_NT_SERVICE_DEFINE(Lorica,
		      Lorica::NT_Service,
		      ACE_TEXT ("Lorica IIOP Firewall"));
#endif /* ACE_WIN32 */

Lorica::Proxy *
Lorica::Service_Loader::init_proxy(void)
{
	Lorica::FileConfig *config = Lorica::FileConfig::instance();
	try {
		config->init(config_file_, corba_debug_level_);
	}
	catch (const Lorica::FileConfig::InitError &) {
		ACE_ERROR((LM_ERROR, ACE_TEXT("Proxy could not read %s.\n"),
			   this->config_file_.c_str()));
		return 0;
	}

	std::auto_ptr<Proxy> proxy(new Proxy(debug_));
	try {
		proxy->configure(*config);

		return proxy.release();
	}
	catch (const Lorica::Proxy::InitError &) {
		ACE_ERROR((LM_ERROR, ACE_TEXT("Proxy initialization failed.\n")));
	}

	return 0;
}

int
Lorica::Service_Loader::run_service(void)
{
#if defined (ACE_WIN32)
	try {
		Lorica::SERVICE::instance()->proxy(init_proxy());
	}
	catch (...) {
		ACE_ERROR((LM_ERROR,
			   ACE_TEXT("%s\n"),
			   ACE_TEXT("Couldn't start Lorica server - init_proxy() failed")));
		return -1;
	}

	ACE_DEBUG((LM_INFO, ACE_TEXT("Starting Lorica service\n")));
	ACE_NT_SERVICE_RUN(Lorica,
			   Lorica::SERVICE::instance(),
			   ret);
	if (0 == ret) {
		ACE_ERROR((LM_ERROR, ACE_TEXT("%s\n"), ACE_TEXT("Couldn't start Lorica server - Service start failed")));
	}

	return ret;

#else /* !ACE_WIN32 */

	daemon_exit_t dstat = EXIT_ERROR;

	dstat = become_daemon(no_fork_, debug_);
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
		ACE_DEBUG((LM_INFO, ACE_TEXT("Lorica is starting up\n")));

		try {
			std::auto_ptr<Proxy>proxy (this->init_proxy());
			if (!proxy.get()) {
				ACE_ERROR((LM_ERROR, "(%P|%t) Lorica::Service_Loader::run_service Could not initialize proxy\n"));
				return -1;
			}

			try {
				proxy->activate();
			}
			catch (CORBA::Exception & ex) {
				ex._tao_print_exception("(%P|%t) Lorica::Service_Loader::run_service Caught a CORBA exception while activating proxy\n");
				return -1;
			}
			catch (...) {
				ACE_ERROR((LM_ERROR, "(%P|%t) Lorica::Service_Loader::run_service Caught an otherwise unknown exception while activating proxy\n"));
				return -1;
			}

			try {
				proxy->wait();;
			}
			catch (CORBA::Exception & ex) {
				ex._tao_print_exception("(%P|%t) Lorica::Service_Loader::run_service Caught a CORBA exception while waiting for proxy\n");
				return -1;
			}
			catch (...) {
				ACE_ERROR((LM_ERROR, "(%P|%t) Lorica::Service_Loader::run_service Caught an otherwise unknown exception while waiting for proxy\n"));
				return -1;
			}
		}
		catch (CORBA::Exception & ex) {
			ex._tao_print_exception("(%P|%t) Lorica::Service_Loader::run_service Caught a CORBA exception while initializing proxy\n");
			return -1;
		}
		catch (...) {
			ACE_ERROR((LM_ERROR, "(%P|%t) Lorica::Service_Loader::run_service Caught an otherwise unknown exception while initializing proxy\n"));
			return -1;
		}
	}

	return EXIT_SUCCESS;
#endif /* ACE_WIN32 */
}

int
Lorica::Service_Loader::run_standalone(void)
{
	std::auto_ptr<Proxy>proxy (this->init_proxy());

	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Lorica [%P] running as a standalone application \n")));

#ifdef ACE_WIN32
	SetConsoleCtrlHandler(&ConsoleHandler, true);
#endif
	proxy->activate();
	proxy->wait();

	return 0;
}

bool
Lorica::Service_Loader::is_service(void)
{
	return !debug_;
}

int
ACE_TMAIN(int argc,
	  ACE_TCHAR *argv[])
{
	Lorica::Service_Loader lorica(argv[0]);
	int result = 0;

#ifdef ACE_WIN32
	{ // Change current working directory
		char *c = NULL;
		char wdir[_MAX_PATH] = { '\0' };

		memcpy(wdir, argv[0], strlen(argv[0]));
		c = &wdir[strlen(argv[0])-1];
		while ('\\' != *c)
			*(c--) = '\0';
		*c = '\0';
		SetCurrentDirectory(wdir);
	}
#endif
	result = lorica.parse_args(argc, argv);
	if (result < 0)
		exit(EXIT_FAILURE);  // Error
	else if (result > 0)
		exit(EXIT_SUCCESS);  // No error, but we should exit anyway.

	// initialize ACE logging framework
	if (!lorica.is_service()) { // ==> if (lorica.debug_) a.k.a. put all logging output in the log file
		size_t len = strlen("Lorica");
		char *name = (char*)malloc(len + 4 + 1);
#ifdef ACE_WIN32
		sprintf_s(name, len + 4 + 1, "%s.log", "Lorica");
#else
		snprintf(name, len + 4 + 1, "%s.log", "Lorica");
#endif
		ofstream *output_file = new ofstream(ACE_TEXT(name), ios::out);
		free(name);

		if (output_file && (output_file->rdstate() == ios::goodbit))
			ACE_LOG_MSG->msg_ostream(output_file, 1);

		ACE_LOG_MSG->open("Lorica", ACE_Log_Msg::STDERR | ACE_Log_Msg::OSTREAM);
	} else
		ACE_LOG_MSG->open("Lorica", ACE_Log_Msg::SYSLOG);

	ACE_DEBUG((LM_INFO, ACE_TEXT("Lorica initializing\n")));

	result = lorica.run_service_command();
	if (result < 0)
		exit(EXIT_FAILURE);  // Error
	else if (result > 0)
		exit(EXIT_SUCCESS);  // No error, but we should exit anyway.

#if defined (ACE_WIN32)
	if (lorica.is_service())
		result = lorica.run_service();
	else
		result = lorica.run_standalone();
#else
	result = lorica.run_service();
#endif

	ACE_DEBUG((LM_INFO, ACE_TEXT("Lorica is shutting down\n")));
	exit(result);
}
