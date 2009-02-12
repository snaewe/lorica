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
#include <ace/Service_Config.h>
#include <ace/Get_Opt.h>
#include <ace/OS_NS_stdio.h>
#include <ace/OS_NS_stdlib.h>
#include <orbsvcs/SecurityC.h>

#include "lorica/ReferenceMapperC.h"

ACE_RCSID (Hello,
	   server,
	   "$Id: server.cpp 1877 2007-05-17 19:25:33Z mesnierp $")

const char *mapped_file = "mapped.ior";
const char *orig_file = "direct.ior";
const char *lorica_ior = //"file://lorica.ior";
	"corbaloc::localhost:10951/LORICA_REFERENCE_MAPPER";

bool linger = false;

int
parse_args (int argc, char *argv[])
{
	ACE_Get_Opt get_opts (argc, argv, "o:m:l");
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
		case 'l':
			linger = true;
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

void
shutdown_handler (void)
{
	if (!CORBA::is_nil(mapper.in()))
		mapper->remove_server(mapped_hello.in());
	orb->shutdown();
}

CORBA::Object_ptr register_with_proxy (CORBA::Object_ptr native)
{
	// Disable protection for this insecure invocation test.

	Security::QOP qop = Security::SecQOPNoProtection;

	CORBA::Any no_protection;
	no_protection <<= qop;

	// Create the Security::QOPPolicy.
	CORBA::Policy_var policy =
		orb->create_policy (Security::SecQOPPolicy,
				    no_protection);

	CORBA::PolicyList policy_list (1);
	policy_list.length (1);
	policy_list[0] = CORBA::Policy::_duplicate (policy.in ());

	// Create an object reference that uses plain IIOP (i.e. no
	// protection).
	CORBA::Object_var object =
		mapper->_set_policy_overrides (policy_list,
					       CORBA::SET_OVERRIDE);

	ACE_DEBUG ((LM_DEBUG,"Trying to narrow an insecure reference\n"));

	Lorica::ReferenceMapper_var open_mapper =
		Lorica::ReferenceMapper::_narrow(object.in());

	ACE_DEBUG ((LM_DEBUG,"Using open mapper for registering\n"));

	return open_mapper->as_server(native,"Hello",
				      Lorica::ServerAgent::_nil());
}

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

		poa_manager->activate();

		Hello *hello_impl;
		ACE_NEW_RETURN (hello_impl,
				Hello (shutdown_handler),
				1);
		PortableServer::ServantBase_var owner_transfer(hello_impl);

		Test::Hello_var hello =
			hello_impl->_this ();

		ACE_DEBUG ((LM_DEBUG, "getting proxy reference\n"));

		CORBA::Object_var obj =
			orb->string_to_object (lorica_ior);

		ACE_DEBUG ((LM_DEBUG, "narrowing proxy reference\n"));

		mapper = Lorica::ReferenceMapper::_narrow(obj.in());
		if (CORBA::is_nil(mapper.in()))
			ACE_ERROR_RETURN ((LM_ERROR,
					   "Cannot get reference to Lorica "
					   "reference mapper\n"),1);

		obj = mapper->as_server (hello.in(),"Hello",
					 Lorica::ServerAgent::_nil());

		ACE_DEBUG ((LM_DEBUG,"register_with_proxy() returned\n"));

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

		if (linger)
			orb->run();
		else
			shutdown_handler();

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
