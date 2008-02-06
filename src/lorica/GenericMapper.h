/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 *    Lorica header file.
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

#ifndef GENERIC_MAPPER_H
#define GENERIC_MAPPER_H

#include <string>
#include <vector>
#include <tao/DynamicAny/DynAnyFactory.h>
#include <tao/IFR_Client_Adapter.h>

#include "ProxyMapper.h"
#include "IFRService.h"
#include "Lorica_GenericEvaluator_Export.h"

namespace OCI_APT
{
	class OperationTable;
	class ArgList;
}

namespace Lorica
{
	class EvaluatorBase;

	class Lorica_GenericEvaluator_Export GenericMapper : public ProxyMapper {
	public:
		GenericMapper(const bool Debug,
			      Lorica_MapperRegistry & mr);

		virtual ~GenericMapper(void);

		// Initializer specific to proxy mappers. Called by the main during
		// program initialization.
		virtual int proxy_mapper_init(PortableServer::POAManager_ptr outward,
					      PortableServer::POAManager_ptr inward,
					      CORBA::ORB_ptr orb);

		// Returns the evaluator associated with a given type ID. Returns null
		// if the type is unknown.
		virtual EvaluatorBase *evaluator_for(const std::string & typeId);

		OCI_APT::ArgList *add_operation(const std::string & typeId,
						const char *operation);

	protected:
		virtual ProxyServant *make_default_servant(void);

	private:
		// a linked list of evaluators known by this mapper
		typedef std::vector<std::string> ListType;
		ListType typeIdList;

		OCI_APT::OperationTable *optable_;
		CORBA::ORB_var orb_;
		DynamicAny::DynAnyFactory_var dynAnyFact_;
		Lorica::IFRService ifr_;
		TAO_IFR_Client_Adapter *ifr_client_;
		bool debug_;
	};
}

#endif // GENERIC_MAPPER_H
