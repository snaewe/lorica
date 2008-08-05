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

#include "Hello.h"
#include <ace/Get_Opt.h>
#include <ace/OS_NS_stdio.h>
#include <tao/IORTable/IORTable.h>

int
main (int argc, char *argv[])
{
	try {
		CORBA::ORB_var orb = CORBA::ORB_init (argc, argv, "");

		CORBA::Object_var poa_object = orb->resolve_initial_references("RootPOA");

		PortableServer::POA_var root_poa = PortableServer::POA::_narrow (poa_object.in ());

		if (CORBA::is_nil (root_poa.in ()))
			ACE_ERROR_RETURN ((LM_ERROR,
					   " (%N|%l) Panic: nil RootPOA\n"),
					  1);

		PortableServer::POAManager_var poa_manager = root_poa->the_POAManager ();

		Hello *hello_impl;
		ACE_NEW_RETURN (hello_impl,
				Hello (orb.in ()),
				1);
		PortableServer::ServantBase_var owner_transfer(hello_impl);

		Test::Hello_var hello =	hello_impl->_this ();

		//
		// Register the server object in the IOR table
		//

		// Turn the object reference into an IOR string
		CORBA::String_var ior_string = orb->object_to_string(hello.in());
		ACE_DEBUG ((LM_DEBUG, "(%N|%l) server IOR: %s\n", ior_string.in()));

		// Get a reference to the IOR Table and bind the hello object
		CORBA::Object_var table_obj = orb->resolve_initial_references("IORTable");
		IORTable::Table_var ior_table = IORTable::Table::_narrow(table_obj.in());
		ior_table->bind(::Test::Hello::IOR_TABLE_KEY, ior_string.in());

		poa_manager->activate ();

		ACE_DEBUG ((LM_DEBUG, "(%N|%l) server - event loop beginning\n"));

		orb->run ();

		ACE_DEBUG ((LM_DEBUG, "(%N|%l) server - event loop finished\n"));

		root_poa->destroy (1, 1);

		orb->destroy ();
	}
	catch (const CORBA::Exception& ex) {
		ex._tao_print_exception ("Exception caught:");
		return 1;
	}

	return 0;
}
