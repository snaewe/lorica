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

//**************************************************************
//
// BASED ON ACE SAMPLE CODE BY:
//    ACE_wrappers/examples/NT_Service and
//    ACE_wrappers/TAO/orbsvcs/ImplRepo_Service
//
//**************************************************************

#include <ace/Log_Msg.h>
#include "Win32_svc_loader.h"
#include "UNIX_svc_loader.h"

int
ACE_TMAIN (int argc, ACE_TCHAR *argv[])
{
	LORICA_SERVICE_LOADER lorica;
	int result = 0;
	lorica.set_working_directory(argv[0]);

	ACE_LOG_MSG->open(argv[0], ACE_Log_Msg::SYSLOG);
	result = lorica.parse_args(argc, argv);
	if (result < 0)
		exit(EXIT_FAILURE);  // Error
	else if (result > 0)
		exit(EXIT_SUCCESS);  // No error, but we should exit anyway.


	ACE_DEBUG((LM_INFO, 
		   ACE_TEXT("(%T) Lorica %s initializing\n"), VERSION));

	result = lorica.run_service_command();
	if (result < 0)
		exit(EXIT_FAILURE);  // Error
	else if (result > 0)
		exit(EXIT_SUCCESS);  // No error, but we should exit anyway.

	result = lorica.execute ();

	ACE_DEBUG((LM_INFO, ACE_TEXT("(%T) Lorica is shutting down\n")));
	return result;
}
