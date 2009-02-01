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

#include <ace/OS_NS_unistd.h>
#include <ace/OS_NS_string.h>
#include <ace/OS_NS_stdio.h>
#include <ace/Mutex.h>
#include <ace/INET_Addr.h>
#include <tao/PortableServer/PortableServer.h>
#include <tao/PortableServer/Servant_Base.h>
#include <tao/BiDir_GIOP/BiDirGIOP.h>
#include <tao/Object.h>
#include <tao/Stub.h>
#include <tao/Profile.h>
#include <tao/Tagged_Components.h>

#if !defined (LORICA_LACKS_SSLIOP)
#include <orbsvcs/SSLIOPC.h>
#endif

#include "ProxyMapper.h"
#include "MapperRegistry.h"
#include "ReferenceMapValue.h"
#include "EvaluatorBase.h"
#include "ProxyServant.h"
#include "ConfigBase.h"
#include "FileConfig.h"
#include "debug.h"

Lorica::MappedObjectId Lorica::ProxyMapper::mapped_object_id_ = {
	{'L', 'R', 'C', 'A'},
	'\x01',
	'0',
	0,
	0,
	0,
};

Lorica::ProxyMapper::ProxyMapper(Lorica_MapperRegistry & mr,
                                 const std::string & id)
	: evaluator_head_(0),
		mapped_values_(0),
		registry_(mr),
		id_ (id),
		ref_count_ (1),
		next_(0)
{
	Lorica::ProxyMapper::mapped_object_id_.pid = static_cast<ACE_UINT16>(ACE_OS::getpid());

	ACE_INET_Addr *host_addr_array = 0;
	size_t count = 0;
	Lorica::ProxyMapper::mapped_object_id_.hostid = 0;
	bool islocal = true;
	if (ACE::get_ip_interfaces(count, host_addr_array) == 0) {
		for (size_t i = 0; i < count && islocal; i++) {
			if (host_addr_array[i].get_type() == PF_INET
#ifdef ACE_HAS_IPV6
					|| host_addr_array[i].get_type() == PF_INET6
#endif
			    )
				{
					Lorica::ProxyMapper::mapped_object_id_.hostid =
						static_cast<ACE_UINT32>(host_addr_array[i].hash());
					islocal = host_addr_array[i].is_loopback();
				}
		}
#if 0
		if (Lorica::ProxyMapper::mapped_object_id_.hostid == 0) {
			// get the last 32 bits from the first address in the list.
			ACE_UINT32 *addr = reinterpret_cast<ACE_UINT32 *>(host_addr_array[0].get_addr());
			Lorica::ProxyMapper::mapped_object_id_.hostid = addr[3];
		}
#endif
	}
	delete [] host_addr_array;

	Lorica::Config * configuration = FILECONFIG::instance();

	if (configuration->getBooleanValue("CacheProxyReferences", false)) {
		std::string gc_period = configuration->get_value("GC_Period_Seconds");
		if (gc_period.length() == 0)
			this->mapped_values_ = new RMVByMapped;
		else
			{
				char * bad;
				time_t p = ACE_OS::strtol(gc_period.c_str(),&bad,10);
				if (*bad != '\0')
					p = 60; // default to 60 seconds, should we complain about param?
				this->mapped_values_ = new RMVByMapped (p);
			}
	}
}

Lorica::ProxyMapper::~ProxyMapper(void)
{
	delete this->evaluator_head_;

	// need to iterate over maps, remove each entry and decrement refcounts
	delete this->mapped_values_;

	if (this->next_)
		this->next_->decr_refcount();
}

CORBA::ORB_ptr
Lorica::ProxyMapper::orb(void) const
{
	return CORBA::ORB::_duplicate(this->orb_.in());
}

