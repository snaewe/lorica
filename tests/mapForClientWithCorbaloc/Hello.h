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

#ifndef HELLO_H
#define HELLO_H

#include "TestS.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

/// Implement the Test::Hello interface
class  Hello : public virtual POA_Test::Hello
{
public:
	// Constructor
	Hello(CORBA::ORB_ptr orb);

	// Destructor 
	virtual ~Hello(void);
  
	virtual	char *get_string(void);
  
	virtual void shutdown(void);
  
	virtual void invoke_callback(::Test::CallBack_ptr cb,
				     const char *message_to_callback);

private:
	/// Use an ORB reference to conver strings to objects and shutdown
	/// the application.
	CORBA::ORB_var orb_;
};

#endif /* HELLO_H */
