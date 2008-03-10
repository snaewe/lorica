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

const char *ior = "file://mapped.ior";

int do_shutdown = 0;

int
parse_args (int argc, char *argv[])
{
	ACE_Get_Opt get_opts (argc, argv, "xk:");
	int c;

	while ((c = get_opts ()) != -1)
		switch (c)
		{
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
					   "\n",
					   argv [0]),
					  -1);
		}

	// Indicates sucessful parsing of the command line
	return 0;
}

void
do_primary_test (CORBA::ORB_var &orb, Forward::Test_var &server)
{
	try
		{
			ACE_DEBUG ((LM_DEBUG,"Client using ior source %s\n", ior));
			CORBA::Object_var object =
				orb->string_to_object (ior);

			server =
				Forward::Test::_narrow (object.in ());

			if (CORBA::is_nil (server.in ()))
				{
					ACE_ERROR ((LM_ERROR,
											"Object reference <%s> is nil\n",
											ior));
					return;
				}

			ACE_DEBUG((LM_DEBUG,"Client sending test string\n"));
			CORBA::String_var test_string = CORBA::string_dup ("123 look at mee");
			CORBA::String_var result = server->do_forward(test_string);
			if (ACE_OS::strcmp (result.in(), test_string.in()) != 0)
				ACE_ERROR ((LM_ERROR,
										"Client did not get the expected result\n"));
			else
				ACE_DEBUG((LM_DEBUG,"Client got echoed string\n"));

		}
	catch (CORBA::Exception &ex)
		{
			ACE_ERROR ((LM_ERROR,
									"Client caught exception: %s\n",ex._name()));
		}
}

int do_shutdown_test (Forward::Test_var &server)
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
	Forward::Test_var server;
	CORBA::ORB_var orb = CORBA::ORB_init (argc, argv);
	if (parse_args (argc, argv) != 0)
		return 1;
	int result = 0;
	do_primary_test (orb, server);
	if (do_shutdown)
		result = do_shutdown_test (server);
	server = Forward::Test::_nil();
	ACE_DEBUG ((LM_DEBUG,"Shutting down and destrying ORB.\n"));
	orb->destroy();
	ACE_DEBUG ((LM_DEBUG,"ORB destroyed\n"));
	return result;
};
