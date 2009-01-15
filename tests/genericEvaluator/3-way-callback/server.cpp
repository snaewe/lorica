/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 *    Lorica source file.
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

#include "Server_i.h"
#include <ace/Get_Opt.h>
#include "lorica/ReferenceMapperC.h"

const char *lorica_ior = "corbaloc::localhost:10951/LORICA_REFERENCE_MAPPER";

int
main (int argc, char *argv[])
{
	try {
		CORBA::ORB_var orb = CORBA::ORB_init (argc, argv, "");

		CORBA::Object_var poa_object = orb->resolve_initial_references("RootPOA");

		if (CORBA::is_nil (poa_object.in ()))
			ACE_ERROR_RETURN((LM_ERROR, " (%P|%t) Unable to initialize the POA.\n"), 1);

		PortableServer::POA_var root_poa = PortableServer::POA::_narrow (poa_object.in ());

		PortableServer::POAManager_var poa_manager = root_poa->the_POAManager ();

		Test_Server_i *server_impl;
		ACE_NEW_RETURN(server_impl, Test_Server_i(), 1);
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

		orb->run ();
		ACE_DEBUG ((LM_DEBUG, "event loop finished\n"));

		root_poa->destroy(1, 1);
		orb->destroy();
	}
	catch (const CORBA::Exception& ex) {
		ex._tao_print_exception("Exception caught:");
		return 1;
	}

	return 0;
}
