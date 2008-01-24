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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ServerAgent_impl.h"

ServerAgent_impl::ServerAgent_impl ()
{
}

ServerAgent_impl::~ServerAgent_impl ()
{
}

void
ServerAgent_impl::error_occured (CORBA::Long code,
				 const char *message)
{
	ACE_DEBUG ((LM_INFO,
		    "ERROR occurred in the proxy.\n"
		    "error code: %d \n"
		    "error message :%s\n",
		    code, message));

}

void
ServerAgent_impl::proxy_disconnect ()
{
	ACE_DEBUG((LM_INFO,
		   "The proxy is shutting down\n"));
}