int
Lorica::ProxyMapper::proxy_mapper_init(PortableServer::POAManager_ptr outward,
                                       PortableServer::POAManager_ptr inward,
                                       CORBA::ORB_ptr orb)
{
	this->orb_ = CORBA::ORB::_duplicate(orb);

	CORBA::Object_var obj = orb->resolve_initial_references("POACurrent");
	this->poa_current_ = PortableServer::Current::_narrow(obj.in());

	obj = orb->resolve_initial_references("RootPOA");
	PortableServer::POA_var root = PortableServer::POA::_narrow (obj.in());

	CORBA::PolicyList policies(5);
	policies.length(4);
	policies[0] = root->create_id_assignment_policy(PortableServer::USER_ID);
	policies[1] = root->create_id_uniqueness_policy(PortableServer::MULTIPLE_ID);
	policies[2] = root->create_servant_retention_policy (PortableServer::NON_RETAIN);
	policies[3] = root->create_request_processing_policy (PortableServer::USE_DEFAULT_SERVANT);

	// initialize the POAs using the supplied Managers

	std::string poaname = this->id_ + "_i";
	this->in_facing_poa_ = root->create_POA(poaname.c_str(), inward, policies);

	PortableServer::ServantBase_var defserv = this->make_default_servant();
	this->in_facing_poa_->set_servant(defserv.in());

	policies.length(5);
	CORBA::Any arg;
	arg <<= BiDirPolicy::BOTH;
	policies[4] = orb->create_policy (BiDirPolicy::BIDIRECTIONAL_POLICY_TYPE,
																		arg);

	poaname = this->id_ + "_o";
	this->out_facing_poa_ = root->create_POA(poaname.c_str(), outward, policies);

	defserv = this->make_default_servant();
	this->out_facing_poa_->set_servant(defserv.in());

	if (this->next_ != 0)
		return this->next_->proxy_mapper_init(outward, inward, orb);

	return 1;
}

void
Lorica::ProxyMapper::add_evaluator(Lorica::EvaluatorBase *ev)
{
	if (this->evaluator_head_ == 0)
		this->evaluator_head_ = ev;
	else
		this->evaluator_head_->add_evaluator(ev);
}

void
Lorica::ProxyMapper::take_mapper(Lorica::ProxyMapper *p)
{
	if (this->next_ == 0)
		this->next_ = p;
	else
		this->next_->take_mapper(p);
}

void
Lorica::ProxyMapper::remove_mapper(Lorica::ProxyMapper *p)
{
	if (this->next_ == p)
		this->next_ = this->next_->next_;
	else if (this->next_ != 0)
		this->next_->remove_mapper(p);
}

void
Lorica::ProxyMapper::destroy_chain(void)
{
	if (this->next_ != 0) {
		this->next_->destroy_chain();
		this->next_->decr_refcount();
		this->next_ = 0;
	}
	this->decr_refcount();
}

Lorica::ProxyMapper *
Lorica::ProxyMapper::next(void)
{
	if (this->next_ != 0)
		this->next_->incr_refcount();

	return this->next_;
}

PortableServer::ObjectId *
Lorica::ProxyMapper::get_mapped_objectId(bool out_facing,
                                         ACE_UINT32 & index)
{
	size_t buflen = sizeof(Lorica::MappedObjectId);
	CORBA::Octet * buffer = PortableServer::ObjectId::allocbuf(buflen);
	ACE_OS::memcpy(buffer, &Lorica::ProxyMapper::mapped_object_id_, buflen);

	Lorica::MappedObjectId *ptr = reinterpret_cast<Lorica::MappedObjectId *>(buffer);
	ptr->in_out = out_facing ? 'O' : 'I';
	ptr->index = this->mapped_values_->next_index();
	index = ptr->index;

	return new PortableServer::ObjectId(buflen,
					    buflen,
					    buffer,
					    true);
}

