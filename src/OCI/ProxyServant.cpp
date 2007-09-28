/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 *  This file and all software in this directory and any subdirectories
 *  (henceforth  referred to as "OCI software") is copyrighted
 *  by [1]Object Computing, Inc (OCI), Copyright (C) 2007, all rights
 *  reserved.
 *
 *  Rights Granted By This License
 *
 *  This software is open-source, freely available software. You are
 *  free to use, modify, copy, and distribute -- perpetually and
 *  irrevocably -- the source code in this directory and any object
 *  code produced from this source, as well as copy and distribute
 *  modified versions of this software. You must, however, include this
 *  copyright statement along with any code built using OCI software
 *  that you release. No copyright statement need be provided if you
 *  just ship binary executables of your software products.
 *
 *  You may use OCI software in commercial and/or binary software
 *  releases and are under no obligation to redistribute any of your
 *  source code that is built using OCI software. Note, however, that
 *  you may not do anything to the OCI software code, such as
 *  copyrighting it yourself or claiming authorship of the OCI software
 *  code, that will prevent OCI software from being distributed freely
 *  using an open-source development model. You needn't inform anyone
 *  that you are using OCI software in your software, though we
 *  encourage you to let us [2]know about it.
 *
 *  You are welcome to contribute bug fixes and enhancements to this
 *  [3]software. Contributions, if accepted by OCI or its delegate and
 *  incorporated into this software, will be subject to the same
 *  copyright as the rest of this software. The contributor
 *  relinquishes all other rights and claims to the contribution.
 *  Contributions will be attributed in the ChangeLog.
 *
 *  OCI software is provided as is with no warranties of any kind,
 *  including the warranties of design, merchantability, and fitness
 *  for a particular purpose, noninfringement, or arising from a course
 *  of dealing, usage or trade practice. Object Computing, Inc., and
 *  its employees shall have no liability with respect to the
 *  infringement of copyrights, trade secrets or any patents by OCI
 *  software or any part thereof. Moreover, in no event will Object
 *  Computing, Inc. or its employees be liable for any lost revenue or
 *  profits or other special, indirect and consequential damages.
 *
 *  OCI software is provided with commercial support, available directly
 *  from Object Computing, Inc.
 *
 *   ------------------------------------------------------------------
 *
 *  References
 *
 *  1. http://www.ociweb.com
 *  2. mailto:sales@ociweb.com
 *  3. mailto:support@ociweb.com
 */

#include "OCI/ProxyServant.h"
#include "tao/TAO_Server_Request.h"
#include "tao/DynamicInterface/Server_Request.h"
#include "tao/DynamicInterface/AMH_DSI_Response_Handler.h"
#include "tao/Messaging/AMH_Response_Handler.h"
#include "tao/ORB_Core.h"
#include "tao/Thread_Lane_Resources.h"
#include "tao/Exception.h"

OCI_APT::ProxyServant::ProxyServant ()
{
}

OCI_APT::ProxyServant::~ProxyServant ()
{
}

void
OCI_APT::ProxyServant::_dispatch (TAO_ServerRequest &request,
				  void * //context
	)
{
	// No need to do any of this if the client isn't waiting.
	if (request.response_expected ())
	{
		if (!CORBA::is_nil (request.forward_location ()))
		{
			request.init_reply ();
			request.tao_send_reply ();

			// No need to invoke in this case.
			return;
		}
		else if (request.sync_with_server ())
		{
			// The last line before the call to this function
			// was an ACE_CHECK_RETURN, so if we're here, we
			// know there is no exception so far, and that's all
			// a SYNC_WITH_SERVER client request cares about.
			request.send_no_exception_reply ();
		}
	}

	// Create DSI request object.
	CORBA::ServerRequest *dsi_request = 0;
	ACE_NEW (dsi_request,
		 CORBA::ServerRequest (request));
	try
	{
		TAO_AMH_DSI_Response_Handler_ptr rh_ptr = 0;
		ACE_NEW (rh_ptr, TAO_AMH_DSI_Response_Handler(request));

		TAO_AMH_DSI_Response_Handler_var rh = rh_ptr;

		// init the handler
		TAO_AMH_BUFFER_ALLOCATOR* amh_allocator =
			request.orb()->orb_core ()->lane_resources().
			amh_response_handler_allocator();
		rh->init (request, amh_allocator);
		// Delegate to user.
		this->invoke (dsi_request,
			      rh.in());
	}
	catch (CORBA::Exception& ex)
	{
		// Only if the client is waiting.
		if (request.response_expected () && !request.sync_with_server ())
		{
			request.tao_send_reply_exception (ex);
		}
	}
	CORBA::release (dsi_request);
}

void
OCI_APT::ProxyServant::invoke(CORBA::ServerRequest_ptr )
	throw (CORBA::SystemException)
{
	// unused method, only asynch invocations are used
}

char *
OCI_APT::ProxyServant::_primary_interface(const PortableServer::ObjectId&,
					  PortableServer::POA_ptr)
	throw (CORBA::SystemException)
{
	return CORBA::string_dup ("IDL:Object:1.0");
}

void
OCI_APT::ProxyServant::invoke(CORBA::ServerRequest_ptr request,
			      TAO_AMH_DSI_Response_Handler_ptr rh)
	throw (CORBA::SystemException)
{
	if ( ACE_OS::strcmp ("_get_interface", request->operation ()) == 0)
		this->invoke_get_interface (request);
	else if ( ACE_OS::strcmp ("_primary_interface", request->operation ()) == 0)
		this->invoke_primary_interface (request);
	else
		this->invoke_i (request, rh);
}

void
OCI_APT::ProxyServant::invoke_get_interface (CORBA::ServerRequest_ptr request)
	throw (CORBA::SystemException)
{
#if 0
	CORBA::NVList_ptr args;
	this->orb_->create_list (0, args );
	request->arguments (args );

	CORBA::InterfaceDef_var theIntDef = this->_get_interface ();
	CORBA::Any theResult;
	theResult.type (CORBA::_tc_Object);
	theResult <<= theIntDef.in ();
	request->set_result (theResult);
#endif
}

void
OCI_APT::ProxyServant::invoke_primary_interface (CORBA::ServerRequest_ptr request)
	throw (CORBA::SystemException)
{
#if 0
	CORBA::NVList_ptr args;
	this->orb_->create_list (0, args);
	request->arguments (args);
	CORBA::String_var rep_id =
		this->interface_repository_id();

	CORBA::Any a;
	a.type(CORBA::_tc_string);
	a <<= rep_id.in ();
	request->set_result (a);
#endif
}
