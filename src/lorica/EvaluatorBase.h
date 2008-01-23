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

#ifndef EVALUATOR_BASE_H
#define EVALUATOR_BASE_H

#include <string>
#include <tao/AnyTypeCode/NVList.h>
#include <tao/PortableServer/POAC.h>
#include <tao/DynamicInterface/Server_Request.h>

#include "Lorica_EvaluatorBase_Export.h"

class ACE_Message_Block;

namespace Lorica
{

	class ProxyMapper;
	class ProxyServant;

	class Lorica_EvaluatorBase_Export EvaluatorBase
	{
	public:
		EvaluatorBase (ProxyMapper &pm);

		EvaluatorBase (const std::string &typeId,
			       ProxyMapper &pm);
		virtual ~EvaluatorBase (void);

		/// multiple evaluators may be associated with a single mapper as
		/// a linked list. This way a single evaluator is responsible for
		/// a single type.

		/// Add a new evaluator to the list
		void add_evaluator (EvaluatorBase *eb);

		/// Find the appropriate evaluator for the given type. Might be
		/// this one.  Recursively calls down the change, returns null if
		/// no evaluators are found for the type.
		EvaluatorBase *find_evaluator (const std::string &typeId);

		/// Returns the typeId for this evaluator.
		const std::string & type_id (void) const;

		/// The subclassible parts of the evaluator.

		/// Evaluate_request takes an operation name string as an in, and
		/// a NVList reference as an in-out. The NVList represents any in
		/// or inout arguments associated with the operation.  As supplied
		/// by the ProxyServant, this list is un-preprocessed, ie it
		/// contains only the CDR Input Stream containing the payload
		/// portion of the request message and a zero-length sequence of
		/// NamedValues as the argument list.  By default, this method is
		/// a no-op, simply returning the unmodified argument
		/// list. Subclassers may, based on the operation name, extract
		/// the arguments any way they wish to validate the data contained
		/// within or map object references with the assistance of the
		/// associated ProxyMapper. Returns true if the arguments mapped
		/// correctly (and further processing is allowed), or false if the
		/// request should not be forwarded to the target object.
		virtual bool evaluate_request (const char * operation,
					       PortableServer::POA_ptr req_poa,
					       CORBA::ServerRequest_ptr request,
					       CORBA::NVList_ptr args,
					       CORBA::NVList_ptr &out_args,
					       CORBA::NamedValue_ptr &result) const;

		/// Evaluate_reply takes the operation name, inout or out arguments
		/// and the operation result. As with Evaluate_request, it is up to
		/// the subclasser to extract the argument values as needed, based
		/// on the operation.
		virtual bool evaluate_reply (const char *operation,
					     PortableServer::POA_ptr req_poa,
					     CORBA::NVList_ptr args,
					     CORBA::NamedValue_ptr result) const;

		/// Evaluate_exception takes the opration name and the exception
		/// payload. Only user exceptions are evaluated, the system exceptions
		/// are handled by the caller.
		virtual bool evaluate_exception (const char *oprartion,
						 PortableServer::POA_ptr req_poa,
						 const char * ex_type,
						 TAO_InputCDR &incoming,
						 TAO_OutputCDR &encap) const;

	protected:
		/// The mapper for this evaluator
		mutable ProxyMapper &mapper_;

	private:
		/// The local copy of the ID
		std::string id_;

		/// The next evaluator in the list.
		EvaluatorBase * next_;

	};
}


#endif // EVALUATOR_BASE_H
