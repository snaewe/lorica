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

#ifndef IFRSERVICE_H
#define IFRSERVICE_H

#include <orbsvcs/IFRService/IFR_Service_Utils.h>
#include <tao/PortableServer/PortableServer.h>
#include <tao/ORB.h>

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

namespace Lorica
{
	class IFRService {
	public:
		/// Initialize the IFR service.
		int init(const bool Debug,
			 CORBA::ORB_ptr orb,
			 PortableServer::POA_ptr poa);

		/// Shut down the IFR Service.
		int fini(void);

	private:
		/// IFR Server instance.
		TAO_IFR_Server *my_ifr_server_;
	};
}

#endif /* IFRSERVICE_H */
