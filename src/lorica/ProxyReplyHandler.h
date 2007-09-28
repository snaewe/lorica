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

#ifndef LORICA_PROXY_REPLY_HANDLER_H
#define LORICA_PROXY_REPLY_HANDLER_H

#include "OCI/ProxyReplyHandler.h"
#include "tao/PortableServer/POAC.h"
#include "tao/AnyTypeCode/NVList.h"
#include <string>

namespace Lorica
{
	class ProxyMapper;
	class EvaluatorBase;

class ProxyReplyHandler : public OCI_APT::ProxyReplyHandler
	{
	public:
		ProxyReplyHandler (const ProxyMapper &pm,
				   const char *operation,
				   PortableServer::POA_ptr poa,
				   const Lorica::EvaluatorBase *eval,
				   CORBA::NVList_ptr out_args,
				   CORBA::NamedValue_ptr result,
				   TAO_AMH_DSI_Response_Handler_ptr resp);

		~ProxyReplyHandler(void);

		/// The code that actually processes the request message
		void handle_response_i (TAO_InputCDR &incoming);
		void handle_excep_i (TAO_InputCDR &incoming,
				     CORBA::ULong reply_status);

	private:
		/// The Proxy Mapper associated with this servant
		const ProxyMapper &mapper_;
		std::string operation_;
		PortableServer::POA_var poa_;
		const EvaluatorBase *evaluator_;
		CORBA::NVList_var out_args_;
		CORBA::NamedValue_var result_;
		TAO_AMH_DSI_Response_Handler_var response_handler_;
	};
}

#endif // LORICA_PROXY_REPLY_HANDLER_H
