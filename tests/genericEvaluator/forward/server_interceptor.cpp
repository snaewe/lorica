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

#include "server_interceptor.h"
#include <tao/OctetSeqC.h>
#include <ace/Log_Msg.h>
#include <tao/ORB_Constants.h>
#include <ace/OS_NS_string.h>
#include <ace/Thread.h>

ForwardTest_Request_Interceptor::ForwardTest_Request_Interceptor (void)
  : myname_ ("ForwardTest_Interceptor")
{
}

ForwardTest_Request_Interceptor::ForwardTest_Request_Interceptor (CORBA::Object_ptr forward_location)
  : myname_ ("ForwardTest_Interceptor"),
    forward_location_ (CORBA::Object::_duplicate (forward_location)),
    forward_location_done_ (false)
{
}

ForwardTest_Request_Interceptor::~ForwardTest_Request_Interceptor (void)
{
}

void
ForwardTest_Request_Interceptor::forward_reference (CORBA::Object_ptr forward_location)
{
  if (CORBA::is_nil (forward_location))
    throw CORBA::INV_OBJREF (
      CORBA::SystemException::_tao_minor_code (
        TAO::VMCID,
        EINVAL),
      CORBA::COMPLETED_NO);
  this->forward_location_ = CORBA::Object::_duplicate (forward_location);
}

char *
ForwardTest_Request_Interceptor::name (void)
{
  return CORBA::string_dup (this->myname_);
}

void
ForwardTest_Request_Interceptor::destroy (void)
{
}

void
ForwardTest_Request_Interceptor::receive_request_service_contexts (
    PortableInterceptor::ServerRequestInfo_ptr ri)
{

  CORBA::String_var operation = ri->operation ();

  ACE_DEBUG ((LM_DEBUG,
              "%s.receive_request_service_contexts from "
              "\"%s\"\n",
              this->myname_,
              operation.in ()));

  // Ignore the "_is_a" operation since it may have been invoked
  // locally on the server side as a side effect of another call,
  // meaning that the client hasn't added the service context yet.
  // Same goes for the shutdown call
  if (ACE_OS_String::strcmp ("_is_a", operation.in ()) == 0 ||
      ACE_OS_String::strcmp ("shutdown", operation.in ()) == 0)
    return;

  if (!forward_location_done_)
    {
      forward_location_done_ = true;

      ACE_DEBUG ((LM_DEBUG,
                  "Sending LOCATION_FORWARD, current thread %i\n",
                  ACE_Thread::self ()));

      throw PortableInterceptor::ForwardRequest (this->forward_location_);
    }
}


void
ForwardTest_Request_Interceptor::receive_request (
    PortableInterceptor::ServerRequestInfo_ptr)
{
  // Do nothing
}

void
ForwardTest_Request_Interceptor::send_reply (
    PortableInterceptor::ServerRequestInfo_ptr ri)
{
    // No op
     ACE_UNUSED_ARG (ri);
}

void
ForwardTest_Request_Interceptor::send_exception (
    PortableInterceptor::ServerRequestInfo_ptr ri)
{
    // No Op
     ACE_UNUSED_ARG (ri);
}

void
ForwardTest_Request_Interceptor::send_other (
             PortableInterceptor::ServerRequestInfo_ptr ri)
{

  // This will throw an exception if a location forward has not
  // occured.  If an exception is thrown then something is wrong with
  // the PortableInterceptor::ForwardRequest support.
  CORBA::Object_var forward = ri->forward_reference ();

  if (CORBA::is_nil (forward.in ()))
    throw CORBA::INTERNAL ();
}

bool
ForwardTest_Request_Interceptor::forward_location_done() const
{
  return forward_location_done_;
}
