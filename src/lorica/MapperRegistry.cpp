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

#include <tao/PortableServer/POAManagerFactory.h>
#include <tao/Stub.h>

#if !defined (LORICA_LACKS_SSLIOP)
# include <orbsvcs/SecurityC.h>
#endif // LORICA_LACKS_SSLIOP

#include "MapperRegistry.h"
#include "ReferenceMapValue.h"
#include "EvaluatorBase.h"
#include "NullProxyMapper.h"
#include "ProxyMapper.h"
#include "debug.h"

Lorica_MapperRegistry::Lorica_MapperRegistry(void)
{
	this->init(0, 0);
}

int
Lorica_MapperRegistry::init(int,
			    ACE_TCHAR *[])
{
	this->null_mapper_ = 0;
	this->generic_mapper_ = 0;
	this->mappers_ = 0;
	this->mappers_ready_ = false;

	return 1;
}

// Shared object finalizer
int
Lorica_MapperRegistry::fini(void)
{
	if (this->null_mapper_ != 0)
		this->null_mapper_->decr_refcount();

	if (this->generic_mapper_ != 0)
		this->generic_mapper_->decr_refcount();

	if (this->mappers_ != 0)
		this->mappers_->destroy_chain();

	return 1;
}

void
Lorica_MapperRegistry::add_null_mapper_type(const std::string & typeId)
{
	if (this->null_mapper_ == 0)
		ACE_NEW(this->null_mapper_, Lorica::NullProxyMapper(*this));

	this->null_mapper_->add_type_id(typeId);
}

void
Lorica_MapperRegistry::create_default_null_mapper(void)
{
	if (this->mappers_ready_) {
		if (Lorica_debug_level > 0) {
			ACE_DEBUG((LM_DEBUG,
				   ACE_TEXT("(%P|%t) %N:%l - called after mappers are ready\n")));
		}

		return;
	}

	if (this->null_mapper_ == 0)
		ACE_NEW(this->null_mapper_, Lorica::NullProxyMapper(*this));
}

void
Lorica_MapperRegistry::set_generic_mapper(Lorica::ProxyMapper *mapper)
{
	if (this->mappers_ready_) {
		if (Lorica_debug_level > 0) {
			ACE_DEBUG((LM_DEBUG,
				   ACE_TEXT("(%P|%t) %N:%l - called after mappers are ready\n")));
		}

		return;
	}

	if (this->generic_mapper_ != 0)
		this->generic_mapper_->decr_refcount();
	this->generic_mapper_ = mapper;
}

void
Lorica_MapperRegistry::add_proxy_mapper(Lorica::ProxyMapper *mapper)
{
	if (this->mappers_ready_)
		return;

	if (this->mappers_ == 0) {
		this->mappers_ = mapper;
	} else
		this->mappers_->take_mapper(mapper);
}

void
Lorica_MapperRegistry::remove_proxy_mapper(Lorica::ProxyMapper *mapper)
{
	if (this->mappers_ == 0)
		return;

	if (this->mappers_ == mapper) {
		Lorica::ProxyMapper *new_head = this->mappers_->next();
		this->mappers_->decr_refcount();
		this->mappers_ = new_head;
	} else
		this->mappers_->remove_mapper(mapper);
}

