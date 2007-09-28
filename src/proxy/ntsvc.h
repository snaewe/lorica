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

//**************************************************************************
//
// BASED ON ACE SAMPLE CODE BY:
//    ACE_wrappers/examples/NT_Service/ntsvc.h
//
//**************************************************************************

#ifndef NTSVC_H_
#define NTSVC_H_

#include "ace/config-all.h"

#if defined (ACE_WIN32)
#include "ace/Event_Handler.h"
#include "ace/NT_Service.h"
#include "ace/Singleton.h"
#include "ace/Synch.h"

namespace Lorica
{
	class Proxy;
	class AutoFinalizer;

	class NT_Service : public ACE_NT_Service
	{
		friend AutoFinalizer;

	public:
		NT_Service (void);
		~NT_Service (void);

		void proxy (Proxy *p);

		virtual void handle_control (DWORD control_code);
		// We override <handle_control> because it handles stop requests
		// privately.

		virtual int  handle_exception (ACE_HANDLE h);
		// We override <handle_exception> so a 'stop' control code can pop
		// the reactor off of its wait.

		virtual int svc (void);
		// This is a virtual method inherited from ACE_NT_Service.

		virtual int handle_timeout (const ACE_Time_Value& tv,
					    const void *arg = 0);

		// Where the real work is done:
	private:
		typedef ACE_NT_Service inherited;
		Proxy *proxy_;
	private:
		int stop_;
	};


	/**
	 * A class with a "magic" constructor/desructor to call the inherited
	 * from ACE_NT_Service report_status() when an instance of it goes out
	 * of scope, should an exception occurs.
	 */
	class AutoFinalizer
	{

	public:
		AutoFinalizer (NT_Service &service);
		~AutoFinalizer ();

	private:
		NT_Service &service_;
	};

	// Define a singleton class as a way to insure that there's only one
	// Service instance in the program, and to protect against access from
	// multiple threads.  The first reference to it at runtime creates it,
	// and the ACE_Object_Manager deletes it at run-down.

	typedef ACE_Singleton<NT_Service, ACE_Mutex> SERVICE;

} // namespace Lorica

#endif /* ACE_WIN32 */
#endif /* NTSVC_H_ */
