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

#ifndef TAO_TESTS_ONEWAYS_INVOKING_TWOWAYS_RECEIVER_I_H
#define TAO_TESTS_ONEWAYS_INVOKING_TWOWAYS_RECEIVER_I_H
#include <ace/pre.h>

#include "TestS.h"

#if defined (_MSC_VER)
# if (_MSC_VER >= 1200)
#  pragma warning(push)
# endif /* _MSC_VER >= 1200 */
# pragma warning (disable:4250)
#endif /* _MSC_VER */

/// Implement the Test::Receiver interface
/**
 * Skeleton implementation..
 */

class Receiver_i
	: public virtual POA_Test::Receiver
	, public virtual PortableServer::RefCountServantBase
{
public:
	/// Constructor
	Receiver_i (CORBA::ORB_ptr orb,
		    Test::Sender_ptr sender,
		    CORBA::ULong iter);

	///Destructor
	virtual ~Receiver_i (void);

	// = The skeleton methods
	virtual CORBA::Long receive_call ();

	/// Retun the number of calls that every thread would make...
	virtual CORBA::Long get_call_count ();

private:
	/// Our Orb...
	CORBA::ORB_var orb_;

	/// THe sender..
	Test::Sender_var sender_;

	/// Number of invocations to be made on the sender..
	CORBA::ULong iteration_;

	/// Number of calls processed so far..
	CORBA::ULong no_calls_;
};

#include <ace/post.h>
#endif /*TAO_TESTS_ONEWAYS_INVOKING_TWOWAYS_RECEIVER_I_H*/
