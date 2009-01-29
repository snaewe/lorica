/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: t; c-basic-offset: 2 -*- */

/*
 *    Lorica header file.
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

/**
 * Definition of Lorica service. An instance of this class initializes the
 * ORB, gets the root POA, POA Manager Factory, IOR Table, and finally
 * runs the ORB.
 *
 */

#ifndef PROXY_H
#define PROXY_H

#include <string>
#include <ace/Task.h>
#include <tao/ORB.h>
#include <tao/PortableServer/POAC.h>
#include <tao/IORTable/IORTable.h>
#include <tao/PortableServer/POAManagerFactoryC.h>

namespace Lorica
{
	// Forward declarations.
	class Config;

/**
 *
 * The Proxy class contains all the base elements used to support the
 * application. It is based on ACE_Task_Base so that it may be easily
 * threaded.
 */

	class Proxy : public ACE_Task_Base {
	public:
		class InitError{};

		Proxy(const bool Debug);

		virtual ~Proxy(void);

		// override the local pid/log files if the commandline 
		// indicates use of alternate paths.
		void local_pid_file (const std::string &lpf);
		void local_ior_file (const std::string &lif);

		// load configuration based on supplied config file
		virtual void configure(Config & config)
			throw (InitError);

		// ACE Task base generic initializer - args[0] is config file name
		virtual int open(void *args = 0);

		// ACE Task base executable method.
		virtual int svc(void);

		// this only shuts down the proxy's svc loop.
		void shutdown(void);
		
		// this causes the ORB to destroy its resources and clean up.
    void destroy(void);

	private:
		// Will ensure that only one instance of Lorica 
		// is running at any one time by setting even if they
		// were to be isolated and run by different users
#ifndef ACE_WIN32
		bool get_lock(const char *lock_file_path);
#endif
		bool setup_shutdown_handler(void);

		static Proxy* this_;

		friend int signal_handler(int signum);

		CORBA::ORB_var orb_;
		PortableServer::POA_var root_poa_;
		PortableServer::POAManagerFactory_var pmf_;
		PortableServer::POAManager_var outside_pm_;
		PortableServer::POAManager_var inside_pm_;
		IORTable::Table_var iorTable_;
		PortableServer::POA_var admin_poa_;

		// the pid file actually
		int lock_fd;

		std::string pid_file_;
		std::string ior_file_;
		std::string local_pid_file_;
		std::string local_ior_file_;

		bool must_shutdown_;
		bool debug_;
	};

	int signal_handler(int signum);

}  /* namespace Lorica */

#endif // PROXY_H
