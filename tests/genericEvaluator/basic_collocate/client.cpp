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

#include "testC.h"
#include <tao/debug.h>
#include <ace/Get_Opt.h>
#include <ace/Task.h>

const char *ior = "file://mapped.ior";
const char *direct = "file://direct.ior";

int niterations = 1;
int do_shutdown = 0;
int test_user_exception = 0;
int test_system_exception = 0;
int timeout = 0;

int
parse_args (int argc, char *argv[])
{
	ACE_Get_Opt get_opts (argc, argv, "xusk:i:t");
	int c;

	while ((c = get_opts ()) != -1)
		switch (c)
		{
		case 'x':
			do_shutdown = 1;
			break;

		case 'u':
			test_user_exception = 1;
			break;

		case 's':
			test_system_exception = 1;
			break;

		case 'k':
			ior = get_opts.optarg;
			break;

		case 'i':
			niterations = ACE_OS::atoi (get_opts.optarg);
			break;
		case 't':
			timeout = 1;
			break;

		case '?':
		default:
			ACE_ERROR_RETURN ((LM_ERROR,
					   "usage:  %s "
					   "-x "
					   "-u "
					   "-s "
					   "-k <ior> "
					   "-i <niterations> "
					   "\n",
					   argv [0]),
					  -1);
		}

	// Indicates sucessful parsing of the command line
	return 0;
}

void
do_primary_test (CORBA::ORB_var &orb, Simple_Server_var &server)
{
	try
	{
		CORBA::Object_var nilobj = CORBA::Object::_nil();

		CORBA::Object_var object =
			orb->string_to_object (ior);

		server =
			Simple_Server::_narrow (object.in ());

		if (CORBA::is_nil (server.in ()))
		{
			ACE_ERROR ((LM_ERROR,
				    "Object reference <%s> is nil\n",
				    ior));
			return;
		}

		CORBA::Short x = server->s();
		ACE_ERROR ((LM_ERROR,"Server->s() returned %d\n", x));
		server->s(510);
		Structure the_in_structure;
		the_in_structure.i = x;
		the_in_structure.seq.length (10);
		the_in_structure.obj = CORBA::Object::_duplicate(server.in());
		Structure_var out_struct;
		CORBA::String_var name = CORBA::string_dup ("test");
		server->struct_test (12345,
				     the_in_structure,
				     out_struct.out(),
				     name.inout());
		CORBA::String_var outior =
			orb->object_to_string(out_struct->obj);
		ACE_DEBUG ((LM_DEBUG,"Got outior:\n%s\n", outior.in()));

		if (test_user_exception == 1)
		{
			server->raise_user_exception ();
		}
		else if (test_system_exception == 1)
		{
			server->raise_system_exception ();
		}
		if (test_user_exception != 0 ||
		    test_system_exception != 0)
		{
			ACE_DEBUG ((LM_DEBUG,"Expected exception not caught!\n"));
			return;
		}

		ACE_DEBUG ((LM_DEBUG,"Sending main ref\n"));
		CORBA::Object_var echo = server->echo_object (server.in());
		ACE_DEBUG ((LM_DEBUG,"Sending nil ref\n"));
		echo = server->echo_object (nilobj.in());

		CORBA::Any a;
		a <<= "String through Any";
		CORBA::Boolean success = server->any_test (a);
		ACE_DEBUG ((LM_DEBUG,"any_test(string) returned %d\n",success));

		a <<= server.in();
		success = server->any_test (a);
		ACE_DEBUG ((LM_DEBUG,"any_test(objref) returned %d\n",success));

		for (int i = 0; i != niterations; ++i)
		{
			the_in_structure.i = i;
			CORBA::String_var name = CORBA::string_dup ("the name");

			Structure_var the_out_structure;

			CORBA::Long r =
				server->struct_test (i,
						     the_in_structure,
						     the_out_structure.out (),
						     name.inout ());

			ACE_DEBUG ((LM_DEBUG,
				    "DSI_Simpler_Server ====\n"
				    "    x = %d\n"
				    "    i = %d\n"
				    "    length = %d\n"
				    "    name = <%s>\n",
				    r,
				    the_out_structure->i,
				    the_out_structure->seq.length (),
				    name.in ()));

			if (r != i)
			{
				ACE_DEBUG ((LM_DEBUG,
					    "(%P|%t) unexpected result = %d for %d",
					    r, i));
			}
		}
	}
	catch (const test_exception& ex)
	{
		ex._tao_print_exception ("Client: exception caught - ");

		ACE_DEBUG ((LM_DEBUG,
			    "error code: %d\n"
			    "error info: %s\n"
			    "status: %s\n",
			    ex.error_code,
			    ex.error_message.in (),
			    ex.status_message.in ()));
	}
	catch (const CORBA::NO_PERMISSION& ex)
	{
		ex._tao_print_exception ("Client: exception caught - ");
	}
	catch (const CORBA::SystemException& sysEx)
	{
		sysEx._tao_print_system_exception ();
	}
	catch (const CORBA::Exception& ex)
	{
		ex._tao_print_exception ("Client: exception caught - ");
	}
}

void do_union_test (Simple_Server_var &server, CORBA::Object_var& obj)
{
	Test_Union arg;
	arg.obj(obj);
	server->give_union (arg);
}

int do_shutdown_test (Simple_Server_var &server)
{
	ACE_DEBUG ((LM_DEBUG,
		    "[client] invoking shutdown on the server \n "));
	try
	{
		server->shutdown ();
	}
	catch (const CORBA::Exception& ex)
	{
		ex._tao_print_exception ("Client: exception caught during shutdown - ");
		return 1;
	}
	return 0;
}


int
main (int argc, char *argv[])
{
	Simple_Server_var server;
	CORBA::ORB_var orb = CORBA::ORB_init (argc, argv);
	if (parse_args (argc, argv) != 0)
		return 1;
	int result = 0;
	do_primary_test (orb, server);
	CORBA::Object_var obj = orb->string_to_object (direct);
	do_union_test (server, obj);
	if (do_shutdown)
		result = do_shutdown_test (server);
	server = Simple_Server::_nil();
	ACE_DEBUG ((LM_DEBUG,"Shutting down and destrying ORB.\n"));
	orb->destroy();
	ACE_DEBUG ((LM_DEBUG,"ORB destroyed\n"));
	return result;
};
