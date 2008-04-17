/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 *    Lorica source file.
 *    Copyright (C) 2007-2008 OMC Denmark ApS.
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

#include "defines/build-system-defs.h"

#include <ace/Dynamic_Service.h>
#include <tao/ORB_Core.h>

#include "lorica/MapperRegistry.h"
#include "lorica/ReferenceMapValue.h"
#include "lorica/debug.h"

#include "proxy/ReferenceMapper_i.h"

Lorica::ReferenceMapper_i::ReferenceMapper_i(CORBA::ORB_ptr orb,
					     IORTable::Table_ptr iort,
					     bool has_sec)
	: orb_(CORBA::ORB::_duplicate(orb)),
	  ior_table_(IORTable::Table::_duplicate(iort)),
	  has_security_(has_sec),
	  access_decision_(TAO::SL2::AccessDecision::_nil())
{
	this->registry_ = ACE_Dynamic_Service<Lorica_MapperRegistry>::instance("MapperRegistry");
	if (this->registry_ == 0 && Lorica_debug_level > 0)
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) %N:%l - no registry\n")));

	try {
		if (this->has_security_) {
			CORBA::Object_var obj =	orb_->resolve_initial_references("SecurityLevel2:SecurityManager");
			SecurityLevel2::SecurityManager_var sl2sm = SecurityLevel2::SecurityManager::_narrow(obj.in());
			SecurityLevel2::AccessDecision_var sl2ad = sl2sm->access_decision();
			this->access_decision_ = TAO::SL2::AccessDecision::_narrow(sl2ad.in());
		} else {
			if (Lorica_debug_level > 0)
				ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) %N:%l - SSLIOP not available\n")));
		}
	}
	catch (CORBA::Exception &) {
		ACE_ERROR ((LM_ERROR, "%N:%l - cannot initialize access decision object\n"));
	}
}

Lorica::ReferenceMapper_i::~ReferenceMapper_i(void)
{
}

void
Lorica::ReferenceMapper_i::allow_insecure_access(CORBA::Object_ptr self)
{
	if (this->has_security_ && !CORBA::is_nil(this->access_decision_))
		{
			CORBA::String_var orbid = this->orb_->id();
			PortableServer::ObjectId_var oid;
			CORBA::OctetSeq_var poaid;

			if (this->registry_->poa_and_object_id (self, poaid.out(), oid.out()))
				this->access_decision_->add_object(orbid.in(), poaid.in(), oid.in(),
								   true);
		}
}

CORBA::Object_ptr
Lorica::ReferenceMapper_i::as_secure_server(CORBA::Object_ptr orig,
					    const char *corbaloc_name,
					    Lorica::ServerAgent_ptr agent)
	throw (CORBA::SystemException,
	       Lorica::ReferenceMapper::SecurityNotAvailable)
{
	if (!this->has_security_)
		throw Lorica::ReferenceMapper::SecurityNotAvailable();

	return this->as_server_i(true, orig, corbaloc_name, agent);
}


CORBA::Object_ptr
Lorica::ReferenceMapper_i::as_server (CORBA::Object_ptr orig,
				      const char *corbaloc_name,
				      Lorica::ServerAgent_ptr agent)
	throw (CORBA::SystemException)
{
	return this->as_server_i (false, orig, corbaloc_name, agent);
}

CORBA::Object_ptr
Lorica::ReferenceMapper_i::as_server_i (bool require_secure,
					CORBA::Object_ptr orig,
					const char *corbaloc_name,
					Lorica::ServerAgent_ptr agent)
	throw (CORBA::SystemException)
{
	Lorica::ReferenceMapValue_var rmv = this->registry_->map_reference(orig, true, require_secure);

	if (rmv.get() == 0) {
		if(Lorica_debug_level > 0)
			ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) %N:%l - registry returned a null ReferenceMapValue\n")));

		return CORBA::Object::_nil();
	}

	if (Lorica_debug_level > 5)
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) %N:%l - registry returned a non-null rmv\n")));

	if (corbaloc_name[0] != 0 && !CORBA::is_nil(this->ior_table_.in())) {
		CORBA::String_var mapped = this->orb_->object_to_string(rmv->mapped_ref_.in());
		if(Lorica_debug_level > 5) {
			ACE_DEBUG((LM_DEBUG,
				   ACE_TEXT("(%P|%t) %N:%l - adding mapped value to IORTable, %s, %s\n"),
				   corbaloc_name,mapped.in()));
		}

		try {
			ior_table_->bind (corbaloc_name,mapped.in());
			if (Lorica_debug_level > 5)
				ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) %N:%l - bound to ior table\n")));
			rmv->ior_table_name_ = corbaloc_name;
		}
		catch (IORTable::AlreadyBound &) {
			ACE_ERROR((LM_ERROR,
				   "(%P|%t) %N:%l - ior_table cannot bind to %s\n",
				   corbaloc_name));
		}
	}

	rmv->require_secure_ = require_secure;
	if (!require_secure)
		this->allow_insecure_access(rmv->mapped_ref_.in());

	if (!CORBA::is_nil(agent))
		rmv->agent_ = Lorica::ServerAgent::_duplicate(agent);

	return CORBA::Object::_duplicate(rmv->mapped_ref_.in());
}

CORBA::Object_ptr
Lorica::ReferenceMapper_i::as_client(CORBA::Object_ptr orig)
	throw (CORBA::SystemException)
{
	Lorica::ReferenceMapValue_var rmv = this->registry_->map_reference(orig, false);
	if (rmv.get() == 0)
		return CORBA::Object::_nil();

	return CORBA::Object::_duplicate(rmv->mapped_ref_);
}

void
Lorica::ReferenceMapper_i::remove_server(CORBA::Object_ptr mapped)
	throw (CORBA::SystemException)
{
	Lorica::ReferenceMapValue_var rmv = this->registry_->remove_reference (mapped, true);
	if ((rmv.get() != 0) && (rmv->ior_table_name_ != "")) {
		if (Lorica_debug_level > 5) {
			ACE_DEBUG((LM_DEBUG,
				   ACE_TEXT("(%P|%t) %N:%l - unbinding %s from IOR Table\n"),
				   rmv->ior_table_name_.c_str()));
		}

		this->ior_table_->unbind(rmv->ior_table_name_.c_str());
	} else {
		if (Lorica_debug_level > 5)
			ACE_DEBUG((LM_DEBUG,
				   ACE_TEXT("(%P|%t) %N:%l - remove reference returned rmv = 0x%x\n"),
				   rmv.get()));
	}

	// rmv releases reference to value which cleans itself up.
}

void
Lorica::ReferenceMapper_i::remove_client(CORBA::Object_ptr mapped)
	throw (CORBA::SystemException)
{
	Lorica::ReferenceMapValue_var rmv = this->registry_->remove_reference(mapped, false);
}

CORBA::Boolean
Lorica::ReferenceMapper_i::supports_security(void)
	throw (CORBA::SystemException)
{
	return this->has_security_;
}
