/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 *    Lorica source file.
 *    Copyright (C) 2007 OMC Denmark ApS.
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

#include "proxy.h"
#include "ntsvc.h"
#include "lorica/Config.h"
#include "lorica/FileConfig.h"
#include "lorica/debug.h"
#include "lorica/MapperRegistry.h"

#include "ace/Get_Opt.h"
#include "ace/streams.h"
#include "ace/OS_NS_errno.h"
#include "ace/SString.h"
#include "ace/ACE.h"


//ACE_STATIC_SVC_REQUIRE(Lorica_MapperRegistry)

namespace Lorica
{
	class Service_Loader
	{
	public:
		enum SERVICE_COMMAND {
			SC_NONE,
			SC_INSTALL,
			SC_REMOVE,
			SC_START,
			SC_STOP
		};

		Service_Loader (void);
		~Service_Loader (void);

		int run (int argc, ACE_TCHAR* argv[]);

		int parse_args (int argc,
				ACE_TCHAR* argv[]);
		int run_service_command ();
		int run_service ();
		int run_standalone ();
		void print_usage_and_die ();
		bool is_service ();
		Proxy *init_proxy ();

	private:
		ACE_CString program_name;

		/// SC_NONE, SC_INSTALL, SC_REMOVE, ...
		SERVICE_COMMAND service_command_;
		bool debug_;

		std::string config_file_;
		int corba_debug_level_;
	};
}

Lorica::Service_Loader::Service_Loader (void)
	: program_name (ACE_TEXT("lorica")),
	  service_command_ (SC_NONE),
	  debug_ (false),
	  config_file_ ("lorica.conf"),
	  corba_debug_level_ (0)
{
}

Lorica::Service_Loader::~Service_Loader (void)
{
}

void
Lorica::Service_Loader::print_usage_and_die (void)
{
#if defined (ACE_WIN32)
	ACE_DEBUG ((LM_INFO,
		    ACE_TEXT("Usage: %s")
		    ACE_TEXT(" -i -r -t -k -d -f -c -l\n")
		    ACE_TEXT(" -i: Install this program as an NT service\n")
		    ACE_TEXT(" -r: Remove this program from the Service Manager\n")
		    ACE_TEXT(" -s: Start the service\n")
		    ACE_TEXT(" -k: Stop the service\n")
		    ACE_TEXT(" -d: Debug; run as a regular application\n")
		    ACE_TEXT(" -f: <required; default:lorica.conf> Configuration file.\n")
		    ACE_TEXT(" -c: <level; default:0> Turn on CORBA debugging\n")
		    ACE_TEXT(" -l: <level; default:0> Turn on Lorica debugging"),
		    program_name.c_str(),
		    0));
#else
	ACE_DEBUG ((LM_INFO,
		    ACE_TEXT("Usage: %s")
		    ACE_TEXT(" -d -f -c -l\n")
		    ACE_TEXT(" -d: Debug; run as a regular application\n")
		    ACE_TEXT(" -f: <required; default:lorica.conf> Configuration file.\n")
		    ACE_TEXT(" -c: <level; default:0> Turn on CORBA debugging\n")
		    ACE_TEXT(" -l: <level; default:0> Turn on Lorica debugging"),
		    program_name.c_str(),
		    0));
#endif /* ACE_WIN32 */
	ACE_OS::exit(1);
}

int
Lorica::Service_Loader::parse_args (int argc, ACE_TCHAR* argv[])
{
#if defined (ACE_WIN32)
	ACE_Get_Opt get_opt (argc, argv, ACE_TEXT ("irtkdf:c:l:"));
#else
	ACE_Get_Opt get_opt (argc, argv, ACE_TEXT ("df:c:l:"));
#endif /* ACE_WIN32 */

	int c;
	const ACE_TCHAR *tmp;

	while ((c = get_opt ()) != -1) {
		switch (c)
		{
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
#endif /* ACE_WIN32 */
		case 'd':
			debug_ = true;
			break;
		case 'f':
			config_file_ = get_opt.opt_arg ();
			break;
		case 'c':
			tmp = get_opt.opt_arg ();
			if (tmp != 0) {
				corba_debug_level_ = ACE_OS::atoi (tmp);
			}
			break;
		case 'l':
			tmp = get_opt.opt_arg ();
			if (tmp != 0) {
				Lorica_debug_level = ACE_OS::atoi (tmp);
			}
			break;
		default:
			print_usage_and_die ();
			break;
		}
	}

#if defined (ACE_WIN32)
	if ((SC_NONE == service_command_) && (!debug_)) {
		ACE_TEXT("You must use a service command if you are running in non-debug mode");
		ACE_OS::exit(1);
	}
#endif /* ACE_WIN32 */

	return 0;
}

