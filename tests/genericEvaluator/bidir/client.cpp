/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: t; c-basic-offset: 2 -*- */

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

#include "Receiver_i.h"
#include "Client_Task.h"
#include "Server_Task.h"
#include <ace/Get_Opt.h>
#include <ace/OS_NS_stdio.h>
#include <tao/BiDir_GIOP/BiDirGIOP.h>
#include <tao/AnyTypeCode/Any.h>

const char *ior = "file://mapped.ior";

int
parse_args (int argc, char *argv[])
{
	ACE_Get_Opt get_opts (argc, argv, "k:");
	int c;

	while ((c = get_opts ()) != -1)
		switch (c)
		{
		case 'k':
			ior = get_opts.optarg;
			break;
		case '?':
		default:
			ACE_ERROR_RETURN ((LM_ERROR,
					   "usage:  %s "
					   "-k <ior>"
					   "\n",
					   argv [0]),
					  -1);
		}

	// Indicates sucessful parsing of the command line
	return 0;
}

int
main (int argc, char *argv[])
{
	try
	{
		CORBA::ORB_var orb =
			CORBA::ORB_init (argc, argv, "");

		CORBA::Object_var poa_object =
			orb->resolve_initial_references("RootPOA");

		if (CORBA::is_nil (poa_object.in ()))
			ACE_ERROR_RETURN ((LM_ERROR,
					   " (%P|%t) Unable to initialize the POA.\n"),
					  1);

		PortableServer::POA_var root_poa =
			PortableServer::POA::_narrow (poa_object.in ());

		PortableServer::POAManager_var poa_manager =
			root_poa->the_POAManager ();

		if (parse_args (argc, argv) != 0)
			return 1;

		// Policies for the childPOA to be created.
		CORBA::PolicyList policies (1);
		policies.length (1);
		
		CORBA::Any pol;
		pol <<= BiDirPolicy::BOTH;
		policies[0] =
			orb->create_policy (BiDirPolicy::BIDIRECTIONAL_POLICY_TYPE,
													pol);

		// Create POA as child of RootPOA with the above policies.  This POA
		// will receive request in the same connection in which it sent
		// the request
		PortableServer::POA_var child_poa =
			root_poa->create_POA ("childPOA",
														poa_manager.in (),
														policies);


		// Get the sender reference..
		CORBA::Object_var tmp =
			orb->string_to_object(ior);

		Test::Sender_var sender =
			Test::Sender::_narrow(tmp.in ());

		if (CORBA::is_nil (sender.in ()))
		{
			ACE_ERROR_RETURN ((LM_DEBUG,
					   "Nil coordinator reference <%s>\n",
					   ior),
					  1);
		}

		Receiver_i *receiver_impl;
		ACE_NEW_RETURN (receiver_impl,
				Receiver_i (orb.in (),
					    sender.in (),
					    10),
				1);

		PortableServer::ServantBase_var servant(receiver_impl);

		PortableServer::ObjectId_var oid = 
			child_poa->activate_object(servant.in());
		tmp = child_poa->id_to_reference (oid.in());
		Test::Receiver_var receiver = Test::Receiver::_narrow(tmp.in());
		CORBA::String_var ior = orb->object_to_string (tmp.in());
		ACE_DEBUG ((LM_DEBUG, "callback reference: %s\n", ior.in()));
		// Activate poa manager
		poa_manager->activate ();

		Client_Task client_task (sender.in (),
					 receiver.in (),
					 ACE_Thread_Manager::instance ());

		Server_Task server_task (orb.in (),
					 ACE_Thread_Manager::instance ());

		// Before creating threads we will let the sender know that we
		// will have two threads that would make invocations..
		sender->active_objects ((CORBA::Short) 2);

		if (server_task.activate (THR_NEW_LWP | THR_JOINABLE, 2,1) == -1)
		{
			ACE_ERROR ((LM_ERROR, "Error activating server task\n"));
		}

		if (client_task.activate (THR_NEW_LWP | THR_JOINABLE, 2, 1) == -1)
		{
			ACE_ERROR ((LM_ERROR, "Error activating client task\n"));
		}
		ACE_OS::sleep (1);
		ACE_DEBUG ((LM_DEBUG," Waiting for tasks to finish\n"));
		ACE_Thread_Manager::instance()->wait ();

		ACE_DEBUG ((LM_DEBUG, "Event Loop finished \n"));

		orb->destroy ();
	}
	catch (const CORBA::Exception& ex)
	{
		ex._tao_print_exception ("Exception caught:");
		return 1;
	}

	return 0;
};
