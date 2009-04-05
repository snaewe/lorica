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
become_daemon(const bool NoFork, const bool Debug)
{
	struct sigaction sig_act;
	pid_t pid = -1;

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
	if (!Debug && // change the working directory
	    ACE_OS::chdir("/") < 0)
		return EXIT_ERROR;

	// close any and all open file descriptors
	int last = Debug ? STDERR_FILENO+1 : 0;

	for (int n = ACE::max_handles() -1; n >= last; n--) {
		if (ACE_OS::close(n) && (EBADF != errno))
			return EXIT_ERROR;
	}
	int fd0 = ACE_OS::open("/dev/null", O_RDWR);
	if (0 != fd0)
		return EXIT_ERROR;

	if (!Debug) { // attach file descriptors STDIN_FILENO(0),
		      // STDOUT_FILENO(1) and STDERR_FILENO(2) to
		      // /dev/null
		ACE_OS::dup2(fd0, 1);
		ACE_OS::dup2(fd0, 2);
	}
	
#endif // LORICA_DARWIN
	if (!Debug
	    && (ACE_OS::mkdir(LORICA_CACHE_DIR, 0755))
	    && (EEXIST != errno)) 
		return EXIT_ERROR;

	return EXIT_DAEMON;
}

static int
lock_fd(const int fd)
{
	struct flock lock;

	lock.l_type = F_WRLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;

	if (-1 == fd)
		return -1;

	return fcntl(fd, F_SETLK, &lock);
}

static int
unlock_fd(const int fd)
{
	struct flock lock;

	lock.l_type = F_UNLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;

	if (-1 == fd)
		return -1;

	return fcntl(fd, F_SETLK, &lock);
}

/*
 * Will attempt to lock the PID file and write the
 * pid into it. This function will return false for
 * all errors. Callee is responsible for closing *fd
 * if (*fd != -1).
 */
static bool
get_process_lock(int & fd,
                 const char *path)
{
	std::string pid_file;
	char buf[32] = { '\0' };
	int p = 0;
	ssize_t w = 0;

	fd = -1;
	if (!path || !strlen(path))
		return false;

	fd = open(path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (-1 == fd)
		return false;

	// lock it
	if (lock_fd(fd))
		return false;

	// we have the lock
	if (ftruncate(fd, 0))
		return false;

	/* write pid */
	p = snprintf(buf, sizeof(buf), "%d", getpid());
	w = write(fd, buf, strlen(buf) + sizeof(char));
	p += sizeof(char);
	if (p != (int)w)
		return false;

	if (fsync(fd))
		return false;

	return true;
}

bool
Lorica::UNIX_Service_Loader::get_lock(const char *lock_file_path)
{
	// take the lock and write the pid
	if (get_process_lock(this->lock_fd_, lock_file_path))
		return true;

	if (-1 != this->lock_fd_) {
		close(this->lock_fd_);
		this->lock_fd_ = -1;
	}
	return false;
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
	std::string def_pid(LORICA_PID_FILE);
	if (this->debug_)
		def_pid = "lorica.pid";

	Lorica::FileConfig *config = Lorica::FileConfig::instance();
	try {
		config->init(config_file_, corba_debug_level_);
		this->pid_file_ = config->get_value("PID_FILE", def_pid);
		this->num_threads_ = 
			(int)config->get_long_value ("Proxy_Threads",1);
	
	}
	catch (const Lorica::FileConfig::InitError &) {
		ACE_ERROR((LM_ERROR, 
			   ACE_TEXT("%N:%l - proxy could not read %s.\n"),
			   this->config_file_.c_str()));
		return 0;
	}

	std::auto_ptr<Proxy> proxy(new Proxy(debug_));
	std::string def_ior(LORICA_IOR_FILE);
	if (debug_)
		def_ior = "lorica.ior";
	
	try {
		proxy->configure(*config, def_ior);

		return proxy.release();
	}
	catch (const Lorica::Proxy::InitError &) {
		ACE_ERROR((LM_ERROR, ACE_TEXT("%N:%l - proxy initialization failed.\n")));
	}

	return 0;
}

int
Lorica::UNIX_Service_Loader::run_i (void)
{
	if (!this->is_service())
		this->reset_log();
	std::auto_ptr<Proxy>proxy (this->init_proxy());

	if (!this->get_lock(this->pid_file_.c_str())) {
		ACE_ERROR ((LM_ERROR,
			    ACE_TEXT ("(%T) %N:%l - ")
			    ACE_TEXT ("Could not lock pidfile\n")));
		return EXIT_FAILURE;
	}

	// Output the pid file indicating we are running
	FILE *output_file= ACE_OS::fopen(this->pid_file_.c_str(), "w");
	if (output_file == 0) {
		ACE_ERROR_RETURN((LM_ERROR,
				  "%N:%l - cannot open output file for writing PID: %s\n",
				  this->pid_file_.c_str()),
				 1);
	}
	ACE_OS::fprintf(output_file, "%d\n", ACE_OS::getpid());
	ACE_OS::fclose(output_file);

	try {
		if (!proxy.get()) {
			ACE_ERROR((LM_ERROR, 
				   ACE_TEXT ("(%T) %N:%l - ")
				   ACE_TEXT ("could not initialize proxy\n")));
			return EXIT_FAILURE;
		}

		proxy->activate(this->proxy_thr_flags_,
				this->num_threads_);
		proxy->wait();
		proxy->destroy();

	}
	catch (CORBA::Exception & ex) {
		ACE_DEBUG((LM_ERROR, 
			   ACE_TEXT("(%T) %N:%l - %C\n"), 
			   ex._info().c_str()));
		return EXIT_FAILURE;
	}
	catch (...) {
		ACE_ERROR((LM_ERROR, 
			   ACE_TEXT("(%T) %N:%l - caught an otherwise unknown")
			   ACE_TEXT(" exception while initializing proxy\n")));
		return EXIT_FAILURE;
	}

	// release the lock
	if (-1 != this->lock_fd_) {
		unlock_fd(this->lock_fd_);
		close(this->lock_fd_);
		this->lock_fd_ = -1;
	}
	ACE_OS::unlink (this->pid_file_.c_str());

	return EXIT_SUCCESS;
}


int
Lorica::UNIX_Service_Loader::run_service(void)
{

	daemon_exit_t dstat = 
		become_daemon(this->no_fork_, this->debug_);
	switch (dstat) {
	case EXIT_DAEMON :
		break;
	case EXIT_OK :
		exit(EXIT_SUCCESS);
	case EXIT_ERROR :
	default :
		exit(EXIT_FAILURE);
	}

	ACE_DEBUG((LM_INFO, ACE_TEXT("(%T) Lorica is starting up\n")));

	return this->run_i();
}

int
Lorica::UNIX_Service_Loader::run_standalone(void)
{
	ACE_DEBUG((LM_DEBUG,
		   ACE_TEXT("(%T) Lorica [%P] ")
		   ACE_TEXT("running as a standalone application \n")));
	return this->run_i ();
}

int
Lorica::UNIX_Service_Loader::execute (void)
{
	return this->run_service();
}

#endif // ACE_WIN32

