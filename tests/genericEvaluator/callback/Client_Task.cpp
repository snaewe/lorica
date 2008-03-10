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

#include "Client_Task.h"

ACE_RCSID(Muxing, Client_Task, "Client_Task.cpp,v 1.1 2001/06/24 13:54:49 bala Exp")

Client_Task::Client_Task (Test::Sender_ptr reply_gen,
			  Test::Receiver_ptr us,
			  ACE_Thread_Manager *thr_mgr)
: ACE_Task_Base (thr_mgr)
	, sender_(Test::Sender::_duplicate (reply_gen))
	, us_ (Test::Receiver::_duplicate (us))

{
}

int
Client_Task::svc (void)
{
	ACE_DEBUG ((LM_DEBUG, "(%P|%t) Starting client task\n"));

	try
	{
		for (int i = 0; i != 1; ++i)
		{
			ACE_DEBUG ((LM_DEBUG,
				    "TAO (%P|%t) sending oneways...\n"));

			this->sender_->send_ready_message (this->us_.in ());
		}
	}
	catch (const CORBA::Exception& ex)
	{
		ex._tao_print_exception ("Caught Exception");
		return -1;
	}
	ACE_DEBUG ((LM_DEBUG, "(%P|%t) Client task finished\n"));
	return 0;
}
