/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

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

#ifndef LORICA_PROXY_SERVANT_H
#define LORICA_PROXY_SERVANT_H

#include <string>
#include <tao/PortableServer/PS_CurrentC.h>

#include "OCI/ProxyServant.h"
#include "Lorica_EvaluatorBase_Export.h"

namespace Lorica
{
	class ProxyMapper;

	class Lorica_EvaluatorBase_Export ProxyServant : public OCI_APT::ProxyServant {
	public:
		ProxyServant(ProxyMapper & pm);

		/// The code that actually processes the request message
		void invoke_i(CORBA::ServerRequest_ptr request,
			      TAO_AMH_DSI_Response_Handler_ptr response_handler);

	protected:
		/// The Proxy Mapper associated with this servant
		ProxyMapper & mapper_;

		CORBA::ORB_var orb_;
		PortableServer::Current_var current_;
	};
}

#endif // LORICA_PROXY_SERVANT_H
