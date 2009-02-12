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

#include "Sender_i.h"

ACE_RCSID(Oneways_Invoking_Twoways, Sender_i, "Sender_i.cpp,v 1.3 2001/06/26 14:56:10 bala Exp")

Sender_i::Sender_i (CORBA::ORB_ptr orb,
		    CORBA::ULong no)
: orb_ (CORBA::ORB::_duplicate (orb)),
	number_ (no),
	active_objects_ (0)
{
}

Sender_i::~Sender_i (void)
{
}

void
Sender_i::active_objects (CORBA::Short no_threads)
{
	this->active_objects_ = no_threads;
}


void
Sender_i::send_ready_message (Test::Receiver_ptr receiver)
{
	ACE_DEBUG ((LM_DEBUG,
		    "Received a call ...\n"));

	ACE_DEBUG ((LM_DEBUG,
		    "Calling the Receiver ..\n"));

	for (CORBA::ULong i = 0;
	     i < this->number_;
	     ++i)
	{
		receiver->receive_call ();
	}

}

void
Sender_i::ping ()
{
	return;
}
