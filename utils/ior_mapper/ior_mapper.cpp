/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: t; c-basic-offset: 2 -*- */

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

#include <ace/Get_Opt.h>
#include <tao/Stub.h>
#include "lorica/ReferenceMapperC.h"

const char *target_ior = "file://target.ior";
const char *ior_table_key = "IORTableKey";
const char *mapped_file = "mapped.ior";

const char *lorica_ior = "corbaloc::localhost:4080/LORICA_REFERENCE_MAPPER";

int
parse_args (int argc, char *argv[])
{
	ACE_Get_Opt get_opts (argc, argv, "i:l:m:n:");
	int c;

	while ((c = get_opts ()) != -1)
		switch (c)
		{
		case 'i':
			target_ior = get_opts.opt_arg ();
			break;

		case 'l':
			lorica_ior = get_opts.opt_arg ();
			break;

		case'm':
			mapped_file = get_opts.opt_arg ();
			break;

		case 'n':
			ior_table_key = get_opts.opt_arg ();
			break;

		case '?':
		default:
			ACE_ERROR_RETURN ((LM_ERROR,
												 "usage:  %s "
												 "-i <original ior> "
												 "-l <lorica ior> "
												 "-m <mapped ior file> "
												 "-n <iortable key name> "
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
	CORBA::ORB_var orb;
	CORBA::Object_var obj;
	CORBA::Object_var target;
	try
		{
			orb = CORBA::ORB_init (argc, argv, "");
		}
	catch (const CORBA::Exception& ex)
		{
			ex._tao_print_exception ("ORB_init exception caught - ");
			return 1;
		}
	
	
	if (parse_args (argc, argv) != 0)
		return 1;
	
	try
		{
			target = orb->string_to_object (target_ior);
			if (CORBA::is_nil (target.in()) || target->_non_existent())
				{
					ACE_ERROR_RETURN ((LM_ERROR,
														 "Cannot reach target object\n"), 1);
				}
		}
	catch (const CORBA::Exception& ex)
	{
		ex._tao_print_exception ("Verifying target exception caught - ");
		return 1;
	}

	try
		{
			obj =	orb->string_to_object (lorica_ior);
			Lorica::ReferenceMapper_var mapper =
				Lorica::ReferenceMapper::_narrow(obj.in());
			if (CORBA::is_nil(mapper.in()))
				ACE_ERROR_RETURN ((LM_ERROR,
													 "Cannot get reference to Lorica "
													 "reference mapper\n"),1);

			// getting the mapped object
			obj = mapper->as_server(target.in(),ior_table_key,
															Lorica::ServerAgent::_nil());
			if (CORBA::is_nil (obj.in()))
				ACE_ERROR_RETURN ((LM_ERROR,
													 "Lorica reference mapper returned a nil "
													 "mapped reference.\n"),1);

			CORBA::String_var mapped_ior =
				orb->object_to_string (obj.in());

			FILE *output_file= ACE_OS::fopen (mapped_file, "w");
			if (output_file == 0)
				ACE_ERROR_RETURN ((LM_ERROR,
													 "Cannot open output file for writing IOR: %s\n",
													 mapped_file),
													1);
			ACE_OS::fprintf (output_file, "%s", mapped_ior.in());
			ACE_OS::fclose (output_file);

		}
	catch (const CORBA::Exception& ex)
		{
			ex._tao_print_exception ("Accessing Lorica exception caught - ");
			return 1;
		}

	return 0;
}
