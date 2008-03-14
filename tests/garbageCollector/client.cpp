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

#include "TestC.h"
#include <ace/Get_Opt.h>

ACE_RCSID(Hello, client, "$Id: client.cpp 1626 2007-03-19 01:42:06Z mesnierp $")

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
			ior = get_opts.opt_arg ();
			break;

		case '?':
		default:
			ACE_ERROR_RETURN ((LM_ERROR,
					   "usage:  %s "
					   "-k <ior> "
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
  int result = 0;
	try
	{
		CORBA::ORB_var orb =
			CORBA::ORB_init (argc, argv, "");

		if (parse_args (argc, argv) != 0)
			return 1;

		CORBA::Object_var tmp = orb->string_to_object(ior);
		Test::Hello_var hello =
			Test::Hello::_narrow(tmp.in ());

		if (CORBA::is_nil (hello.in ()))
		{
			ACE_ERROR_RETURN ((LM_DEBUG,
					   "Nil Test::Hello reference <%s>\n",
					   ior),
					  1);
		}

		CORBA::String_var the_string =
			hello->get_string ();

		ACE_DEBUG ((LM_DEBUG, "Client - got first string, waiting 6\n"));

		ACE_OS::sleep (6); // wait until after one GC episode, but before
		                   // the server shuts down and try again.

		the_string = hello->get_string();

		ACE_DEBUG ((LM_DEBUG, "Client - got second string, waiting 15\n"));

		ACE_OS::sleep (15); // this gets us past the server shutdown,
                        // garbage collection, and server restart

		try {
			the_string = hello->get_string();
			ACE_ERROR ((LM_ERROR,"ERROR: Client got third string!\n"));
			result = 1;
		}
		catch (const CORBA::Exception& ex)
			{
				ACE_DEBUG ((LM_DEBUG,
										"Client - got expected exception on third try: %s\n",
										ex._name()));
			}

		orb->destroy ();
	}
	catch (const CORBA::Exception& ex)
	{
		ex._tao_print_exception ("Exception caught:");
		return 1;
	}

	return result;
}
