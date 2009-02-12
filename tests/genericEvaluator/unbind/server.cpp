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

#include "Servants.h"
#include <ace/Get_Opt.h>
#include "lorica/ReferenceMapperC.h"

const char *lorica_ior  = "corbaloc::localhost:1950/LORICA_REFERENCE_MAPPER";

int main(int argc, char *argv[]) {
	try {
		CORBA::ORB_var orb = CORBA::ORB_init(argc, argv, "");
		CORBA::Object_var poa_object = orb->resolve_initial_references("RootPOA");
		if (CORBA::is_nil(poa_object.in ())) {
			ACE_ERROR_RETURN((LM_ERROR, "Server::Unable to initialize the POA.\n"), 1);
		}

		PortableServer::POA_var root_poa = PortableServer::POA::_narrow(poa_object.in ());
		PortableServer::POAManager_var poa_manager = root_poa->the_POAManager();

		// getting lorica reference mapper
		CORBA::Object_var obj = orb->string_to_object(lorica_ior);
		Lorica::ReferenceMapper_var mapper = Lorica::ReferenceMapper::_narrow(obj.in());
		if (CORBA::is_nil(mapper.in())) {
			ACE_ERROR_RETURN((LM_ERROR, "Server::Cannot get reference to Lorica reference mapper\n"), 1);
		}

		// getting the first original object
		First *first_impl;
		ACE_NEW_RETURN(first_impl, First(), 1);
		PortableServer::ServantBase_var first_owner_transfer(first_impl);
		Test::First_var first = first_impl->_this();

		// getting the second original object
		Second *second_impl;
		ACE_NEW_RETURN(second_impl, Second(), 1);
		PortableServer::ServantBase_var second_owner_transfer(second_impl);
		Test::Second_var second = second_impl->_this();

		// getting the third original object
		Third *third_impl;
		ACE_NEW_RETURN(third_impl, Third(), 1);
		PortableServer::ServantBase_var third_owner_transfer(third_impl);
		Test::Third_var third = third_impl->_this();

		// mapping objects
		ACE_DEBUG ((LM_DEBUG, "Server::Binding first object\n"));
		CORBA::Object_var first_obj = mapper->as_server(first.in(),"First", Lorica::ServerAgent::_nil());
		
		ACE_DEBUG ((LM_DEBUG, "Server::Binding second object\n"));
		CORBA::Object_var second_obj = mapper->as_server(second.in(),"Second", Lorica::ServerAgent::_nil());
		
		ACE_DEBUG ((LM_DEBUG, "Server::Binding third object\n"));
		CORBA::Object_var third_obj = mapper->as_server(third.in(),"Third", Lorica::ServerAgent::_nil());

		// unbinding the mapped objects
		ACE_DEBUG ((LM_DEBUG, "Server::UnBinding first object\n"));
		mapper->remove_server(first_obj.in());
		ACE_DEBUG ((LM_DEBUG, "Server::UnBinding second object\n"));
		mapper->remove_server(second_obj.in());
		ACE_DEBUG ((LM_DEBUG, "Server::UnBinding third object\n"));
		mapper->remove_server(third_obj.in());

		return 0;
	} catch (const CORBA::Exception& ex) {
		ex._tao_print_exception ("Server::Caught Exception => ");
		return 1;
	}
}
