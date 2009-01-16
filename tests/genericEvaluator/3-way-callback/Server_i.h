/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 *    Lorica header file.
 *    Copyright (C) 2009 OMC Denmark ApS.
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

#ifndef SERVER_I_H 
#define SERVER_I_H 

#include "TestS.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

class  Test_Server_i : public virtual POA_Test::Server
{
public:
        // Constructor 
        Test_Server_i(CORBA::ORB_ptr orb,
                      int *success_count,
                      int *failure_count);
  
        // Destructor 
        virtual ~Test_Server_i(void);
  
        virtual ::CORBA::Boolean receive_call(::Test::CallBack_ptr cb);

private:
        // Our Orb...
        CORBA::ORB_var orb_;
        int *success_count_;
        int *failure_count_;
};

#endif /* SERVER_I_H */
