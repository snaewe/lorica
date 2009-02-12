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

#include "ServerAgent_i.h"

ServerAgent_i::ServerAgent_i ()
{
}

ServerAgent_i::~ServerAgent_i ()
{
}

void
ServerAgent_i::error_occured (CORBA::Long code,
			      const char *message)
{
	ACE_DEBUG ((LM_INFO, "ERROR occurred in the proxy.\n"
		    "error code: %d \n"
		    "error message :%s\n",
		    code, message));

}

void
ServerAgent_i::proxy_disconnect ()
{
	ACE_DEBUG((LM_INFO, "The proxy is shutting down\n"));
}
