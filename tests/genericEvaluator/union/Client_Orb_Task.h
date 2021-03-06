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

#ifndef CLIENT_ORB_TASK_H
#define CLIENT_ORB_TASK_H

#include <tao/corba.h>
#include <ace/Task.h>

class Client_Orb_Task : public ACE_Task_Base
{
public:
	Client_Orb_Task(CORBA::ORB_ptr orb);
	int svc (void);

private:
	CORBA::ORB_var orb_;
};

#endif /* CLIENT_ORB_TASK_H */
