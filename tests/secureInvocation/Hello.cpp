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

#include "Hello.h"

ACE_RCSID(Hello, Hello, "$Id: Hello.cpp 1871 2007-05-14 19:34:59Z mesnierp $")

Hello::Hello (void  (*sh)())
: do_shutdown_(sh)
{
	int argc = 0;
	char **argv = 0;
	CORBA::ORB_var orb = CORBA::ORB_init (argc,argv);
	try
	{
		CORBA::Object_var obj =
			orb->resolve_initial_references ("SecurityLevel3:SecurityCurrent");
		if (!CORBA::is_nil(obj.in()))
			sec_current_ = SecurityLevel3::SecurityCurrent::_narrow (obj.in ());
		ACE_DEBUG ((LM_DEBUG,"Got security current\n"));
	}
	catch (CORBA::Exception &ex)
	{
		ACE_DEBUG ((LM_DEBUG, "Getting security current raised: %s\n",
			    ex._name()));
	}
}

char *
Hello::get_string (void)
	ACE_THROW_SPEC ((CORBA::SystemException))
{
	ACE_DEBUG ((LM_DEBUG,"Get_String invoked\n"));
	if (CORBA::is_nil (sec_current_.in()))
		ACE_DEBUG ((LM_DEBUG,"No security current in get_string\n"));
	else
	{
		try
		{
			SecurityLevel3::ClientCredentials_var cc =
				sec_current_->client_credentials();
			if (CORBA::is_nil(cc.in()))
				ACE_DEBUG ((LM_DEBUG,"No credentials available\n"));
			else
				ACE_DEBUG ((LM_DEBUG,"We have credentials!\n"));
		}
		catch (CORBA::Exception &ex)
		{
			ACE_DEBUG ((LM_DEBUG,"Caught %s\n", ex._name()));
		}
	}
	return CORBA::string_dup ("Hello there!");
}

void
Hello::shutdown (void)
	ACE_THROW_SPEC ((CORBA::SystemException))
{
	this->do_shutdown_ ();
}
