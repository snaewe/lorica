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

#ifndef ONEWAYS_INVOKING_TWOWAYS_SERVER_TASK_H
#define ONEWAYS_INVOKING_TWOWAYS_SERVER_TASK_H
#include "ace/pre.h"

#include "tao/corba.h"
#include "ace/Task.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

/// Implement a Task to run the experiments using multiple threads.
class Server_Task : public ACE_Task_Base
{
public:
	/// Constructor
	Server_Task (CORBA::ORB_ptr orb,
		     ACE_Thread_Manager *thr_mgr);

	/// Thread entry point
	int svc (void);

private:
	/// Reference to the ORB
	CORBA::ORB_var orb_;
};

#include "ace/post.h"
#endif /* ONEWAYS_INVOKING_TWOWAYS_SERVER_TASK_H*/
