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

#include "defines/build-system-defs.h"

#include "proxy/ReferenceMapper_i.h"
#include "proxy/proxy.h"
#include "lorica/FileConfig.h"
#include "lorica/MapperRegistry.h"
#include "lorica/debug.h"

#ifndef ACE_WIN32
#include "lorica/GenericMapper.h"
#endif

#if defined (LORICA_STATIC) && !defined (LORICA_LACKS_SSLIOP)
#include <orbsvcs/SSLIOP/SSLIOP_Factory.h>
#endif

#include <tao/EndpointPolicy/IIOPEndpointValue_i.h>
#include <tao/EndpointPolicy/EndpointPolicy.h>
#include <tao/PortableServer/Servant_Base.h>
#include <tao/ORB_Core.h>

#include <ace/Signal.h>
#include <ace/OS_NS_stdio.h>
#include <ace/OS_NS_unistd.h>
#include <ace/OS_NS_fcntl.h>

#include <ace/Service_Gestalt.h>
#include <ace/Time_Value.h>

Lorica::Proxy* Lorica::Proxy::this_ = 0;


Lorica::Proxy::Proxy(const bool Debug)
	: ior_file_(),
	  must_shutdown_(false),
	  debug_(Debug)
{
}

void
Lorica::Proxy::shutdown(void)
{
	this->must_shutdown_ = true;
	this->orb_->shutdown(true);
}

int
Lorica::signal_handler(int signum)
{
	/// Need to make this thread safe
	if (Lorica::Proxy::this_ != 0)
		Lorica::Proxy::this_->shutdown ();

	ACE_UNUSED_ARG(signum);

	return 0;
};

bool
Lorica::Proxy::setup_shutdown_handler(void)
{
	ACE_Sig_Set sigset;

	// Register signal handlers.
	sigset.sig_add(SIGINT);
	sigset.sig_add(SIGUSR1);
	sigset.sig_add(SIGUSR2);
	sigset.sig_add(SIGTERM);

	// Register the <handle_signal> method to process all the signals in
	// <sigset>.
	ACE_Sig_Action sa(sigset,
			  (ACE_SignalHandler)Lorica::signal_handler);
	ACE_UNUSED_ARG(sa);

	return true;
}

int
Lorica::Proxy::open(void *args)
{
	if (args == 0)
		return 0;

	ACE_TCHAR *config_file_ = reinterpret_cast<ACE_TCHAR*>(args);
	int corba_debug_level_ = 0;

	Lorica::FileConfig *config = Lorica::FileConfig::instance();
	try {
		config->init(config_file_, corba_debug_level_);
	}
	catch (const FileConfig::InitError &) {
		ACE_ERROR((LM_ERROR,
			   ACE_TEXT("%N:%l - proxy could not read config file %s.\n"),
			   config_file_));

		return 0;
	}

	try {
		this->configure(*config);
	}
	catch (const Lorica::Proxy::InitError &) {
		ACE_ERROR((LM_ERROR,
			   ACE_TEXT("%N:%l - proxy configuration failed.\n")));

		return 0;
	}

	return 1;
}

