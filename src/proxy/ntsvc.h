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

//**************************************************************************
//
// BASED ON ACE SAMPLE CODE BY:
//    ACE_wrappers/examples/NT_Service/ntsvc.h
//
//**************************************************************************

#ifndef NTSVC_H_
#define NTSVC_H_

#include <ace/config-all.h>

#if defined (ACE_WIN32)
#include <ace/Event_Handler.h>
#include <ace/NT_Service.h>
#include <ace/Singleton.h>
#include <ace/Synch.h>

namespace Lorica
{
	class Proxy;
	class AutoFinalizer;

	class NT_Service : public ACE_NT_Service {
		friend AutoFinalizer;

	public:
		NT_Service(void);

		~NT_Service(void);

		void proxy(Proxy *p);

		void proxy_activate_args(long flags, int nthreads);

		// We override <handle_control> because it handles stop requests
		// privately.
		virtual void handle_control(DWORD control_code);

		// We override <handle_exception> so a 'stop' control code can pop
		// the reactor off of its wait.
		virtual int handle_exception(ACE_HANDLE h);

		// This is a virtual method inherited from ACE_NT_Service.
		virtual int svc(void);

		// Where the real work is done:
		virtual int handle_timeout(const ACE_Time_Value & tv,
					   const void *arg = 0);

	private:
		typedef ACE_NT_Service inherited;
		Proxy *proxy_;
		long   thr_flags_;
		int    num_threads_;
		int    stop_;
	};


	/**
	 * A class with a "magic" constructor/desructor to call the inherited
	 * from ACE_NT_Service report_status() when an instance of it goes out
	 * of scope, should an exception occurs.
	 */
	class AutoFinalizer {
	public:
		AutoFinalizer(NT_Service &service);

		~AutoFinalizer(void);

	private:
		NT_Service & service_;
	};

	// Define a singleton class as a way to insure that there's only one
	// Service instance in the program, and to protect against access from
	// multiple threads.  The first reference to it at runtime creates it,
	// and the ACE_Object_Manager deletes it at run-down.
	typedef ACE_Singleton<NT_Service, ACE_Mutex> SERVICE;

} // namespace Lorica

#endif /* ACE_WIN32 */
#endif /* NTSVC_H_ */
