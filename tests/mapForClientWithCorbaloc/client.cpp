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

#include "TestC.h"
#include "lorica/ReferenceMapperC.h"

const char *lorica_ior = "corbaloc::localhost:10951/LORICA_REFERENCE_MAPPER";
const char *server_ior = "corbaloc::localhost:20951/TEST_HELLO";

int
main (int argc, char *argv[])
{
	int retv = 1;

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
		CORBA::String_var unmapped_string = hello->get_string();
		ACE_DEBUG ((LM_DEBUG, "(%N|%l) string returned from unmapped object: <%s>\n",
			    unmapped_string.in ()));
 
		CORBA::Object_var tmp = mapper->as_client_with_corbaloc(server_ior,									
                                                                        "IDL:test.lorica/Test/Hello:1.0");
		hello = Test::Hello::_narrow(tmp.in());
		if (CORBA::is_nil(hello.in())) {
			ACE_ERROR_RETURN ((LM_DEBUG,
					   "Nil Test::Hello reference <corbaloc::localhost:20951/TEST_HELLO>\n"),
					  1);
		}
		CORBA::String_var mapped_string = hello->get_string();
		ACE_DEBUG ((LM_DEBUG, "(%N|%l) string returned from mapped object:   <%s>\n",
			    mapped_string.in ()));

		if (!strcmp(mapped_string.in(), unmapped_string.in()))
			retv = 0;

		hello->shutdown ();
		orb->destroy ();
	}
	catch (const CORBA::Exception& ex) {
		ex._tao_print_exception ("Exception caught:");
		retv = 1;
	}

	return retv;
}
