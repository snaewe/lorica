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

#include "TestC.h"
#include <ace/Get_Opt.h>
#include "lorica/ReferenceMapperC.h"

ACE_RCSID(Hello, client, "$Id: client.cpp 1877 2007-05-17 19:25:33Z mesnierp $")

const char *ior = "file://mapped.ior";
const char *lorica_ior = "corbaloc::localhost:10961/LORICA_REFERENCE_MAPPER";
int
parse_args (int argc, char *argv[])
{
	ACE_Get_Opt get_opts (argc, argv, "p:k:");
	int c;

	while ((c = get_opts ()) != -1)
		switch (c)
		{
		case 'k':
			ior = get_opts.opt_arg ();
			break;
		case 'p':
			lorica_ior = get_opts.opt_arg ();
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
	try
	{
		CORBA::ORB_var orb =
			CORBA::ORB_init (argc, argv, "");

		if (parse_args (argc, argv) != 0)
			return 1;

		CORBA::Object_var obj =
			orb->string_to_object (lorica_ior);
		Lorica::ReferenceMapper_var mapper =
			Lorica::ReferenceMapper::_narrow(obj.in());
		if (CORBA::is_nil(mapper.in()))
			ACE_ERROR_RETURN ((LM_ERROR,
					   "Cannot get reference to Lorica "
					   "reference mapper\n"),1);

		CORBA::Object_var tmp = orb->string_to_object(ior);

		ACE_DEBUG ((LM_DEBUG,"Setting up bridge between loricae\n"));
		tmp = mapper->as_client (tmp.in());

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

		ACE_DEBUG ((LM_DEBUG, "(%P|%t) - string returned <%s>\n",
			    the_string.in ()));
		hello->shutdown ();

		orb->destroy ();
	}
	catch (const CORBA::Exception& ex)
	{
		ex._tao_print_exception ("Exception caught:");
		return 1;
	}

	return 0;
}
