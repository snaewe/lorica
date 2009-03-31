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

#ifndef UNIX_SVC_LOADER_H
#define UNIX_SVC_LOADER_H

#include "svc_loader_base.h"

#if !defined (ACE_WIN32)
# define LORICA_SERVICE_LOADER Lorica::UNIX_Service_Loader

namespace Lorica
{
	class UNIX_Service_Loader : public Service_Loader_Base {
	public:

		UNIX_Service_Loader(void);

		~UNIX_Service_Loader(void);

		int parse_args(int argc,
			       ACE_TCHAR *argv[]);

		int run_service(void);

		int run_standalone(void);

		void print_usage_and_die(const ACE_TCHAR *prog);

		Proxy *init_proxy(void);
		
		virtual int execute (void);

	private:
		bool no_fork_;
	};
}

#endif // ACE_WIN32

#endif // UNIX_SVC_LOADER_H
