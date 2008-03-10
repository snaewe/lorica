/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 *    Lorica header file.
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

#ifndef TAO_DSI_GATEWAY_TEST_I_H
#define TAO_DSI_GATEWAY_TEST_I_H

#include "testS.h"
#include "ServerAgent_i.h"

class Simple_Server_i : public POA_Simple_Server
{
	// = TITLE
	//   Simpler Server implementation
	//
	// = DESCRIPTION
	//   Implements the Simple_Server interface in test.idl
	//
public:
	Simple_Server_i (CORBA::ORB_ptr orb,
			 ServerAgent_i *agent,
			 int do_timeout);
	// ctor

	// = The Simple_Server methods.
	CORBA::Boolean any_test (const CORBA::Any &a);

	CORBA::Long struct_test (CORBA::Long x,
				 const Structure& the_in_structure,
				 Structure_out the_out_structure,
				 char *&name);

	CORBA::Object_ptr echo_object (CORBA::Object_ptr obj);

	void raise_user_exception ();

	void raise_system_exception ();

	void shutdown ();

private:
	void timeout_check();

	int do_timeout_;
	int delay_trigger_;
	CORBA::ORB_var orb_;
	ServerAgent_i * agent_;
};

#endif /* TAO_DSI_GATEWAY_TEST_I_H */