Lorica::ProxyMapper::MappedStatus
Lorica::ProxyMapper::already_mapped(CORBA::Object_ptr native,
                                    bool out_facing,
                                    ACE_UINT32 & index)
{
	if (this->mapped_values_ != 0) {
		TAO::ObjectKey_var key = native->_key();
		if (key.ptr() == 0 || key->length() < sizeof (mapped_object_id_)) {
			if (Lorica_debug_level > 4) {
				ACE_DEBUG((LM_DEBUG,
					   ACE_TEXT("(%T) %N:%l - key is null\n")));
			}
			return NOT_MAPPED;
		}
		if (Lorica_debug_level > 4) {
			ACE_DEBUG((LM_DEBUG,
				   ACE_TEXT("(%T) %N:%l - key len = %d, mapped id len = %d\n"),
				   key->length(),
				   sizeof(mapped_object_id_)));
		}
		
		CORBA::Octet *buffer = key->get_buffer();
		Lorica::MappedObjectId temp;
		bool has_potential = false;
		size_t offset = key->length() - sizeof(mapped_object_id_);
		
		if (Lorica_debug_level > 4) {
			ACE_DEBUG((LM_DEBUG,
				   ACE_TEXT("(%T) %N:%l - Key at offset = ")));
			
			for (size_t x = offset; x < offset+6; x++)
				ACE_DEBUG((LM_DEBUG,
					   ACE_TEXT("'%02hhu' "),
					   buffer[x]));
			
			ACE_DEBUG((LM_DEBUG,
				   ACE_TEXT("\n")));
		}
		
		if (ACE_OS::memcmp(buffer+offset,
				   &mapped_object_id_, 
				   sizeof(mapped_object_id_.magic)) == 0) {
			has_potential = true;
			ACE_OS::memcpy (&temp, buffer+offset, sizeof (temp));
			if (Lorica_debug_level > 4) {
				ACE_DEBUG((LM_DEBUG,
					   ACE_TEXT("(%T) %N:%l - got a potential match\n")));
			}
		}
		
		if (has_potential) {
			if (temp.pid == mapped_object_id_.pid
			    && temp.hostid == mapped_object_id_.hostid) {
				char io = out_facing ? 'O' : 'I';
				index = temp.index;
				
				if (Lorica_debug_level > 4) {
					ACE_DEBUG((LM_DEBUG,
						   ACE_TEXT("(%T) %N:%l - got a match, index = %d, io = %c, in_out = %c\n"),
						   index,
						   io,
						   temp.in_out));
				}
				
				return temp.in_out == io ? ALREADY_MAPPED : REVERSE_MAPPED;
			}
			if (Lorica_debug_level > 2) {
				ACE_DEBUG((LM_WARNING, 
					   ACE_TEXT("(%T) %N:%l - got a reference mapped by another Lorica - pid = %d and hostid = %d\n"),
					   temp.pid,
					   temp.hostid));
			}
		}
	}
	if (Lorica_debug_level > 4) {
		ACE_DEBUG((LM_DEBUG,
			   ACE_TEXT("(%T) %N:%l - failed to match\n")));
	}
	
	return NOT_MAPPED;
}

bool
Lorica::ProxyMapper::supports_secure(CORBA::Object_ptr native)
{
#if !defined (LORICA_LACKS_SSLIOP)
	if (!registry_.has_security())
		return false;

	TAO_Stub * stub = native->_stubobj();
	TAO_Profile * profile = stub->profile_in_use();
	TAO_Tagged_Components & components = profile->tagged_components();

	IOP::TaggedComponent tagged_component;
	tagged_component.tag = SSLIOP::TAG_SSL_SEC_TRANS;

	if (components.get_component(tagged_component)) {
		const CORBA::Octet *buf = tagged_component.component_data.get_buffer();

		TAO_InputCDR in_cdr(reinterpret_cast<const char *> (buf),
												tagged_component.component_data.length());

		// Extract the Byte Order.
		CORBA::Boolean byte_order;
		if ((in_cdr >> ACE_InputCDR::to_boolean (byte_order)) == 0)
			return false;
		in_cdr.reset_byte_order(static_cast<int> (byte_order));

		// Extract endpoints sequence.
		SSLIOP::SSL ssl;

		if ((in_cdr >> ssl))
			return true;
	}
#else
	ACE_UNUSED_ARG (native);
#endif // LORICA_LACKS_SSLIOP
	return false;

}

Lorica::ReferenceMapValue *
Lorica::ProxyMapper::add_native_unchecked(CORBA::Object_ptr native,
					  const std::string & typeId,
					  bool out_facing,
					  bool require_secure)
{
	Lorica::ReferenceMapValue_var rmv;

	rmv = new Lorica::ReferenceMapValue;

	if (this->supports_secure (native))
		rmv->orig_ref_ = native->_set_policy_overrides(this->registry_.sec_policies(), CORBA::SET_OVERRIDE);
	else
		rmv->orig_ref_ = CORBA::Object::_duplicate(native);

	ACE_UINT32 index = 0;
	PortableServer::ObjectId_var oid;
	if (this->mapped_values_ == 0) {
		char *nior = orb_->object_to_string(native);
		oid = PortableServer::string_to_ObjectId(nior);
		CORBA::string_free(nior);
	} else
		oid = this->get_mapped_objectId(out_facing,index);

	PortableServer::POA_var poa = out_facing ? this->out_facing_poa_ : this->in_facing_poa_;

	rmv->mapped_ref_ = poa->create_reference_with_id(oid.in(), typeId.c_str());
	rmv->require_secure_ = require_secure;

	// put new value in maps
	if (this->mapped_values_ != 0) {
		if (Lorica_debug_level > 2) {
			ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T) %N:%l - adding \"%C\" to map\n"), typeId.c_str()));
		}

		this->mapped_values_->bind(index, rmv.get());
	} else {
		if (Lorica_debug_level > 2) {
			ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T) %N:%l - can not map \"%C\" - no map\n"), typeId.c_str()));
		}
	}

	return rmv.release();
}

