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
#include "ntsvc.h"
#include "Win32_svc_loader.h"
#include "lorica/ConfigBase.h"
#include "lorica/FileConfig.h"
#include "lorica/debug.h"
#include "lorica/MapperRegistry.h"

#if defined (ACE_WIN32)
Lorica::Win32_Service_Loader::Win32_Service_Loader(void)
	: service_command_(SC_NONE)
{
}

Lorica::Win32_Service_Loader::~Win32_Service_Loader(void)
{
}

void
Lorica::Win32_Service_Loader::print_usage_and_die(const ACE_TCHAR *prog)
{
	ACE_DEBUG((LM_INFO,
		   ACE_TEXT("Usage: %s")
		   ACE_TEXT(" -V -i -r -t -k -d -f -c -l\n")
		   ACE_TEXT(" -V: Print the version\n")
		   ACE_TEXT(" -i: Install this program as an NT service\n")
		   ACE_TEXT(" -r: Remove this program from the Service Manager\n")
		   ACE_TEXT(" -s: Start the service\n")
		   ACE_TEXT(" -k: Stop the service\n")
		   ACE_TEXT(" -d: Debug; run as a regular application\n")
		   ACE_TEXT(" -f: <file> Configuration file, default is \"lorica.conf\"\n")
		   ACE_TEXT(" -c: <level> Turn on CORBA debugging, default 0\n")
		   ACE_TEXT(" -l: <level> Turn on Lorica debugging, default 0\n"),
		   prog,
		   0));
	ACE_OS::exit(1);
}

int
Lorica::Win32_Service_Loader::parse_args(int argc,
				   ACE_TCHAR *argv[])
{
	if (!argc)
		return 0;

	ACE_Get_Opt get_opt(argc, argv, ACE_TEXT("Virtkdf:c:l:"));

	int c;
	const ACE_TCHAR *tmp;

	while ((c = get_opt()) != -1) {
		switch (c) {
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

int
Lorica::Win32_Service_Loader::run_service_command(void)
{
	// initialize ACE logging framework
	if (!this->is_service()) {
		// ==> if (lorica.debug_) a.k.a. put all logging
		// output in the log file
		this->reset_log();
	}

	Lorica::SERVICE::instance()->name(ACE_TEXT("Lorica"),
					  ACE_TEXT("Lorica IIOP Firewall"));

	if (service_command_ == SC_NONE)
		return 0;

	int result = 1;
	switch (service_command_) {
	case SC_INSTALL: {
		char pathname[_MAX_PATH] = { '\0' };
		DWORD length = ACE_TEXT_GetModuleFileName(NULL,
							  pathname,
							  _MAX_PATH);
		
		// Append the command used for running as a service
		ACE_OS::strcat(pathname, ACE_TEXT(" -s"));
		if (-1 == 
		    Lorica::SERVICE::instance()->insert(SERVICE_AUTO_START, 
							SERVICE_ERROR_NORMAL,
							pathname)) {
			ACE_ERROR ((LM_ERROR, 
				    ACE_TEXT("%N:%l - error inserting service\n")));
			result = -1;
		}
		
		break;
	}
	case SC_REMOVE :
		if (-1 == Lorica::SERVICE::instance()->remove()) {
			ACE_ERROR ((LM_ERROR, ACE_TEXT("%N:%l - remove\n")));
			result = -1;
		}
		break;
	case SC_START :
		if (-1 == Lorica::SERVICE::instance()->start_svc()) {
			ACE_ERROR ((LM_ERROR, ACE_TEXT("%N:%l - start\n")));
			result = -1;
		}
		break;
	case SC_STOP :
		if (-1 == Lorica::SERVICE::instance()->stop_svc()) {
			ACE_ERROR ((LM_ERROR, ACE_TEXT("%N:%l - stop\n")));
			result = -1;
		}
		break;
	default:
		result = -1;
	}

	return result;
}

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

Lorica::Proxy *
Lorica::Win32_Service_Loader::init_proxy(void)
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
Lorica::Win32_Service_Loader::run_service(void)
{
	try {
		Lorica::SERVICE::instance()->proxy(init_proxy());
	}
	catch (...) {
		ACE_ERROR((LM_ERROR,
			   ACE_TEXT("%N:%l - couldn't start Lorica server - init_proxy() failed\n")));
		return -1;
	}

	ACE_DEBUG((LM_INFO, ACE_TEXT("(%T) Starting Lorica service\n")));
	ACE_NT_SERVICE_RUN(Lorica,
			   Lorica::SERVICE::instance(),
			   ret);
	if (0 == ret) {
		ACE_ERROR((LM_ERROR, ACE_TEXT("%N:%l - couldn't start Lorica server - Service start failed\n")));
	}

	return ret;

}

int
Lorica::Win32_Service_Loader::run_standalone(void)
{
	std::auto_ptr<Proxy>proxy (this->init_proxy());
	Lorica::SERVICE::instance()->proxy(proxy.get());
	
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T) Lorica [%P] running as a standalone application \n")));

	SetConsoleCtrlHandler(&ConsoleHandler, true);
	proxy->activate();
	proxy->wait();
	Lorica::SERVICE::instance()->proxy(0);
	proxy->destroy();
	return 0;
}

void
Lorica::Win32_Service_Loader::set_working_directory(const ACE_TCHAR *progname)
{
	char *c = NULL;
	char cwd[_MAX_PATH] = { '\0' };

	if (!GetFullPathName(progname,
			     _MAX_PATH,
			     cwd,
			     NULL))
		exit(EXIT_FAILURE);

	c = &cwd[strlen(cwd)-1];
	while ('\\' != *c)
		*(c--) = '\0';
	*c = '\0';
	SetCurrentDirectory(cwd);
		
	// use this when Windows 2000 has become irrelevant
#if (_WIN32_WINNT > 0x0501)
	SetDllDirectory(cwd); 
#endif
}

int
Lorica::Win32_Service_Loader::execute (void)
{
	if (this->is_service())
		return this->run_service();
	else
		return this->run_standalone();
}

#endif // ACE_WIN32
