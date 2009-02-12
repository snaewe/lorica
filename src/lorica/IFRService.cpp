/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: t; c-basic-offset: 2 -*- */

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

#include "defines/build-system-defs.h"

#include <orbsvcs/IFRService/ComponentRepository_i.h>
#include <orbsvcs/IFRService/Options.h>
#include <orbsvcs/IFRService/IFR_ComponentsS.h>
#include <orbsvcs/IFRService/IFR_Service_Utils.h>
#include <orbsvcs/IOR_Multicast.h>
#include <tao/IORTable/IORTable.h>
#include <tao/ORB_Core.h>
#include <ace/Auto_Ptr.h>

#include "IFRService.h"
#include "ConfigBase.h"
#include "FileConfig.h"
#include "debug.h"

int
Lorica::IFRService::init(const bool Debug,
												 CORBA::ORB_ptr orb,
                         PortableServer::POA_ptr poa)
{
	Lorica::Config *config = Lorica::FileConfig::instance();

	if (!config->collocate_ifr()) {
		this->my_ifr_server_ = 0;
		return 0;
	}

	ACE_ARGV *arguments = config->get_ifr_options(Debug);
	int argc = arguments->argc();

	if (Lorica_debug_level > 0) {
		ACE_DEBUG((LM_DEBUG,
			   ACE_TEXT("(%T) %N:%l - passing %d args to ifr_server's orb_init:\n"),
			   argc));
		for (int i = 0; i < argc; i++)
			ACE_DEBUG((LM_DEBUG,
				   ACE_TEXT("%N:%l - %s\n"),
				   arguments->argv()[i]));
	}

	int result;
	try {
		ACE_NEW_RETURN(this->my_ifr_server_,
			       TAO_IFR_Server,
			       -1);

		result = this->my_ifr_server_->init_with_poa(argc, arguments->argv(),
																								 orb, poa);
		if (result != 0)
			return result;
	}
	catch (const CORBA::Exception & ex) {
		if (Lorica_debug_level > 0)
			ACE_DEBUG((LM_ERROR, ACE_TEXT("(%T) %N:%l - %s\n"), ex._info().c_str()));
		throw;
	}

	return 0;
}

int
Lorica::IFRService::fini(void)
{
	try {
		if (this->my_ifr_server_)
			this->my_ifr_server_->fini();
	}
	catch (const CORBA::Exception& ex) {
		if (Lorica_debug_level > 0)
			ACE_DEBUG((LM_ERROR, ACE_TEXT("(%T) %N:%l - %s\n"), ex._info().c_str()));
		throw;
	}

	return 0;
}