Lorica::ReferenceMapValue *
Lorica::ProxyMapper::add_native(CORBA::Object_ptr native,
				const std::string & typeId,
				bool out_facing,
				bool require_secure)
{
	// Are we handling this type id?
	if (this->evaluator_for (typeId) != 0) {
		ACE_UINT32 index = 0;

		switch (this->already_mapped(native, out_facing, index))
			{
			case ALREADY_MAPPED: // the "native" ref is really a mapped
				{
					if (Lorica_debug_level > 2) {
						ACE_DEBUG((LM_DEBUG,
							   ACE_TEXT("(%T) %N:%l - native is already mapped\n")));
					}

					Lorica::ReferenceMapValue_var rmv;
					if (!mapped_values_->find(index,rmv.out())) {
						ACE_ERROR((LM_ERROR,
							   ACE_TEXT("(%T) %N:%l - error, an already mapped value could not be found\n")));
					}
					
					return rmv.release();
				}
			case REVERSE_MAPPED:
				{
					if (Lorica_debug_level > 2) {
						ACE_DEBUG((LM_DEBUG,
							   ACE_TEXT("(%T) %N:%l - a reverse mapped reference is to be mapped\n")));
					}

					Lorica::ReferenceMapValue_var rmv;
					if (!mapped_values_->find(index,rmv.out())) {
						ACE_ERROR((LM_ERROR,
							   ACE_TEXT("(%T) %N:%l - error, an already mapped value could not be found\n")));
					}

					return rmv->reverse();
				}
			case NOT_MAPPED:
				{
					if (Lorica_debug_level > 2) {
						ACE_DEBUG((LM_DEBUG,
							   ACE_TEXT("(%T) %N:%l - native is not mapped\n")));
					}

					return this->add_native_unchecked(native,
									  typeId,
									  out_facing,
									  require_secure);
				}
			case MAPPER_ERROR:
				{
					return 0;
				}
			}
	} else if (this->next_ != 0) {
		return this->next_->add_native(native,
					       typeId,
					       out_facing,
					       require_secure);
	}

	return 0;
}

Lorica::ReferenceMapValue *
Lorica::ProxyMapper::remove_mapped(CORBA::Object_ptr mapped,
				   const std::string & typeId,
				   bool out_facing)
{
	// Are we handling this type id?
	if (this->evaluator_for (typeId) != 0) {
		if (this->mapped_values_ != 0) {
			PortableServer::POA_var p = out_facing ? this->out_facing_poa_ : this->in_facing_poa_;
			PortableServer::ObjectId_var oid = p->reference_to_id(mapped);
			Lorica::MappedObjectId *ptr = reinterpret_cast<Lorica::MappedObjectId *>(oid->get_buffer());

			Lorica::ReferenceMapValue_var rmv;
			this->mapped_values_->unbind(ptr->index, rmv.out());

			return rmv.release();
		}

		return 0;
	} else if (this->next_ != 0)
		return this->next_->remove_mapped(mapped, typeId, out_facing);

	return 0;
}


CORBA::Object_ptr
Lorica::ProxyMapper::current_native(Lorica::ServerAgent_ptr & agent)
{
	try {
		PortableServer::ObjectId_var oid = this->poa_current_->get_object_id();
		if (this->mapped_values_ == 0) {
			CORBA::String_var idstr = PortableServer::ObjectId_to_string(oid.in());
			CORBA::Object_var nref = orb_->string_to_object(idstr.in());

			// there is no agent since there is no mapped_values table.
			agent = 0;

			return nref._retn();
		} else
			return this->native_for_mapped_i (oid.inout(), agent);
	}
	catch (CORBA::Exception & ex) {
		// report failure
		char error_msg[256];

		ACE_OS::snprintf(error_msg, 256,
				 "ProxyMapper::current_native failed "
				 "with this exception %s", ex._name ());

		agent->error_occured(errno, error_msg);
		ACE_ERROR((LM_ERROR,
			   "(%T) %N:%l - failed with this exception: %s",
			   ex._name ()));
	}

	return CORBA::Object::_nil();
}

