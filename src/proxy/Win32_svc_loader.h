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

#ifndef WIN32_SVC_LOADER_H
#define WIN32_SVC_LOADER_H

#include "svc_loader_base.h"

#if defined (ACE_WIN32)
# define LORICA_SERVICE_LOADER Lorica::Win32_Service_Loader

namespace Lorica
{
	class Win32_Service_Loader : public Service_Loader_Base {
	public:
		enum SERVICE_COMMAND {
			SC_NONE,
			SC_INSTALL,
			SC_REMOVE,
			SC_START,
			SC_STOP
		};

		Win32_Service_Loader(void);

		virtual ~Win32_Service_Loader(void);

		int parse_args(int argc,
			       ACE_TCHAR *argv[]);

		int run_service_command(void);

		int run_service(void);

		int run_standalone(void);

		void print_usage_and_die(const ACE_TCHAR *prog);

		void set_working_directory(const ACE_TCHAR *prog);

		int execute (void);

		Proxy *init_proxy(void);

	private:
		/// SC_NONE, SC_INSTALL, SC_REMOVE, ...
		SERVICE_COMMAND service_command_;
	};
}

#endif // ACE_WIN32

#endif // WIN32_SVC_LOADER_H
