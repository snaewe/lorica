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

//**************************************************************************
//
// BASED ON ACE SAMPLE CODE BY:
//    ACE_wrappers/examples/NT_Service/ntsvc.cpp
//
//**************************************************************************

#include "ntsvc.h"
#include "proxy.h"

#if defined (ACE_WIN32)

Lorica::AutoFinalizer::AutoFinalizer (Lorica::NT_Service &service)
	: service_ (service)
{
}

Lorica::AutoFinalizer::~AutoFinalizer ()
{
	service_.report_status (SERVICE_STOPPED);
}

Lorica::NT_Service::NT_Service (void)
	: proxy_ (0)
{
}

Lorica::NT_Service::~NT_Service (void)
{
	delete proxy_;
}

// This method is called when the service gets a control request.  It
// handles requests for stop and shutdown by calling terminate ().
// All others get handled by calling up to inherited::handle_control.

void
Lorica::NT_Service::handle_control (DWORD control_code)
{
	if (control_code == SERVICE_CONTROL_SHUTDOWN
	    || control_code == SERVICE_CONTROL_STOP)
	{
		report_status (SERVICE_STOP_PENDING);

		ACE_DEBUG ((LM_INFO,
			    ACE_TEXT ("Service control stop requested\n")));
		this->proxy_->shutdown ();
	}
	else
		inherited::handle_control (control_code);
}

// This is just here to be the target of the notify from above... it
// doesn't do anything except aid on popping the reactor off its wait
// and causing a drop out of handle_events.

int
Lorica::NT_Service::handle_exception (ACE_HANDLE)
{
	return 0;
}

int
Lorica::NT_Service::handle_timeout (const ACE_Time_Value &tv,
				    const void *)
{
	ACE_UNUSED_ARG (tv);
	MessageBeep (MB_OK);
	return 0;
}

void
Lorica::NT_Service::proxy (Proxy *p)
{
	this->proxy_ = p;
}

// This is the main processing function for the Service.  It sets up
// the initial configuration and runs the event loop until a shutdown
// request is received.

int
Lorica::NT_Service::svc (void)
{
	try
	{
		AutoFinalizer af (*this);
		report_status (SERVICE_RUNNING);
		this->proxy_->svc();
	}
	catch (const CORBA::Exception& ex)
	{
		ex._tao_print_exception ("TAO NT Naming Service");
		return -1;
	}

	return 0;
}
#endif /* ACE_WIN32 */
