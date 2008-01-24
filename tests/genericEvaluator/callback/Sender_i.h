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

#ifndef TAO_TESTS_ONEWAYS_INVOKING_TWOWAYS_SENDER_I_H
#define TAO_TESTS_ONEWAYS_INVOKING_TWOWAYS_SENDER_I_H
#include <ace/pre.h>

#include "TestS.h"

#if defined (_MSC_VER)
# if (_MSC_VER >= 1200)
#  pragma warning(push)
# endif /* _MSC_VER >= 1200 */
# pragma warning (disable:4250)
#endif /* _MSC_VER */

/// Implement the Test::Sender interface
/**
 * Implements the oneway call send_ready_message. This in turn calls
 * the receiver with two way invocations..
 */

class Sender_i
	: public virtual POA_Test::Sender
	, public virtual PortableServer::RefCountServantBase
{
public:
	/// Constructor
	Sender_i (CORBA::ORB_ptr orb,
		  CORBA::ULong no);

	///Destructor
	virtual ~Sender_i (void);

	// = The skeleton methods
	virtual void active_objects (CORBA::Short ao);

	virtual void send_ready_message (Test::Receiver_ptr receiver);

	virtual void ping ();

private:
	/// Our local ORB ptr
	CORBA::ORB_ptr orb_;

	/// Number of invocations to be made on the receiver
	CORBA::ULong number_;

	/// Number of active objects who are trying to connect
	CORBA::Short active_objects_;
};

#include <ace/post.h>
#endif /*TAO_TESTS_ONEWAYS_INVOKING_TWOWAYS_SENDER_I_H*/
