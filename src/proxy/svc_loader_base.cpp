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
//    ACE_wrappers/examples/NT_Service and
//    ACE_wrappers/TAO/orbsvcs/ImplRepo_Service
//
//**************************************************************************

#include "svc_loader_base.h"
#include <ace/Log_Msg.h>
#include <ace/streams.h>

Lorica::Service_Loader_Base::Service_Loader_Base(void)
	: config_file_(LORICA_CONF_FILE),
	  corba_debug_level_(0),
	  debug_(false),
	  proxy_thr_flags_ (THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED),
	  num_threads_ (1)
{
}

Lorica::Service_Loader_Base::~Service_Loader_Base(void)
{
}

int
Lorica::Service_Loader_Base::run_service_command(void)
{
	return 0;
}

bool
Lorica::Service_Loader_Base::is_service (void)
{
	return !debug_;
}

void
Lorica::Service_Loader_Base::set_working_directory(const ACE_TCHAR *)
{
}

void
Lorica::Service_Loader_Base::reset_log(void)
{
	ACE_LOG_MSG->clr_flags(ACE_Log_Msg::SYSLOG);

	ofstream *output_file = 
		new ofstream(ACE_TEXT("lorica.log"), ios::out);

	if (output_file && (output_file->rdstate() == ios::goodbit))
		ACE_LOG_MSG->msg_ostream(output_file, 1);
	
	ACE_LOG_MSG->set_flags(ACE_Log_Msg::STDERR | ACE_Log_Msg::OSTREAM);
}

bool
Lorica::Service_Loader_Base::get_lock (const char *)
{
	return false;
}
