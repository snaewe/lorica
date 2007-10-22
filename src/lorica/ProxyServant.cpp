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

#include "tao/DynamicInterface/Request.h"
#include "tao/TAO_Server_Request.h"

#include "lorica/ProxyMapper.h"
#include "lorica/EvaluatorBase.h"
#include "lorica/ProxyReplyHandler.h"
#include "lorica/ProxyServant.h"
#include "lorica/debug.h"

Lorica::ProxyServant::ProxyServant (ProxyMapper &pm)
  : mapper_(pm)
{
  this->orb_ = mapper_.orb();
  CORBA::Object_var obj = orb_->resolve_initial_references("POACurrent");
  this->current_ = PortableServer::Current::_narrow(obj.in());
}

/// The code that actually processes the request message
void
Lorica::ProxyServant::invoke_i (CORBA::ServerRequest_ptr request,
        TAO_AMH_DSI_Response_Handler_ptr response_handler)
{
  Lorica::ServerAgent_var agent;
  CORBA::Object_var target_obj = mapper_.current_native(agent.out());
  EvaluatorBase *evb = this->mapper_.evaluator_for (target_obj.in());
  if (evb == 0)
  {
    throw CORBA::NO_IMPLEMENT();
  }

  bool is_oneway = !request->_tao_server_request().response_expected() ||
    request->_tao_server_request().sync_with_server ();

  CORBA::NVList_var args;

  CORBA::NVList_var out_args;
  CORBA::NamedValue_var result;
  this->orb_->create_list (0, args);
  this->orb_->create_list (0, out_args);
  PortableServer::POA_var poa = this->current_->get_POA();
  PortableServer::ObjectId_var oid = this->current_->get_object_id();

  try
  {
    if (! evb->evaluate_request (request->operation(),
				 poa.in(),
				 request,
				 args.inout(),
				 out_args.inout(),
				 result.out()))
    {
      if (!CORBA::is_nil (agent))
        agent->error_occured (errno,
                  "ProxyServant::invoke_i, evaluate_request "
                  "failed, Caught CORBA::BAD_OPERATION()\n");

      if (Lorica_debug_level > 0)
        ACE_ERROR((LM_ERROR,
             "(%P|%t) Lorica::ProxyServant::invoke_i evaluate_"
             "request failed. Caught CORBA::BAD_OPERATION()\n"));

      throw CORBA::BAD_OPERATION();
    }
  }
  catch (CORBA::UserException &ex)
  {
    if (Lorica_debug_level > 0)
      ex._tao_print_exception ("evaluate_request");
    if (!CORBA::is_nil (agent))
      agent->error_occured (errno,
                "ProxyServant::invoke_i, evaluate_request "
                "failed, A CORBA::BAD_OPERATION() is thrown");

    if (Lorica_debug_level > 0)
      ACE_ERROR ((LM_ERROR, "(%P|%t) Lorica::ProxyServant::invoke_i "
            "evaluate_request raised %s\n",
            ex._name()));

    throw CORBA::BAD_OPERATION();
  }

  // do the rest, ie set up reply handler

  CORBA::Request_var target_request;

  target_obj->_create_request (0, // context
			       request->operation(),
			       args._retn(), // hand off ownership
			       0, // result,
			       0, // exception list
			       0, // context_list
			       target_request.inout(),
			       0);

  if (is_oneway)
  {
    target_request->send_oneway();
    return;
  }

  Messaging::ReplyHandler_var rh =
    new Lorica::ProxyReplyHandler(this->mapper_,
				  request->operation(),
				  poa.in(),
				  evb,
				  out_args._retn(),
				  result._retn(),
				  response_handler);

  target_request->sendc(rh.in());
}