int
Lorica::Service_Loader::run_service_command ()
{
	if (service_command_ == SC_NONE)
		return 0;
#if defined (ACE_WIN32)
	Lorica::SERVICE::instance ()->name (ACE_TEXT("Lorica"),
					    ACE_TEXT("Lorica CORBA Firewall Proxy"));
	int result = 1;
	switch (service_command_)
	{
	case SC_INSTALL:
	{
		// TODO: path for the executable is not defined yet.
		const DWORD MAX_PATH_LENGTH = 4096;
		char pathname[MAX_PATH_LENGTH];

		DWORD length = ACE_TEXT_GetModuleFileName(0,
							  pathname,
							  MAX_PATH_LENGTH);
		// Append the command used for running the implrepo as a service
		ACE_OS::strcat (pathname, ACE_TEXT (" -s"));
		if (-1 == Lorica::SERVICE::instance ()->insert
		    (SERVICE_DEMAND_START,
		     SERVICE_ERROR_NORMAL, pathname))
		{
			ACE_ERROR ((LM_ERROR, ACE_TEXT ("%p\n"), ACE_TEXT ("insert")));
			result = -1;
		}

		break;
	}
	case SC_REMOVE :
		if (-1 == Lorica::SERVICE::instance ()->remove ())
		{
			ACE_ERROR ((LM_ERROR, ACE_TEXT ("%p\n"), ACE_TEXT ("remove")));
			result = -1;
		}
		break;
	case SC_START :
		if (-1 == Lorica::SERVICE::instance ()->start_svc ())
		{
			ACE_ERROR ((LM_ERROR, ACE_TEXT ("%p\n"), ACE_TEXT ("start")));
			result = -1;
		}
		break;
	case SC_STOP :
		if (-1 == Lorica::SERVICE::instance ()->stop_svc ())
		{
			ACE_ERROR ((LM_ERROR, ACE_TEXT ("%p\n"), ACE_TEXT ("stop")));
			result = -1;
		}
		break;
	default:
		result = -1;
	}
	return result;;
#else /* ACE_WIN32 */
	ACE_ERROR ((LM_ERROR,
		    ACE_TEXT("NT Service not supported on this platform")));
	return -1;
#endif /* ACE_WIN32 */
}

#if defined (ACE_WIN32)
// Define a function to handle Ctrl+C to cleanly shut this down.

static BOOL __stdcall
ConsoleHandler (DWORD ctrlType)
{
	ACE_UNUSED_ARG (ctrlType);
	Lorica::SERVICE::instance ()->handle_control (SERVICE_CONTROL_STOP);
	return TRUE;
}

ACE_NT_SERVICE_DEFINE (Lorica,
		       Lorica::NT_Service,
		       ACE_TEXT ("Lorica CORBA Firewall Proxy"));

#endif /* ACE_WIN32 */


Lorica::Proxy *
Lorica::Service_Loader::init_proxy ()
{
	Lorica::FileConfig *config = Lorica::FileConfig::instance ();
	try {
		config->init (config_file_, corba_debug_level_);
	}
	catch (const Lorica::FileConfig::InitError& )
	{
		ACE_ERROR((LM_ERROR,
			   ACE_TEXT("Proxy could not read %s.\n"),
			   this->config_file_.c_str()));
		return 0;
	}

	std::auto_ptr<Proxy> proxy(new Proxy());
	try {
		proxy->configure (*config);
		return proxy.release();
	}
	catch (const Lorica::Proxy::InitError& )
	{
		ACE_ERROR((LM_ERROR,
			   ACE_TEXT("Proxy initialization failed.\n")));
	}
	return 0;
}

int
Lorica::Service_Loader::run_service ()
{
#if defined (ACE_WIN32)
	ofstream *output_file = new ofstream(ACE_TEXT("lorica.log"), ios::out);
	if (output_file && output_file->rdstate() == ios::goodbit)
		ACE_LOG_MSG->msg_ostream(output_file, 1);
	ACE_LOG_MSG->open(program_name.c_str(),
			  ACE_Log_Msg::STDERR | ACE_Log_Msg::OSTREAM,
			  0);
	ACE_DEBUG ((LM_INFO, ACE_TEXT ("%T (%t): Starting Lorica server.\n")));
	Lorica::SERVICE::instance()->proxy (init_proxy());
	ACE_NT_SERVICE_RUN (Lorica,
			    Lorica::SERVICE::instance (),
			    ret);
	if (ret == 0)
		ACE_ERROR ((LM_ERROR,
			    ACE_TEXT ("%p\n"),
			    ACE_TEXT ("Couldn't start Lorica server")));

	return ret;
#else /* ACE_WIN32 */
	int ret  = ACE::daemonize(".",0,"lorica_proxy");
	if (ret == 0)
		ret = this->run_standalone();
	return ret == 0;
#endif /* ACE_WIN32 */
}

int
Lorica::Service_Loader::run_standalone ()
{
	std::auto_ptr<Proxy>proxy (this->init_proxy());

	ACE_DEBUG((LM_DEBUG,
		   ACE_TEXT("Lorica [%P] running as a standalone application \n")));

	proxy->activate ();
	proxy->wait ();

	return 0;
}

bool
Lorica::Service_Loader::is_service ()
{
	return !debug_;
}


int
ACE_TMAIN (int argc, ACE_TCHAR* argv[])
{
	Lorica::Service_Loader lorica;
	int result = 0;
	result = lorica.parse_args (argc, argv);
	if (result < 0)
		return 1;  // Error
	else if (result > 0)
		return 0;  // No error, but we should exit anyway.

	result = lorica.run_service_command ();
	if (result < 0)
		return 1;  // Error
	else if (result > 0)
		return 0;  // No error, but we should exit anyway.

	if (lorica.is_service ())
		result = lorica.run_service ();
	else
		result = lorica.run_standalone ();

	return result;

}
