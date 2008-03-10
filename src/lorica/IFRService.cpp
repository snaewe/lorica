/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 *    Lorica source file.
 *    Copyright (C) 2007-2008 OMC Denmark ApS.
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
			 CORBA::ORB_ptr orb)
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
			   "(%P|%t) Lorica::IFRService::init "
			   "passing %d args to ifr_server's orb_init:\n", argc));
		for (int i = 0; i < argc; i++)
			ACE_DEBUG((LM_DEBUG,"  %s\n", arguments->argv()[i]));
	}

	int result;
	try {
		ACE_NEW_RETURN(this->my_ifr_server_,
			       TAO_IFR_Server,
			       -1);

		result = this->my_ifr_server_->init_with_orb(argc,
							     arguments->argv(),
							     orb);
		if (result != 0)
			return result;
	}
	catch (const CORBA::Exception & ex) {
		if (Lorica_debug_level > 0)
			ex._tao_print_exception("Lorica::IFRService::init");
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
			ex._tao_print_exception("Lorica::IFR_Service::fini");
		throw;
	}

	return 0;
}