CORBA::Object_ptr
Lorica::ProxyMapper::native_for_mapped_i(const CORBA::OctetSeq & oid,
					 Lorica::ServerAgent_ptr & sa)
{
	const Lorica::MappedObjectId *mid = reinterpret_cast<const Lorica::MappedObjectId *>(oid.get_buffer());

	ReferenceMapValue_var rmv;
	if (!this->mapped_values_->find(mid->index,rmv.out())) {
		sa = 0;

		return CORBA::Object::_nil();
	}

	sa = Lorica::ServerAgent::_duplicate(rmv->agent_);

	return CORBA::Object::_duplicate(rmv->orig_ref_);
}

bool
Lorica::ProxyMapper::native_for_mapped(CORBA::Object_ptr & ref,
				       PortableServer::POA_ptr poa)
{
	Lorica::ServerAgent_var agent;
	try {
		PortableServer::ObjectId_var oid = poa->reference_to_id(ref);
		CORBA::Object_var nref;
		if (this->mapped_values_ == 0) {
			CORBA::String_var idstr = PortableServer::ObjectId_to_string(oid.in());
			nref = orb_->string_to_object(idstr.in());
		} else {
			nref = this->native_for_mapped_i(oid.in(), agent.out());

			if (CORBA::is_nil(nref.in()))
				return false;
		}

		CORBA::release(ref);
		ref = nref._retn();

		return true;
	}
	catch (CORBA::Exception & ex) {
		// report failure
		char error_msg[256];

		ACE_OS::snprintf(error_msg, 256,
				 "ProxyMapper::native_for_mapped failed "
				 "with this exception %s", ex._name());

		agent->error_occured (errno, error_msg);
		ACE_ERROR((LM_ERROR,
			   "(%T) %N:%l - failed with this exception: %s",
			   ex._name()));
	}

	return false;
}

bool
Lorica::ProxyMapper::poa_and_object_id(CORBA::Object_ptr mapped_ref,
				       CORBA::OctetSeq *& poaid,
				       PortableServer::ObjectId *& objid)
{
	// Are we handling this type id?
	if (this->evaluator_for (mapped_ref) == 0) {
		if (this->next_ != 0)
			return this->next_->poa_and_object_id(mapped_ref, poaid, objid);

		poaid = new CORBA::OctetSeq;
		objid = new PortableServer::ObjectId;

		return false;
	}
	try {
		poaid = this->out_facing_poa_->id();
		objid = this->out_facing_poa_->reference_to_id(mapped_ref);
	}
	catch (CORBA::Exception & ex) {
		ACE_ERROR((LM_ERROR,
			   "(%T) %N:%l - caught %s\n",
			   ex._name()));

		objid = new PortableServer::ObjectId;

		return false;
	}

	return true;
}


bool
Lorica::ProxyMapper::mapped_for_native(CORBA::Object_ptr & ref,
				       PortableServer::POA_ptr poa)
{
	if (CORBA::is_nil(ref))
		return true;

	bool want_outfacing = (poa == in_facing_poa_.in());

	ReferenceMapValue_var rmv = this->registry_.map_reference(ref,
								  want_outfacing);

	if (rmv.get() != 0) {
		CORBA::release(ref);
		ref = CORBA::Object::_duplicate(rmv->mapped_ref_.in());

		return true;
	} else {
		if (Lorica_debug_level > 0)
			ACE_ERROR((LM_ERROR,
				   "(%T) %N:%l - must add native reference\n"));
	}

	return false;
}

Lorica::EvaluatorBase *
Lorica::ProxyMapper::evaluator_for(CORBA::Object_ptr native)
{
	if (CORBA::is_nil(native))
		return 0;

	std::string typeId = native->_stubobj()->type_id.in();

	return this->evaluator_for(typeId);
}

Lorica::EvaluatorBase *
Lorica::ProxyMapper::evaluator_for(const std::string & typeId)
{
	if (this->evaluator_head_ != 0)
		return this->evaluator_head_->find_evaluator(typeId);

	return 0;
}

PortableServer::POA_ptr
Lorica::ProxyMapper::other_POA(PortableServer::POA_ptr p)
{
	return (p == this->in_facing_poa_.in())
		? PortableServer::POA::_duplicate(this->out_facing_poa_.in())
		: PortableServer::POA::_duplicate (this->in_facing_poa_.in());
}

void
Lorica::ProxyMapper::incr_refcount(void)
{
	++this->ref_count_;
}

long
Lorica::ProxyMapper::decr_refcount(void)
{
	if (--this->ref_count_ > 0)
		return this->ref_count_;

	ACE_ASSERT(this->ref_count_ == 0);

	delete this;

	return 0;
}
