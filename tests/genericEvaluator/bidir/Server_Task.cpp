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

#include "Server_Task.h"

ACE_RCSID(Muxing, Server_Task, "Server_Task.cpp,v 1.1 2001/06/24 13:54:49 bala Exp")

Server_Task::Server_Task (CORBA::ORB_ptr orb,
			  ACE_Thread_Manager *thr_mgr)
: ACE_Task_Base (thr_mgr)
	, orb_ (CORBA::ORB::_duplicate (orb))
{
}

int
Server_Task::svc (void)
{
	ACE_DEBUG ((LM_DEBUG, "(%P|%t) Starting server task\n"));
	try
	{
		// run the test for at most 10 seconds...
		ACE_Time_Value tv (10, 0);
		this->orb_->run (tv);
	}
	catch (const CORBA::Exception& ex)
	{
		return -1;
	}
	ACE_DEBUG ((LM_DEBUG, "(%P|%t) Server task finished\n"));
	return 0;
}
