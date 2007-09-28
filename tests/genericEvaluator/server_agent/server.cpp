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

#include "test_i.h"
#include "ServerAgent_i.h"
#include "ace/Get_Opt.h"
#include "ace/Task.h"

#include "lorica/ReferenceMapperC.h"
const char *ior_output_file = "server.ior";

const char *mapped_file = "mapped.ior";
const char *orig_file = "direct.ior";
const char *lorica_ior = "corbaloc::localhost:10951/lorica_reference_mapper";

int nthreads = 4;
int timeout = 0;

int
parse_args (int argc, char *argv[])
{
	ACE_Get_Opt get_opts (argc, argv, "o:m:n:t");
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
			nthreads = ACE_OS::atoi (get_opts.optarg);
			break;
		case 't':
			timeout = 1;
			break;
		case '?':
		default:
			ACE_ERROR_RETURN ((LM_ERROR,
					   "usage:  %s "
					   "-o <iorfile>"
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
		{
			ACE_ERROR_RETURN ((LM_ERROR,
					   " (%P|%t) Unable to initialize the POA.\n"),
					  1);
		}

		PortableServer::POA_var root_poa =
			PortableServer::POA::_narrow (poa_object.in ());

		PortableServer::POAManager_var poa_manager =
			root_poa->the_POAManager ();

		if (parse_args (argc, argv) != 0)
		{
			return 1;
		}

		// getting the mapper object
		CORBA::Object_var obj =
			orb->string_to_object (lorica_ior);
		Lorica::ReferenceMapper_var mapper =
			Lorica::ReferenceMapper::_narrow(obj.in());
		if (CORBA::is_nil(mapper.in()))
			ACE_ERROR_RETURN ((LM_ERROR,
					   "Cannot get reference to Lorica "
					   "reference mapper\n"),1);

		// getting the ServerAgent callback
		ServerAgent_i server_agent_impl (mapper);
		Lorica::ServerAgent_var  server_agent =
			server_agent_impl._this ();

		// getting the original oject
		Simple_Server_i server_impl (orb.in (), &server_agent_impl,  timeout);
		Simple_Server_var server =
			server_impl._this ();


		// getting the mapped object
		obj = mapper->as_server(server.in(),"SimpleServer",
					server_agent.in ());
		if (CORBA::is_nil (obj.in()))
			ACE_ERROR_RETURN ((LM_ERROR,
					   "Lorica reference mapper returned a nil "
					   "mapped reference.\n"),1);

		// narrowing the mapped object
		Simple_Server_var h2 = Simple_Server::_narrow(obj.in());
		if (CORBA::is_nil(h2.in()))
			ACE_ERROR_RETURN ((LM_ERROR,
					   "Lorica reference mapper returned an "
					   "incorrectly typed reference\n"),1);

		CORBA::String_var orig_ior =
			orb->object_to_string (server.in ());
		CORBA::String_var mapped_ior =
			orb->object_to_string (h2.in());

		if (ACE_OS::strcmp (orig_ior.in(), mapped_ior.in()) == 0)
			ACE_ERROR_RETURN ((LM_ERROR,
					   "Lorica reference mapper returned "
					   "the original reference unmapped.\n"),1);

		server_agent_impl.add_managed_ref (&server_impl, obj.in());

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

		poa_manager->activate ();

		orb->run ();

		ACE_DEBUG ((LM_DEBUG, "event loop finished\n"));
	}
	catch (const CORBA::Exception& ex)
	{
		ex._tao_print_exception ("Server: exception caught - ");
		return 1;
	}

	return 0;
}
