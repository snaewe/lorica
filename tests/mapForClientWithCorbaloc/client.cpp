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

const char *lorica_ior = "corbaloc::localhost:10951/LORICA_REFERENCE_MAPPER";
const char *server_ior = "corbaloc::localhost:20951/TEST_HELLO";

int
main (int argc, char *argv[])
{
	try {
		CORBA::ORB_var orb = CORBA::ORB_init(argc, argv, "");

		CORBA::Object_var obj = orb->string_to_object(lorica_ior);
		Lorica::ReferenceMapper_var mapper = Lorica::ReferenceMapper::_narrow(obj.in());
		if (CORBA::is_nil(mapper.in()))
			ACE_ERROR_RETURN ((LM_ERROR,
					   "Cannot get reference to Lorica "
					   "reference mapper\n"),1);

		obj = orb->string_to_object(server_ior);
		Test::Hello_var hello = Test::Hello::_narrow(obj.in());
		CORBA::String_var the_string = hello->get_string();
		ACE_DEBUG ((LM_DEBUG, "(%N|%l) string returned from unmapped object <%s>\n",
			    the_string.in ()));
 
		CORBA::Object_var tmp = mapper->as_client_with_corbaloc(server_ior,									
                                                                        "IDL:test.lorica/Test/Hello:1.0");
		hello = Test::Hello::_narrow(tmp.in());
		if (CORBA::is_nil(hello.in())) {
			ACE_ERROR_RETURN ((LM_DEBUG,
					   "Nil Test::Hello reference <corbaloc::localhost:20951/TEST_HELLO>\n"),
					  1);
		}
		the_string = hello->get_string();
		ACE_DEBUG ((LM_DEBUG, "(%N|%l) string returned from mapped object<%s>\n",
			    the_string.in ()));

		hello->shutdown ();
		orb->destroy ();
	}
	catch (const CORBA::Exception& ex) {
		ex._tao_print_exception ("Exception caught:");
		return 1;
	}

	return 0;
}
