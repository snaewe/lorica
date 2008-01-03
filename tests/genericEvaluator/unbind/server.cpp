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

#include "Servants.h"
#include "ace/Get_Opt.h"
#include "lorica/ReferenceMapperC.h"

const char *lorica_ior  = "corbaloc::localhost:1950/lorica_reference_mapper";

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

		// mapping the first object
		CORBA::Object_var first_obj = mapper->as_server(first.in(),"First", Lorica::ServerAgent::_nil());
		if (CORBA::is_nil (first_obj.in())) {
			ACE_ERROR_RETURN((LM_ERROR, "Server::Lorica reference mapper returned a nil mapped reference for the first object.\n"), 1);
		}

		// getting the second original object
		Second *second_impl;
		ACE_NEW_RETURN(second_impl, Second(), 1);
		PortableServer::ServantBase_var second_owner_transfer(second_impl);
		Test::Second_var second = second_impl->_this();

		// mapping the second object
		CORBA::Object_var second_obj = mapper->as_server(second.in(),"Second", Lorica::ServerAgent::_nil());
		if (CORBA::is_nil (second_obj.in())) {
			ACE_ERROR_RETURN((LM_ERROR, "Server::Lorica reference mapper returned a nil mapped reference for the second object.\n"), 1);
		}

		// removing object mappings from lorica
		mapper->remove_server(first_obj.in());
		mapper->remove_server(second_obj.in());

		return 0;
	} catch (const CORBA::Exception& ex) {
		ex._tao_print_exception ("Server::Caught Exception => ");
		return 1;
	}
}