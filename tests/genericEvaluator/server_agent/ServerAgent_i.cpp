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

#include "ServerAgent_i.h"

ServerAgent_i::ServerAgent_i (Lorica::ReferenceMapper_ptr p)
	: head_ (0),
	  proxy_ (Lorica::ReferenceMapper::_duplicate(p))
{
}

ServerAgent_i::~ServerAgent_i ()
{
	while (head_)
	{
		ListNode *ln = head_->next_;
		delete head_;
		head_ = ln;
	}
}

ServerAgent_i::ListNode::ListNode (PortableServer::ServantBase * native,
				   CORBA::Object_ptr mapped)
	: native_ (PortableServer::ServantBase_var::_duplicate (native)),
	  mapped_ (CORBA::Object::_duplicate (mapped)),
	  next_ (0)
{
}

void
ServerAgent_i::error_occured (CORBA::Long code,
			      const char *message)
{
	ACE_DEBUG ((LM_INFO, "ERROR occurred in the proxy.\n"
		    "error code: %d \n"
		    "error message :%s\n",
		    code, message));

}

void
ServerAgent_i::proxy_disconnect ()
{
	ACE_DEBUG((LM_INFO, "The proxy is shutting down\n"));
}

void
ServerAgent_i::add_managed_ref (PortableServer::ServantBase * native,
				CORBA::Object_ptr mapped)
{
	ListNode *ln = new ListNode(native,mapped);
	if (this->head_ == 0)
		this->head_ = ln;
	else
	{
		ListNode *itr = this->head_;
		while (itr->next_)
			itr = itr->next_;
		itr->next_ = ln;
	}
}

void
ServerAgent_i::remove_managed_ref (PortableServer::ServantBase *native)
{
	ListNode *itr = this->head_;
	ListNode *prev = 0;
	while (itr)
	{
		if (native == itr->native_.in())
		{
			proxy_->remove_server(itr->mapped_);
			if (prev)
				prev->next_ = itr->next_;
			else
				this->head_ = itr->next_;
			delete itr;
			break;
		}
		prev = itr;
		itr = itr->next_;
	}
}
