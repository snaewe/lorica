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

#ifndef TAO_PROXY_SERVANT
#define TAO_PROXY_SERVANT

#include "tao/DynamicInterface/Dynamic_Implementation.h"
#include "OCI/OCI_AsynchProxyTools_Export.h"
#include "tao/SystemException.h"

class TAO_AMH_DSI_Response_Handler;
typedef TAO_AMH_DSI_Response_Handler * TAO_AMH_DSI_Response_Handler_ptr;

namespace OCI_APT
{
	class OCI_AsynchProxyTools_Export ProxyServant :
		public virtual TAO_DynamicImplementation
	{
	public:
		ProxyServant();
		virtual ~ProxyServant();

		virtual void _dispatch(TAO_ServerRequest& request, void *context);

		void invoke (CORBA::ServerRequest_ptr request)
			throw (CORBA::SystemException);

		char * _primary_interface(const PortableServer::ObjectId&,
					  PortableServer::POA_ptr)
			throw (CORBA::SystemException);

		void invoke (CORBA::ServerRequest_ptr request,
			     TAO_AMH_DSI_Response_Handler_ptr rh)
			throw (CORBA::SystemException);

		void invoke_get_interface (CORBA::ServerRequest_ptr request)
			throw (CORBA::SystemException);

		void invoke_primary_interface (CORBA::ServerRequest_ptr request)
			throw (CORBA::SystemException);

		virtual void invoke_i (CORBA::ServerRequest_ptr request,
				       TAO_AMH_DSI_Response_Handler_ptr response_handler
			) = 0;


	};

}

#endif // TAO_PROXY_SERVANT
