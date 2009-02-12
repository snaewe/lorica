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

#ifndef SERVER_AGENT_I_H
#define SERVER_AGENT_I_H

#include "lorica/ServerAgentS.h"
#include <tao/PortableServer/Servant_Base.h>
#include "lorica/ReferenceMapperC.h"

class ServerAgent_i : public POA_Lorica::ServerAgent
{
public:
	struct ListNode
	{
		ListNode (PortableServer::ServantBase * native,
			  CORBA::Object_ptr mapped);
		PortableServer::ServantBase_var native_;
		CORBA::Object_ptr mapped_;
		ListNode *next_;
	};

	ServerAgent_i (Lorica::ReferenceMapper_ptr proxy);
	~ServerAgent_i ();
	void error_occured (CORBA::Long code,
			    const char *message);
	void proxy_disconnect ();

	void add_managed_ref (PortableServer::ServantBase * native,
			      CORBA::Object_ptr mapped);
	void remove_managed_ref (PortableServer::ServantBase * native);

private:
	ListNode * head_;
	Lorica::ReferenceMapper_var proxy_;
};
#endif /* SERVER_AGENT_I_H */
