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

#include "Server_i.h"
#include <ace/Get_Opt.h>
#include "lorica/ReferenceMapperC.h"

const char *lorica_ior = "corbaloc::localhost:10951/LORICA_REFERENCE_MAPPER";

#define d(__str__) printf("%d - %s\n", __LINE__, __str__)

int
main (int argc, char *argv[])
{
	int success_count = 0;
	int failure_count = 0;

	try {
		CORBA::ORB_var orb = CORBA::ORB_init (argc, argv, "");
		CORBA::Object_var poa_object = orb->resolve_initial_references("RootPOA");

		if (CORBA::is_nil (poa_object.in ()))
			ACE_ERROR_RETURN((LM_ERROR, " (%P|%t) Unable to initialize the POA.\n"), 1);

		PortableServer::POA_var root_poa = PortableServer::POA::_narrow (poa_object.in ());
		PortableServer::POAManager_var poa_manager = root_poa->the_POAManager ();

		Test_Server_i *server_impl;
		ACE_NEW_RETURN(server_impl, Test_Server_i(orb.in(), &success_count, &failure_count), 1);
		PortableServer::ServantBase_var receiver_owner_transfer(server_impl);

		// getting the original object
		Test::Server_var server = server_impl->_this();

		// getting the mapper object
		CORBA::Object_var obj = orb->string_to_object(lorica_ior);
		Lorica::ReferenceMapper_var mapper = Lorica::ReferenceMapper::_narrow(obj.in());
		if (CORBA::is_nil(mapper.in()))
			ACE_ERROR_RETURN((LM_ERROR, "Cannot get reference to Lorica reference mapper\n"), 1);

		// getting the mapped object
		obj = mapper->as_server(server.in(), "Server", Lorica::ServerAgent::_nil());
		if (CORBA::is_nil (obj.in()))
			ACE_ERROR_RETURN ((LM_ERROR, "Lorica reference mapper returned a nil mapped reference.\n"),1);

		// narrowing the mapped object
		Test::Server_var mapped_server = Test::Server::_narrow(obj.in());
		if (CORBA::is_nil(mapped_server.in()))
			ACE_ERROR_RETURN ((LM_ERROR, "Lorica reference mapper returned an incorrectly typed reference\n"), 1);

		poa_manager->activate();

		ACE_DEBUG ((LM_DEBUG, "The server ORB run loop is now in progress - Please start two remote clients to test (and a third client to shut it down) \n"));
		orb->run ();
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("event loop finished - Error count = %d\n"), failure_count));

		root_poa->destroy(1, 1);
		orb->destroy();
	}
	catch (const CORBA::Exception& ex) {
		ex._tao_print_exception("Exception caught:");
		return 1;
	}

	return failure_count;
}
