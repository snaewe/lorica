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

#include "Hello.h"
#include <ace/Service_Config.h>
#include <ace/Get_Opt.h>
#include <ace/OS_NS_stdio.h>
#include <ace/OS_NS_stdlib.h>

#include "lorica/ReferenceMapperC.h"

ACE_RCSID (Hello,
	   server,
	   "$Id: server.cpp 1785 2007-05-04 16:40:44Z mesnierp $")

const char *mapped_file = "mapped.ior";
const char *orig_file = "direct.ior";
const char *lorica_ior = "corbaloc::localhost:10951/LORICA_REFERENCE_MAPPER";
bool no_register = false;

int
parse_args (int argc, char *argv[])
{
	ACE_Get_Opt get_opts (argc, argv, "o:m:n");
	int c;

	while ((c = get_opts ()) != -1)
		switch (c)
		{
		case 'o':
			orig_file = get_opts.opt_arg ();
			break;
		case 'm':
			mapped_file = get_opts.opt_arg();
			break;
		case 'n':
			no_register = true;
			break;

		case '?':
		default:
			ACE_ERROR_RETURN ((LM_ERROR,
					   "usage:  %s "
					   "-o <origfile> "
					   "-m <mappedfile> "
					   "\n",
					   argv [0]),
					  -1);
		}

	char * env = ACE_OS::getenv("LoricaIOR");
	if (env != 0)
		lorica_ior = env;
	ACE_DEBUG ((LM_DEBUG,"Using %s as Lorica IOR\n",lorica_ior));
	// Indicates sucessful parsing of the command line
	return 0;
}

Lorica::ReferenceMapper_var mapper;
CORBA::ORB_var orb;

Test::Hello_var mapped_hello;

int
main (int argc, char *argv[])
{
	try
	{
		orb = CORBA::ORB_init (argc, argv, "");

		CORBA::Object_var poa_object =
			orb->resolve_initial_references("RootPOA");

		PortableServer::POA_var root_poa =
			PortableServer::POA::_narrow (poa_object.in ());

		if (CORBA::is_nil (root_poa.in ()))
			ACE_ERROR_RETURN ((LM_ERROR,
					   " (%P|%t) Panic: nil RootPOA\n"),
					  1);

		PortableServer::POAManager_var poa_manager =
			root_poa->the_POAManager ();

		if (parse_args (argc, argv) != 0)
			return 1;

		CORBA::PolicyList policies(2);
    policies.length(2);
		policies[0] = 
			root_poa->create_lifespan_policy(PortableServer::PERSISTENT);
		policies[1] = 
			root_poa->create_id_assignment_policy(PortableServer::USER_ID);

		PortableServer::POA_var child_poa = 
			root_poa->create_POA ("persistent", poa_manager.in(), policies);

		poa_manager->activate();

		PortableServer::ServantBase_var servant (new Hello);

		PortableServer::ObjectId_var oid = 
			PortableServer::string_to_ObjectId ("gctest");
    child_poa->activate_object_with_id (oid.in(), servant.in() );
		CORBA::Object_var obj = child_poa->id_to_reference (oid.in());
		Test::Hello_var hello = Test::Hello::_narrow (obj.in());

		if (!no_register) 
			{
				obj = orb->string_to_object (lorica_ior);

				mapper = Lorica::ReferenceMapper::_narrow(obj.in());
				if (CORBA::is_nil(mapper.in()))
					ACE_ERROR_RETURN ((LM_ERROR,
														 "Cannot get reference to Lorica "
														 "reference mapper\n"),1);

				obj = mapper->as_server(hello.in(),"Hello",
																Lorica::ServerAgent::_nil());

				if (CORBA::is_nil (obj.in()))
					ACE_ERROR_RETURN ((LM_ERROR,
														 "Lorica reference mapper returned a nil "
														 "mapped reference.\n"),1);
				mapped_hello = Test::Hello::_narrow(obj.in());
				if (CORBA::is_nil(mapped_hello.in()))
					ACE_ERROR_RETURN ((LM_ERROR,
														 "Lorica reference mapper returned an "
														 "incorrectly typed reference\n"),1);

				CORBA::String_var orig_ior =
					orb->object_to_string (hello.in ());
				CORBA::String_var mapped_ior =
					orb->object_to_string (mapped_hello.in());

				if (ACE_OS::strcmp (orig_ior.in(), mapped_ior.in()) == 0)
					ACE_ERROR_RETURN ((LM_ERROR,
														 "Lorica reference mapper returned "
														 "the original reference unmapped.\n"),1);

				ACE_DEBUG ((LM_DEBUG,"writing original IOR to file %s\n",orig_file));
				ACE_DEBUG ((LM_DEBUG,"writing mapped IOR to file %s\n",mapped_file));
				ACE_DEBUG ((LM_DEBUG,"Size of orig IOR = %d, size of mapped = %d\n",
										ACE_OS::strlen(orig_ior.in()),
										ACE_OS::strlen(mapped_ior.in())));

				FILE *output_file= ACE_OS::fopen (mapped_file, "w");
				if (output_file == 0)
					ACE_ERROR_RETURN ((LM_ERROR,
														 "Cannot open output file for writing IOR: %s\n",
														 mapped_file),
														1);
				ACE_OS::fprintf (output_file, "%s", mapped_ior.in());
				ACE_OS::fclose (output_file);
		
				output_file= ACE_OS::fopen (orig_file, "w");
				if (output_file == 0)
					ACE_ERROR_RETURN ((LM_ERROR,
														 "Cannot open output file for writing IOR: %s\n",
														 orig_file),
														1);
				ACE_OS::fprintf (output_file, "%s", orig_ior.in());
				ACE_OS::fclose (output_file);
			}

		ACE_Time_Value delay(8,0); // run for 8 seconds, which gets past 1 gc 
                               // iterateion and gives client time for 2
		                           // string gets.
		orb->run(delay);

		// No need to run the ORB the test only requires modifying an IOR
		orb->destroy ();
	}
	catch (const CORBA::Exception& ex)
	{
		ex._tao_print_exception ("Exception caught:");
		return 1;
	}

	return 0;
}
