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

#ifndef SVC_LOADER_BASE_H
#define SVC_LOADER_BASE_H

#include "defines/build-system-defs.h"
#include <ace/ace_wchar.h>
#include "proxy.h"

namespace Lorica
{
	class Service_Loader_Base {
	public:
		Service_Loader_Base(void);

		virtual ~Service_Loader_Base(void);

		virtual int parse_args(int argc,
			       ACE_TCHAR *argv[]) = 0;

		virtual int run_service_command(void);

		virtual int run_service(void) = 0;

		virtual int run_standalone(void) = 0;

		void print_usage_and_die(const ACE_TCHAR *prog);

		bool is_service(void);

		virtual Proxy *init_proxy(void) = 0;

		virtual void set_working_directory (const ACE_TCHAR *prog);

		void reset_log (void);

		virtual int execute (void) = 0;

	protected:
		std::string config_file_;
		int corba_debug_level_;
		bool debug_;
	};
}

#endif // SVC_LOADER_BASE_H
