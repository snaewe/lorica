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

#include <tao/ORB_Core.h>

#include "GenericMapper.h"
#include "GenericEvaluator.h"
#include "EvaluatorBase.h"
#include "ProxyServant.h"

Lorica::GenericMapper::GenericMapper(const bool Debug,
				     Lorica_MapperRegistry & mr)
	: Lorica::ProxyMapper(mr, "_lorica_generic"),
	  typeIdList(),
	  debug_(Debug)
{
}

Lorica::GenericMapper::~GenericMapper(void)
{
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

	// Collocate the IFR Service.
	ifr_.init(debug_, orb);

	ifr_client_ = ACE_Dynamic_Service<TAO_IFR_Client_Adapter>::instance(TAO_ORB_Core::ifr_client_adapter_name());

	if (ifr_client_ == 0)
		throw ::CORBA::INTF_REPOS();

	return this->Lorica::ProxyMapper::proxy_mapper_init(outward, inward, orb);
}

OCI_APT::ArgList*
Lorica::GenericMapper::add_operation(const std::string & typeId,
				     const char *operation)
{
	CORBA::InterfaceDef_var intDef = this->ifr_client_->get_interface(this->orb_, typeId.c_str());

	if (CORBA::is_nil(intDef.in())) {
		ACE_ERROR((LM_ERROR,
			   ACE_TEXT ("[Lorica::ProxyServantArgList] ")
			   ACE_TEXT ("_get_interface returned nil\n")));

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
			ex._tao_print_exception ("get_interface exception:");
		}
	}

	return eval;
}

Lorica::ProxyServant*
Lorica::GenericMapper::make_default_servant(void)
{
	return new Lorica::ProxyServant(*this);
}
