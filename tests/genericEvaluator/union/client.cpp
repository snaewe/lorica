/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: f; c-basic-offset: 2 -*- */

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

#include "testC.h"
#include <tao/debug.h>
#include <ace/Get_Opt.h>
#include <ace/Task.h>

#include "CommentRestriction_impl.h"
#include "RestrictionContainer_impl.h"
#include "Client_Orb_Task.h"

const char *ior = "file://mapped.ior";

int do_shutdown = 0;

int parse_args(int argc, char *argv[]) {
	ACE_Get_Opt get_opts (argc, argv, "xk:?");

	int c;
	while((c = get_opts ()) != -1) {
		switch (c) {
		case 'x':
			do_shutdown = 1;
			break;
		case 'k':
			ior = get_opts.optarg;
			break;
		case '?':
		default:
			ACE_ERROR_RETURN ((LM_ERROR,
					   "usage:  %s "
					   "-x "
					   "-k <ior> "
					   "?"
					   "\n",
					   argv [0]),
					  -1);
		}
	}

	// Indicates sucessful parsing of the command line
	return 0;
}

void do_primary_test(CORBA::ORB_var &orb, PortableServer::POA_var &root_poa,UnionTest::TestServer_var &server) {
	try {
		// Access the Server
		ACE_DEBUG((LM_DEBUG, "Client::Using IOR source %s\n", ior));
		CORBA::Object_var object = orb->string_to_object(ior);

		server = UnionTest::TestServer::_narrow(object.in());
		if (CORBA::is_nil(server.in())) {
			ACE_ERROR((LM_ERROR, "Client::Object reference <%s> is nil\n", ior));
			return;
		}

		ACE_DEBUG((LM_DEBUG,"Client::Executing server method with a union parameter\n"));
		// Test code start
		
		// create comment restriction and activate in POA
		CORBA::String_var comment_message= "union";
		CommentRestriction_impl * comment_restriction_impl = new CommentRestriction_impl(comment_message);
		PortableServer::ObjectId_var comment_restriction_oid = root_poa->activate_object(comment_restriction_impl);
		CORBA::Object_var comment_restriction_obj = root_poa->id_to_reference(comment_restriction_oid);
		UnionTest::CommentRestriction_var comment_restriction = UnionTest::CommentRestriction::_narrow(comment_restriction_obj);

		// create restriction union
		UnionTest::Restriction_var restriction = new UnionTest::Restriction;
		restriction->comment(comment_restriction.in());

		// create restriction container.
		RestrictionContainer_impl * restrict_container_impl = new RestrictionContainer_impl(restriction);
		PortableServer::ObjectId_var restrict_container_oid = root_poa->activate_object(restrict_container_impl);
		CORBA::Object_var restrict_container_obj = root_poa->id_to_reference(restrict_container_oid);
		UnionTest::RestrictionContainer_var restrict_container = UnionTest::RestrictionContainer::_narrow(restrict_container_obj);

		// Start orb in new thread
		Client_Orb_Task client_orb_task(orb.in());
		if(client_orb_task.activate() == -1) {
			ACE_ERROR((LM_ERROR, "Client::Error activating orb task\n"));
		}

		// call server
		server->Restrict(restrict_container.in());

		// Stop the orb
		orb->shutdown(true);
		client_orb_task.wait();

		// Test code end		
		ACE_DEBUG((LM_DEBUG, "Client::Server was able to access union without throwing exceptions\n"));
	} catch(CORBA::Exception &ex) {
		ACE_ERROR((LM_ERROR, "Client::Caught exception => %s\n", ex._name()));
	}
}

int shutdown(UnionTest::TestServer_var &server) {
	ACE_DEBUG((LM_DEBUG, "Client::Invoking shutdown on the server\n "));
	try {
		server->shutdown();
		return 0;
	} catch(const CORBA::Exception& ex) {
		ex._tao_print_exception ("Client::Caught exception => ");
		return 1;
	}
}


int main(int argc, char *argv[]) {
	int result = 0;
	UnionTest::TestServer_var server;

	CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);
	CORBA::Object_var poa_object = orb->resolve_initial_references("RootPOA");
	if(CORBA::is_nil(poa_object.in ())){
		ACE_ERROR_RETURN((LM_ERROR, "Client::Unable to initialize the POA.\n"), 1);
	}
	PortableServer::POA_var root_poa = PortableServer::POA::_narrow(poa_object.in ());
	PortableServer::POAManager_var poa_manager = root_poa->the_POAManager();
	poa_manager->activate();

	// Check input parameters
	if(parse_args(argc, argv) != 0) {
		return 1;
	}

	// Do the test
	do_primary_test(orb, root_poa, server);

	if(do_shutdown) {
		result = shutdown(server);
	}
	
	ACE_DEBUG ((LM_DEBUG,"Client::Destrying ORB.\n"));
	orb->destroy();
	
	return result;
};
