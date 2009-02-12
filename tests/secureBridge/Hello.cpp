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

ACE_RCSID(Hello, Hello, "$Id: Hello.cpp 1877 2007-05-17 19:25:33Z mesnierp $")

Hello::Hello (void  (*sh)())
: do_shutdown_(sh)
{
}

char *
Hello::get_string (void)
	ACE_THROW_SPEC ((CORBA::SystemException))
{
	ACE_DEBUG ((LM_DEBUG,"Get_String invoked\n"));
	return CORBA::string_dup ("Hello there!");
}

void
Hello::shutdown (void)
	ACE_THROW_SPEC ((CORBA::SystemException))
{
	this->do_shutdown_ ();
}
