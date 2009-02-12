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

ACE_RCSID(Hello, Hello, "$Id: Hello.cpp 1574 2007-03-05 02:48:36Z mesnierp $")

Hello::Hello (CORBA::ORB_ptr orb)
: orb_ (CORBA::ORB::_duplicate (orb))
{
}

char *
Hello::get_string (void)
	ACE_THROW_SPEC ((CORBA::SystemException))
{
	return CORBA::string_dup ("Hello there!");
}

void
Hello::shutdown (void)
	ACE_THROW_SPEC ((CORBA::SystemException))
{
	this->orb_->shutdown (0);
}
