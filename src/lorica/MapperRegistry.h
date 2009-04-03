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

#ifndef MAPPER_REGISTRY_H
#define MAPPER_REGISTRY_H

#include <string>
#include <ace/Service_Config.h>
#include <ace/Service_Object.h>
#include <tao/PortableServer/POAManagerC.h>
#include <tao/PortableServer/POAC.h>

#include "Lorica_EvaluatorBase_Export.h"

namespace Lorica
{
	class ReferenceMapValue;
	class ProxyMapper;
	class NullProxyMapper;
}

class Lorica_EvaluatorBase_Export Lorica_MapperRegistry : public ACE_Service_Object {
public:
	Lorica_MapperRegistry(void);

	// Shared object initializer
	int init(int argc, ACE_TCHAR *argv[]);

	// Shared object finalizer
	int fini(void);

	// explicitly clean up resources - fini not always called.
	void destroy (void);

	// Add a type to the list of types explicitly supported by the null
	// prxoy mapper. This overrudes the default behavior of allowing any
	// unknown type to be supported by the null proxy..
	void add_null_mapper_type(const std::string & typeId);

	// Cause the null proxy mapper to be created so that it supports any
	// type. This call is only effective if there is not already a null
	// proxy mapper defined.
	void create_default_null_mapper(void);

	// Sets the generic mapper.
	void set_generic_mapper(Lorica::ProxyMapper *mapper);

	// Add a new proxy mapper to the list
	void add_proxy_mapper(Lorica::ProxyMapper *mapper);

	// Remove a mapper from the list
	void remove_proxy_mapper(Lorica::ProxyMapper *mapper);

	// Initalize the proxy mappers. The mappers are loaded in to the
	// registry via the service configurator, but once that happens,
	// they must be initialized with poa managers and an orb
	// reference.
	void init_mappers(PortableServer::POAManager_ptr inner,
			  PortableServer::POAManager_ptr outer,
			  CORBA::ORB_ptr orb,
			  bool has_security_);

	// Create a mapping for a native reference using the appropriate proxy
	// mapper based on the object type. This method traverses the list of
	// proxy mappers iteratively to locate the correct mapper.
	// The out_facing parameter determines on which POA (outside facing or
	// inside facing) the mapped reference will use. If the supplied ref
	// is known, the previously created reference map value is returned,
	// otherwise a new one is created.
	Lorica::ReferenceMapValue *map_reference(CORBA::Object_ptr native,
						 bool out_facing,
						 bool require_secure = false);

	// Remove a previously mapped reference
	Lorica::ReferenceMapValue *remove_reference(CORBA::Object_ptr mapped,
						    bool out_facing);

	// Static initializer ensures the factory is loaded
	static int initialize(void);

	bool has_security(void) const;

	const CORBA::PolicyList & sec_policies(void) const;

	// Returns the POA and Object id values for an outward facing object
	// reference. This is used by the reference mapper to allow insecure
	// access to objects when security enforcement is enabled.
	bool poa_and_object_id(CORBA::Object_ptr ref,
			       CORBA::OctetSeq *& poaid,
			       PortableServer::ObjectId *& objid);

private:
	// the collection of proxy mappers
	Lorica::ProxyMapper *mappers_;

	// The null-evaluator proxy mapper
	Lorica::NullProxyMapper *null_mapper_;

	// The Generic proxy mapper
	Lorica::ProxyMapper *generic_mapper_;

	// a flag to indicate that init_mappers has been called
	bool mappers_ready_;

	// a flag to indicate that securty features (SSLIOP) are available.
	bool has_security_;

	// when security is availble this is a list of policies to override
	// on individual client-side objects.
	CORBA::PolicyList sec_policies_;
};

static int Lorica_Requires_Mapper_Registry_Initializer = Lorica_MapperRegistry::initialize();

ACE_STATIC_SVC_DECLARE_EXPORT(Lorica_EvaluatorBase, Lorica_MapperRegistry)
ACE_FACTORY_DECLARE(Lorica_EvaluatorBase, Lorica_MapperRegistry)

#endif // MAPPER_REGISTRY_H