void
Lorica::Proxy::configure(Config & config, 
			 const std::string &def_ior_file )
	throw (InitError)
{
	try {
		// This should be OK even if multiple copies of Proxy
		// get created as they all create the same ORB instance
		// and therefore the single ORB instance will get shutdown.
		Lorica::Proxy::this_ = this;

		std::auto_ptr<ACE_ARGV> arguments(config.get_orb_options());

		// Create proxy ORB.
		int argc = arguments->argc();
		if (Lorica_debug_level > 2) {
			ACE_DEBUG((LM_DEBUG,
				   ACE_TEXT("(%T) %N:%l - passing %d args to ORB_init:\n"), 
				   argc));

			for (int i = 0; i < argc; i++)
				ACE_DEBUG((LM_DEBUG, ACE_TEXT("%N:%l - %s\n"), arguments->argv()[i]));
		}
		orb_ = CORBA::ORB_init(argc, arguments->argv());

		// test if we have any security functionality
		ACE_Service_Repository * repo = 
			orb_->orb_core()->configuration()->
			current_service_repository();
		config.secure_available(repo->find("SSLIOP_Factory") == 0);

		int attempts = 3;
		CORBA::Object_var obj = CORBA::Object::_nil();
		resolve_again:
		try {
			obj = orb_->resolve_initial_references("RootPOA");
		}
		catch (CORBA::Exception & ex) {
			if (attempts--) {
				ACE_DEBUG((LM_INFO, ACE_TEXT("(%T) %N:%l - Exception trying to resolve initial references\n")));
				ACE_OS::sleep(10);
				goto resolve_again;
			}
			ACE_DEBUG((LM_ERROR, ACE_TEXT("(%T) %N:%l - %s\n"), ex._info().c_str()));
			throw InitError();
		}

		root_poa_ = PortableServer::POA::_narrow(obj.in());
		if (CORBA::is_nil(root_poa_.in())) {
			ACE_ERROR((LM_ERROR,
				   "(%T) %N:%l - could not get root POA\n"));
			throw InitError();
		}

		pmf_ = root_poa_->the_POAManagerFactory();
		if (CORBA::is_nil(pmf_.in())) {
			ACE_ERROR((LM_ERROR,
				   "(%T) %N:%l - could not get PMF\n"));
			throw InitError();
		}

		obj = orb_->resolve_initial_references("IORTable");
		iorTable_ = IORTable::Table::_narrow(obj.in());
		if (CORBA::is_nil(iorTable_.in())) {
			ACE_ERROR((LM_ERROR,
				   "(%T) %N:%l - could not get IORTable\n"));
			throw InitError();
		}

		CORBA::PolicyList policies;
		EndpointPolicy::EndpointList list;
		CORBA::Any policy_value;

		// Create external POA manager
		Config::Endpoints ex_points = config.get_endpoints(true);
		policies.length(1);
		list.length(ex_points.size());
		if (Lorica_debug_level > 2) {
			ACE_DEBUG((LM_DEBUG,
				   ACE_TEXT("(%T) %N:%l - setting up External POA manager with %d endpoints\n"),
				   ex_points.size()));
		}
		for (size_t count = 0; count < ex_points.size(); count++) {
			std::string inter = ex_points[count].hostname_;
			if (!ex_points[count].alias_.empty())
				inter = ex_points[count].alias_;

			list[count] = new IIOPEndpointValue_i(inter.c_str(),
							      ex_points[count].port_);
		}
		policy_value <<= list;
		policies[0] = orb_->create_policy(EndpointPolicy::ENDPOINT_POLICY_TYPE,
						  policy_value);
		outside_pm_ = pmf_->create_POAManager("OutsidePOAManager",
						      policies);

		// Create internal POA manager.
		Config::Endpoints in_points = config.get_endpoints(false);
		policies.length(1);
		list.length(in_points.size());
		if (Lorica_debug_level > 2) {
			ACE_DEBUG((LM_DEBUG,
				   ACE_TEXT("(%T) %N:%l - setting up Internal POA manager with %d endpoints\n"),
				   in_points.size()));
		}
		for (size_t count = 0; count < in_points.size(); count++) {
			std::string inter = in_points[count].hostname_;
			if (!in_points[count].alias_.empty())
				inter = in_points[count].alias_;

			list[count] = new IIOPEndpointValue_i(inter.c_str(),
							      in_points[count].port_);
		}
		policy_value <<= list;
		policies[0] = orb_->create_policy(EndpointPolicy::ENDPOINT_POLICY_TYPE,
						  policy_value);
		inside_pm_ = pmf_->create_POAManager("InsidePOAManager",
						     policies);

		if (Lorica_debug_level > 2) {
			ACE_DEBUG((LM_DEBUG,
				   ACE_TEXT("(%T) %N:%l - creating admin POA with internal POA manager\n")));
		}
		policies.length(0);
		admin_poa_ = root_poa_->create_POA("adminPOA",
						   inside_pm_.in(),
						   policies);

		try {
			ReferenceMapper_i *refMapper = new ReferenceMapper_i(orb_.in(),
									     iorTable_.in(),
									     config.secure_available());
			PortableServer::ServantBase_var refMapperServant = refMapper;

			PortableServer::ObjectId_var oid = admin_poa_->activate_object(refMapperServant.in());
			obj = admin_poa_->id_to_reference(oid.in());

			// refMapper->allow_insecure_access (obj.in());
		}
		catch (CORBA::Exception & ex) {
			ACE_DEBUG((LM_ERROR, ACE_TEXT("(%T) %N:%l - %s\n"), ex._info().c_str()));
			throw InitError();
		}

		ReferenceMapper_var refMapper_obj = ReferenceMapper::_narrow(obj.in());

		CORBA::String_var ior = 
			orb_->object_to_string(refMapper_obj.in());
		iorTable_->bind(Lorica::ReferenceMapper::IOR_TABLE_KEY,
				ior.in());

		this->ior_file_ = config.get_value("IOR_FILE", def_ior_file);

		FILE *output_file =
			ACE_OS::fopen (ACE_TEXT_CHAR_TO_TCHAR (this->ior_file_.c_str()), ACE_TEXT("w"));
		if (!output_file) {
			ACE_ERROR((LM_ERROR,
				   "(%T) %N:%l - cannot open output file for writing IOR: %s\n",
				   this->ior_file_.c_str()));
			throw InitError();
		}
		ACE_OS::fprintf(output_file, "%s", ior.in());
		ACE_OS::fclose(output_file);

		if (!setup_shutdown_handler()) {
			ACE_ERROR ((LM_ERROR,
				    "(%T) %N:%l - could not set up shutdown handler\n"));
			throw InitError();
		}

		// Initialize the mapper registry
		Lorica_MapperRegistry *mreg = 
			ACE_Dynamic_Service<Lorica_MapperRegistry>::instance
			(this->orb_->orb_core()->configuration(),"MapperRegistry");

		std::string ne_ids = config.null_eval_type_ids();
		if (!ne_ids.empty()) {
			if (Lorica_debug_level > 2) {
				ACE_DEBUG((LM_DEBUG,
					   ACE_TEXT("(%T) %N:%l - adding type ids for null ")
					   ACE_TEXT("evaluator: %s\n"),
					   ne_ids.c_str()));
			}

			size_t space = ne_ids.find(' ');
			size_t start = 0;
			while (space != std::string::npos) {
				mreg->add_null_mapper_type(ne_ids.substr(start,space - start));
				start = space+1;
				space = ne_ids.find(' ',start);
			}
			mreg->add_null_mapper_type(ne_ids.substr(start));
		} else if (config.null_eval_any()) {
			if (Lorica_debug_level > 2)
				ACE_DEBUG((LM_DEBUG,
					   ACE_TEXT("(%T) %N:%l - creating default null mapper\n")));
			mreg->create_default_null_mapper();
		}
		if (config.generic_evaluator()) {
			if (Lorica_debug_level > 0) {
				ACE_DEBUG((LM_DEBUG,
					   ACE_TEXT("(%T) %N:%l - loading generic evaluator\n")));
			}
#ifdef ACE_WIN32
			this->orb_->orb_core()->configuration()->process_directive
				(ACE_TEXT_ALWAYS_CHAR
				 (ACE_DYNAMIC_SERVICE_DIRECTIVE("Lorica_GenericLoader",
								"lorica_GenericEvaluator",
								"_make_Lorica_GenericLoader",
								"")
					 )
					);
#else
			Lorica::GenericMapper *gen_eval =
				new Lorica::GenericMapper(debug_, *mreg);
			mreg->add_proxy_mapper(gen_eval);
#endif
		}

		// add generic or other mappers...
		mreg->init_mappers(this->outside_pm_,
				   this->inside_pm_,
				   this->orb_,
				   config.secure_available());



		// last thing to do, put both POAs in the active state.
		outside_pm_->activate();
		inside_pm_->activate();

	}
	catch (CORBA::Exception & ex) {
		ACE_DEBUG((LM_ERROR, ACE_TEXT("(%T) %N:%l - %s\n"), ex._info().c_str()));
		throw InitError();
	}
	catch (...) {
		ACE_ERROR((LM_ERROR, "%N:%l - Caught an otherwise unknown exception\n"));
		throw InitError();
	}
}

Lorica::Proxy::~Proxy(void)
{
	this->destroy();
}

int
Lorica::Proxy::svc(void)
{
// 	while (!this->must_shutdown_) {
// 		ACE_Time_Value timeout(1,0);
// 		this->orb_->run(timeout);
// 	}
	this->orb_->run();
	return 0;
}

void
Lorica::Proxy::destroy (void)
{
	if (this->orb_.ptr() == 0)
		return;

	Lorica_MapperRegistry *mreg = 
		ACE_Dynamic_Service<Lorica_MapperRegistry>::instance
		(this->orb_->orb_core()->configuration(),"MapperRegistry");
	if (mreg != 0)
		mreg->destroy();

	this->orb_->destroy();
	this->orb_ = CORBA::ORB::_nil();
}
