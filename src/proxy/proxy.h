/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 *    Lorica header file.
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

/**
 * Definition of Lorica service. An instance of this class initializes the
 * ORB, gets the root POA, POA Manager Factory, IOR Table, and finally
 * runs the ORB.
 *
 */

#ifndef PROXY_H
#define PROXY_H

#include "ace/Task.h"

#include "tao/ORB.h"
#include "tao/PortableServer/POAC.h"
#include "tao/IORTable/IORTable.h"
#include "tao/PortableServer/POAManagerFactoryC.h"
#include <string>

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

	class Proxy : public ACE_Task_Base
	{
	public:
		class InitError {};

	public:
		Proxy ();
		virtual ~Proxy (void);

		// load configuration based on supplied config file
		virtual void configure (Config& config) throw (InitError);

		// ACE Task base generic initializer - args[0] is config file name
		virtual int open (void *args = 0);

		// ACE Task base executable method.
		virtual int svc (void);

		void shutdown (void);

	private:
		bool setup_shutdown_handler (void);

		static Proxy* this_;

		friend int signal_handler (int signum);

	private:
		CORBA::ORB_var orb_;
		PortableServer::POA_var root_poa_;
		PortableServer::POAManagerFactory_var pmf_;
		PortableServer::POAManager_var outside_pm_;
		PortableServer::POAManager_var inside_pm_;
		IORTable::Table_var iorTable_;
		PortableServer::POA_var admin_poa_;

		std::string pidfile_;
		bool must_shutdown_;
	};

	int signal_handler (int signum);

}  /* namespace Lorica */

#endif // PROXY_H
