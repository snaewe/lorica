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

#include "test_i.h"
#include <tao/debug.h>
#include <ace/OS_NS_unistd.h>

Simple_Server_i::Simple_Server_i (CORBA::ORB_ptr orb,
				  ServerAgent_i* agent,
				  int do_timeout)
	: do_timeout_ (do_timeout),
	  delay_trigger_ (0),
	  orb_ (CORBA::ORB::_duplicate (orb)),
	  agent_ (agent)
{
}

void
Simple_Server_i::timeout_check()
{
	if (this->do_timeout_)
	{
		if (delay_trigger_)
			ACE_OS::sleep(3);
		else
			delay_trigger_ = 1;
	}
}

CORBA::Boolean
Simple_Server_i::any_test (const CORBA::Any &a)
{
	const char *target;
	CORBA::Boolean success = (a >>= target);
	if (success)
	{
		if (TAO_debug_level > 0)
			ACE_DEBUG ((LM_DEBUG,
				    "Simple_Server_i::any_test got \"%s\"\n",
				    target));
	}
	else
	{
		CORBA::Object_var obj;
		CORBA::Any::to_object to_obj(obj.inout());
		success = (a >>= to_obj);
		if (success)
		{
			if (TAO_debug_level > 0)
				ACE_DEBUG ((LM_DEBUG,
					    "Simple_Server_i::any_test got an object reference\n"));
		}
	}
	if (!success && TAO_debug_level > 0)
		ACE_DEBUG ((LM_DEBUG,
			    "Simple_Server_i::any_test failed\n"));
	return success;
}

CORBA::Long
Simple_Server_i::struct_test (CORBA::Long x,
			      const Structure& the_in_structure,
			      Structure_out the_out_structure,
			      char *&name)
{
	this->timeout_check();

	Structure *tmp;
	ACE_NEW_RETURN (tmp, Structure (the_in_structure), -1);
	the_out_structure = tmp;
	// The ORB
	if (TAO_debug_level > 0)
	{
		ACE_DEBUG ((LM_DEBUG,
			    "Simple_Server_i::struct_test ====\n"
			    "    x = %d\n"
			    "    i = %d\n"
			    "    length = %d\n"
			    "    name = <%s>\n",
			    x,
			    the_in_structure.i,
			    the_in_structure.seq.length (),
			    name));
	}

	return x;
}

CORBA::Object_ptr
Simple_Server_i::echo_object (CORBA::Object_ptr obj)
{
	this->timeout_check();
	if (TAO_debug_level > 0)
		ACE_DEBUG ((LM_DEBUG,
			    "Simple_Server_i::echo_object returning obj\n"));
	return CORBA::Object::_duplicate(obj);
}

void
Simple_Server_i::raise_user_exception ()
{
	this->timeout_check();

	throw test_exception (33, "reactor meltdown", "kaput");
}

void
Simple_Server_i::raise_system_exception ()
{
	this->timeout_check();

	throw CORBA::NO_PERMISSION ();
}

void
Simple_Server_i::shutdown ()
{
	agent_->remove_managed_ref (this);
	ACE_OS::sleep( 2 );
	this->orb_->shutdown (0);
}
