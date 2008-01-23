/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 *    Lorica header file. 
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

#ifndef GENERIC_EVALUATOR_H
#define GENERIC_EVALUATOR_H

#include "EvaluatorBase.h"
#include "OCI/OperationTable.h"
#include "tao/DynamicAny/DynAnyFactory.h"
#include "tao/ORB.h"

#include "Lorica_GenericEvaluator_Export.h"

namespace Lorica
{
class Lorica_GenericEvaluator_Export GenericEvaluator : public EvaluatorBase
	{
	public:
		GenericEvaluator (const std::string &id,
				  CORBA::InterfaceDef_ptr,
				  OCI_APT::OperationTable*,
				  DynamicAny::DynAnyFactory_ptr,
				  ProxyMapper &pm);

		virtual ~GenericEvaluator ();

		bool evaluate_request (const char * operation,
				       PortableServer::POA_ptr req_poa,
				       CORBA::ServerRequest_ptr request,
				       CORBA::NVList_ptr args,
				       CORBA::NVList_ptr &out_args,
				       CORBA::NamedValue_ptr &result) const;

		bool evaluate_reply (const char *operation,
				     PortableServer::POA_ptr req_poa,
				     CORBA::NVList_ptr args,
				     CORBA::NamedValue_ptr result) const;

		virtual bool evaluate_exception (const char *operation,
						 PortableServer::POA_ptr req_poa,
						 const char * ex_type,
						 TAO_InputCDR &,
						 TAO_OutputCDR &) const;


	private:
		CORBA::InterfaceDef_var intDef_;
		mutable OCI_APT::OperationTable* optable_;
		DynamicAny::DynAnyFactory_var dynAnyFact_;

		bool has_objref (CORBA::TypeCode_ptr tc) const;

		void proxify_params (PortableServer::POA_ptr req_poa,
				     CORBA::NVList_ptr params,
				     OCI_APT::ArgList * arglist,
				     CORBA::Flags direction) const;

		void proxify_result (PortableServer::POA_ptr req_poa,
				     CORBA::NamedValue_ptr nv,
				     OCI_APT::ArgList *arg_list) const;

		void proxify_exception (PortableServer::POA_ptr req_poa,
					CORBA::Any &value,
					OCI_APT::Arg *exception) const;

		void proxify_any (PortableServer::POA_ptr req_poa,
				  CORBA::Any &any) const;

		bool proxify (PortableServer::POA_ptr,
			      DynamicAny::DynAny_ptr &theDynAny) const;
	};
}

#endif // GENERIC_EVALUATOR_H
