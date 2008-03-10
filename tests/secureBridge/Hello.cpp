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
