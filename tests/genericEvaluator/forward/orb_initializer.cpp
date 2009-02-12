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

#include "orb_initializer.h"
#include "server_interceptor.h"

Server_ORBInitializer::Server_ORBInitializer (void)
{
}

void
Server_ORBInitializer::pre_init (
    PortableInterceptor::ORBInitInfo_ptr)
{
        // No Op
}

void
Server_ORBInitializer::post_init (
    PortableInterceptor::ORBInitInfo_ptr info)
{
  // Install the Echo server request interceptor
  ACE_NEW_THROW_EX (this->server_interceptor_,
                    ForwardTest_Request_Interceptor,
                    CORBA::NO_MEMORY ());

  PortableInterceptor::ServerRequestInterceptor_var interceptor =
    this->server_interceptor_;

  info->add_server_request_interceptor (interceptor.in ());
}

ForwardTest_Request_Interceptor *
Server_ORBInitializer::server_interceptor (void)
{
  return this->server_interceptor_;
}