void
Lorica_MapperRegistry::init_mappers(PortableServer::POAManager_ptr outward,
				    PortableServer::POAManager_ptr inward,
				    CORBA::ORB_ptr orb,
				    bool has_security)
{
	// first, prevent multiple activation
	if (this->mappers_ready_)
		return;

	// consolidate the mapper list, to append first the generic mapper and
	// then the null mapper.
	if (this->generic_mapper_ != 0) {
		if (Lorica_debug_level > 0) {
			ACE_DEBUG((LM_DEBUG,
				   ACE_TEXT("(%P|%t) %N:%l - adding generic mapper\n")));
		}

		this->add_proxy_mapper(this->generic_mapper_);
		this->generic_mapper_ = 0;
	}

	if (this->null_mapper_ != 0) {
		if (Lorica_debug_level > 0) {
			ACE_DEBUG((LM_DEBUG,
				   ACE_TEXT("(%P|%t) %N:%l - adding null mapper\n")));
		}

		this->add_proxy_mapper (this->null_mapper_);
		this->null_mapper_ = 0;
	}

	if (this->mappers_ != 0) {
		if (Lorica_debug_level > 0) {
			ACE_DEBUG((LM_DEBUG,
				   ACE_TEXT("(%P|%t) %N:%l - calling mapper init on the root.\n")));
		}

		this->mappers_->proxy_mapper_init(outward, inward, orb);
	}

	this->has_security_ = has_security;
#if !defined (LORICA_LACKS_SSLIOP)
	if (has_security) {
		this->sec_policies_.length(1);

		Security::QOP qop = Security::SecQOPIntegrityAndConfidentiality;
		CORBA::Any i_and_c;
		i_and_c <<= qop;

		// Create the Security::QOPPolicy.
		sec_policies_[0] = orb->create_policy(Security::SecQOPPolicy, i_and_c);
	} else
#endif // LORICA_LACKS_SSLIOP 
		this->sec_policies_.length(0);

	this->mappers_ready_ = true;
}

bool
Lorica_MapperRegistry::has_security(void) const
{
	return this->has_security_;
}


const CORBA::PolicyList &
Lorica_MapperRegistry::sec_policies(void) const
{
	return this->sec_policies_;
}


Lorica::ReferenceMapValue *
Lorica_MapperRegistry::map_reference(CORBA::Object_ptr native,
				     bool out_facing,
				     bool require_secure)
{
	// iterate over the mappers list until one is found that can
	// handle this type. Return the newly created map entry or 0.
	Lorica::ReferenceMapValue_var rmv;
	std::string typeId = native->_stubobj()->type_id.in();
	//  std::string typeId(native->_interface_repository_id());

	if (this->mappers_ != 0) {
		if (Lorica_debug_level > 2) {
			ACE_DEBUG((LM_DEBUG,
				   ACE_TEXT("(%P|%t) %N:%l - adding %s\n"),
				   typeId.c_str()));
		}

		rmv = this->mappers_->add_native(native,
						 typeId,
						 out_facing,
						 require_secure);
	} else {
		if (Lorica_debug_level > 0)
			ACE_DEBUG((LM_DEBUG,
				   ACE_TEXT("(%P|%t) %N:%l - no mappers registered\n")));
	}

	return rmv.release();
}

Lorica::ReferenceMapValue *
Lorica_MapperRegistry::remove_reference(CORBA::Object_ptr mapped,
					bool out_facing)
{
	// iterate over the mappers list until one is found that can
	// handle this type. Return the newly created map entry or 0.
	Lorica::ReferenceMapValue_var rmv;
	std::string typeId = mapped->_stubobj()->type_id.in();

	//  std::string typeId(mapped->_interface_repository_id());
	if (this->mappers_ != 0) {
		rmv = this->mappers_->remove_mapped(mapped,
						    typeId,
						    out_facing);
	}

	return rmv.release();
}

int
Lorica_MapperRegistry::initialize(void)
{
	return ACE_Service_Config::process_directive(ace_svc_desc_Lorica_MapperRegistry);
}

bool
Lorica_MapperRegistry::poa_and_object_id(CORBA::Object_ptr ref,
					 CORBA::OctetSeq *& poaid,
					 PortableServer::ObjectId *& objid)
{
	if (this->mappers_ == 0) {
		poaid = new CORBA::OctetSeq;
		objid = new PortableServer::ObjectId;

		return false;
	}

	return this->mappers_->poa_and_object_id(ref, poaid, objid);
}

ACE_STATIC_SVC_DEFINE(Lorica_MapperRegistry,
		      ACE_TEXT("MapperRegistry"),
		      ACE_SVC_OBJ_T,
		      &ACE_SVC_NAME(Lorica_MapperRegistry),
		      ACE_Service_Type::DELETE_THIS | ACE_Service_Type::DELETE_OBJ,
		      0)
ACE_FACTORY_DEFINE(Lorica_EvaluatorBase, Lorica_MapperRegistry)
