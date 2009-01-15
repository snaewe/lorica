/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 *    Lorica source file.
 *    Copyright (C) 2009 OMC Denmark ApS.
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

#include "Server_i.h"

// Implementation skeleton constructor
Test_Server_i::Test_Server_i(CORBA::ORB_ptr orb)
  : orb_(CORBA::ORB::_duplicate (orb))
{
}

// Implementation skeleton destructor
Test_Server_i::~Test_Server_i(void)
{
}

::CORBA::Boolean
Test_Server_i::receive_call(::Test::CallBack_ptr cb)
{
	static int ping_count = 0;
	::Test::CallBack_var call_back = cb;

	ping_count++;
	if (2 < ping_count) {
		orb_->shutdown();
                return (::CORBA::Boolean)1;
	}
	try {
		call_back->ping();
	}
        catch (const CORBA::Exception &e) {
                ACE_DEBUG((LM_CRITICAL, ACE_TEXT("%N:%l - %s\n"), e._info().c_str()));
                return (::CORBA::Boolean)0;
        }
        catch (...) {
                ACE_DEBUG((LM_CRITICAL, ACE_TEXT("%N:%l - Unknown C++ exception\n")));
                return (::CORBA::Boolean)0;
        }
	
	return (::CORBA::Boolean)1;
}

