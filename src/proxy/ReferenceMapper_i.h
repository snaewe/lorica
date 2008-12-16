/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 *    Lorica header file.
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

#ifndef REFERENCE_MAPPER_I_H
#define REFERENCE_MAPPER_I_H

#include <tao/IORTable/IORTable.h>
#if !defined (LORICA_LACKS_SSLIOP)
#include <orbsvcs/SecurityLevel2C.h>
#endif
#include "lorica/ReferenceMapperS.h"

class Lorica_MapperRegistry;

namespace Lorica
{
	class Config;
	// Interface used by applications within a firewalled enclave. The
	// ReferenceMapper converts supplied object references into proxy managed
	// references that may be exported by servers or used by internal clients.

	class ReferenceMapper_i : public virtual POA_Lorica::ReferenceMapper {
	public:
		class InitError {
		};

	public:
		ReferenceMapper_i(CORBA::ORB_ptr orb,
				  IORTable::Table_ptr iort,
				  bool has_sec);

		virtual ~ReferenceMapper_i(void);


		// The arguments are:
		//
		//   orig:           The original object reference
		//
		//   corbaloc_name:  If not "", use this name to create a binding in
		//                   the proxy's IOR table.
		//
		//   agent:          The server agent used to receive messages related to
		//                   the reference being mapped.
		//
		CORBA::Object_ptr as_server(CORBA::Object_ptr orig,
					    const char *corbaloc_name,
					    Lorica::ServerAgent_ptr agent)
			throw (CORBA::SystemException);

		// The same as "as_server" however in this form, the mapped reference
		// will require secure access. If security is not available an execption
		// is raised.
		CORBA::Object_ptr as_secure_server(CORBA::Object_ptr orig,
						   const char *corbaloc_name,
						   Lorica::ServerAgent_ptr agent)
			throw (CORBA::SystemException,
			       Lorica::ReferenceMapper::SecurityNotAvailable);

		// Map a reference that a client inside an enclave would use to reach
		// servers on the outside. This is necessary only when the interface
		// associated with the supplied reference may be used to pass object
		// references.
		CORBA::Object_ptr as_client(CORBA::Object_ptr orig)
			throw (CORBA::SystemException);

		// As as_client() above but with an explicit IFR ID.
		::CORBA::Object_ptr as_client_with_corbaloc(const char *corbaloc,
							    const char *interface_repository_id)
			throw (CORBA::SystemException);
  
		// Remove a previously mapped server reference. This will unbind the
		// reference from the IOR table, if previously bound. This should
		// probably generate an error if the mapped reference is not known.
		void remove_server(CORBA::Object_ptr mapped)
			throw (CORBA::SystemException);

		// Remove a previously mapped client reference. Client references are
		// counted, as many clients may wish to communicate with the same server.
		// This should probably return an error if the mapped reference is not
		// known.
		void remove_client(CORBA::Object_ptr mapped)
			throw (CORBA::SystemException);

		// Returns true if the proxy is capable of supporting secure invocations.
		// This status depends on the configuration of the proxy. If SSLIOP is
		// not installed, then a current won't be available and thus no security
		// support is available.
		CORBA::Boolean supports_security(void)
			throw (CORBA::SystemException);

		// Internal access to the registry, used to initialize the mappers.
		Lorica_MapperRegistry *registry(void);

		// Add the supplied reference to the AccessDecision object's list of
		// allowed objects.
		void allow_insecure_access(CORBA::Object_ptr self);

	private:

		CORBA::Object_ptr as_server_i(bool require_secure,
					      CORBA::Object_ptr orig,
					      const char *corbaloc_name,
					      Lorica::ServerAgent_ptr agent)
			throw (CORBA::SystemException);

		Lorica_MapperRegistry *registry_;
		CORBA::ORB_var orb_;
		IORTable::Table_var ior_table_;

		bool has_security_;
#if !defined (LORICA_LACKS_SSLIOP)
		TAO::SL2::AccessDecision_var access_decision_;
#endif //LORICA_LACKS_SSLIOP
	};
}


#endif // REFERENCE_MAPPER_I_H
