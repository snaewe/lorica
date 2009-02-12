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

#include "Receiver_i.h"

ACE_RCSID(Oneways_Invoking_Twoways, Receiver_i, "Receiver_i.cpp,v 1.3 2001/08/10 13:08:56 bala Exp")

Receiver_i::Receiver_i (CORBA::ORB_ptr orb,
			Test::Sender_ptr sender,
			CORBA::ULong iter)
: orb_ (CORBA::ORB::_duplicate (orb)),
	sender_ (Test::Sender::_duplicate (sender)),
	iteration_ (iter),
	no_calls_ (0)
{
}

Receiver_i::~Receiver_i (void)
{
}



CORBA::Long
Receiver_i::receive_call ()
{
	ACE_DEBUG ((LM_DEBUG,
		    "(%P|%t)  Doing a ping... \n"));

	this->sender_->ping ();

	return this->no_calls_++;
}

CORBA::Long
Receiver_i::get_call_count ()
{
	return this->iteration_;
}
