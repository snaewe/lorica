/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: t; c-basic-offset: 2 -*- */

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

#include "defines/build-system-defs.h"

#include <tao/ORB_Core.h>

#include "GenericMapper.h"
#include "GenericEvaluator.h"
#include "EvaluatorBase.h"
#include "ProxyServant.h"

#ifdef LORICA_STATIC
#include <tao/IFR_Client/IFR_Client_Adapter_Impl.h>

// This is necessary to ensure that the required service object
// is initialized when statically linked.
ACE_STATIC_SVC_REQUIRE (TAO_IFR_Client_Adapter_Impl)

#endif /* LORICA_STATIC */

Lorica::GenericMapper::GenericMapper(const bool Debug,
																		 Lorica_MapperRegistry & mr)
: Lorica::ProxyMapper(mr, "_lorica_generic"),
	typeIdList (),
	optable_ (0),
  ifr_client_ (0),
	debug_ (Debug)
{
}

Lorica::GenericMapper::~GenericMapper(void)
{
	delete optable_;
  ifr_.fini();
}

int
Lorica::GenericMapper::proxy_mapper_init(PortableServer::POAManager_ptr outward,
					 PortableServer::POAManager_ptr inward,
					 CORBA::ORB_ptr orb)
{
	this->orb_ = CORBA::ORB::_duplicate(orb);
	CORBA::Object_var obj = orb->resolve_initial_references("DynAnyFactory");

	dynAnyFact_ = DynamicAny::DynAnyFactory::_narrow(obj.in());
	obj = orb->resolve_initial_references("POACurrent");
	poa_current_ = PortableServer::Current::_narrow(obj.in());

	this->optable_ = new OCI_APT::OperationTable();

	int result = this->Lorica::ProxyMapper::proxy_mapper_init(outward, inward, orb);

	// Collocate the IFR Service.
	ifr_.init(debug_, orb, this->in_facing_poa_);

	ifr_client_ = ACE_Dynamic_Service<TAO_IFR_Client_Adapter>::instance(TAO_ORB_Core::ifr_client_adapter_name());

	if (ifr_client_ == 0)
		throw ::CORBA::INTF_REPOS();

	return result;
}

OCI_APT::ArgList*
Lorica::GenericMapper::add_operation(const std::string & typeId,
				     const char *operation)
{
	CORBA::InterfaceDef_var intDef = this->ifr_client_->get_interface(this->orb_, typeId.c_str());

	if (CORBA::is_nil(intDef.in())) {
		ACE_ERROR((LM_ERROR,
			   ACE_TEXT("%N:%l - get_interface returned nil\n")));

		throw CORBA::BAD_OPERATION(CORBA::OMGVMCID | 2,
					   CORBA::COMPLETED_NO);
	}

	return this->optable_->add_interface(intDef.in(), operation);
}

Lorica::EvaluatorBase*
Lorica::GenericMapper::evaluator_for(const std::string & typeId)
{
	Lorica::EvaluatorBase *eval = 0;

	if (this->evaluator_head_ != 0)
		eval = this->evaluator_head_->find_evaluator(typeId);

	if (eval == 0) {
		try {
			CORBA::InterfaceDef_var intDef = this->ifr_client_->get_interface(this->orb_, typeId.c_str());

			if (!CORBA::is_nil(intDef.in())) {
				eval = new Lorica::GenericEvaluator(typeId,
								    intDef.in(),
								    optable_,
								    dynAnyFact_.in(),
								    *this);
				this->add_evaluator(eval);
			}
		}
		catch (CORBA::Exception &ex) {
			ACE_DEBUG((LM_ERROR, ACE_TEXT("(%T) %N:%l - %s\n"), ex._info().c_str()));
		}
	}

	return eval;
}

Lorica::ProxyServant*
Lorica::GenericMapper::make_default_servant(void)
{
	return new Lorica::ProxyServant(*this);
}
