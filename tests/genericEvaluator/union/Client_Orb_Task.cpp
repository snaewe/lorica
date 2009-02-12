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

#include "Client_Orb_Task.h"

Client_Orb_Task::Client_Orb_Task(CORBA::ORB_ptr orb) : orb_(CORBA::ORB::_duplicate(orb)){
	// NOP
}

int Client_Orb_Task::svc(void){
	ACE_DEBUG((LM_DEBUG, "Client::Task::Starting client ORB\n"));
	try {
		orb_->run();
	} catch(const CORBA::Exception& ex) {
		ACE_ERROR((LM_ERROR, "Client::Task::Caught exception => %s\n", ex._name()));
		return -1;
	}

	ACE_DEBUG((LM_DEBUG, "Client::Task::Client ORB stopped\n"));
	return 0;
}
