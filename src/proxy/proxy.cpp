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

#include "proxy/ReferenceMapper_i.h"
#include "proxy/proxy.h"
#include "lorica/FileConfig.h"
#include "lorica/MapperRegistry.h"
#include "lorica/debug.h"
#ifndef ACE_WIN32
#include "lorica/GenericMapper.h"
#endif

#include <tao/EndpointPolicy/IIOPEndpointValue_i.h>
#include <tao/EndpointPolicy/EndpointPolicy.h>
#include <tao/PortableServer/Servant_Base.h>
#include <tao/ORB_Core.h>

#include <ace/Signal.h>
#include <ace/OS_NS_stdio.h>
#include <ace/OS_NS_unistd.h>
#include <ace/Service_Gestalt.h>
#include <ace/Time_Value.h>

Lorica::Proxy* Lorica::Proxy::this_ = 0;

void
Lorica::Proxy::shutdown ()
{
	this->must_shutdown_ = true;
}

int
Lorica::signal_handler (int signum)
{
	/// Need to make this thread safe
	if (Lorica::Proxy::this_ != 0) {
		Lorica::Proxy::this_->shutdown ();
	}

	ACE_UNUSED_ARG (signum);
	return 0;
};

bool
Lorica::Proxy::setup_shutdown_handler (void)
{
	ACE_Sig_Set sigset;

	// Register signal handlers.
	sigset.sig_add (SIGINT);
	sigset.sig_add (SIGUSR1);
	sigset.sig_add (SIGTERM);

	// Register the <handle_signal> method to process all the signals in
	// <sigset>.
	ACE_Sig_Action sa (sigset,
			   (ACE_SignalHandler) Lorica::signal_handler);
	ACE_UNUSED_ARG (sa);

	return true;
}

Lorica::Proxy::Proxy ()
	: pid_file_ (),
	  ior_file_ (),
	  must_shutdown_ (false)
{
	// nothing else to do
}

int
Lorica::Proxy::open (void *args)
{
	if (args == 0)
		return 0;
	ACE_TCHAR *config_file_ = reinterpret_cast<ACE_TCHAR*>(args);
	int corba_debug_level_ = 0;

	Lorica::FileConfig *config = Lorica::FileConfig::instance ();
	try {
		config->init (config_file_, corba_debug_level_);
	}
	catch (const FileConfig::InitError& ) {
		ACE_ERROR((LM_ERROR,
			   ACE_TEXT("Proxy could not read config file %s.\n"),
			   config_file_));
		return 0;
	}

	try {
		this->configure(*config);
	}
	catch (const Lorica::Proxy::InitError& ) {
		ACE_ERROR((LM_ERROR,
			   ACE_TEXT("Proxy configuration failed.\n")));
		return 0;
	}

	return 1;
}

void
Lorica::Proxy::configure(Config& config)
	throw (InitError)
{
	try {
		// This should be OK even if multiple copies of Proxy
		// get created as they all create the same ORB instance
		// and therefore the single ORB instance will get shutdown.
		Lorica::Proxy::this_ = this;

		std::auto_ptr<ACE_ARGV> arguments (config.get_orb_options());

		this->pid_file_ = config.get_value("PID_FILE");
		if (this->pid_file_.length() == 0)
#ifdef ACE_WIN32
			this->pid_file_ = "lorica.pid";
#else
		this->pid_file_ = LORICA_PID_FILE;
#endif
		// Create proxy ORB.
		int argc = arguments->argc();
		if (Lorica_debug_level > 2) {
			ACE_DEBUG ((LM_DEBUG,
				    "(%P|%t) Lorica::Proxy::configure "
				    "passing %d args to ORB_init:\n", argc));
			for (int i = 0; i < argc; i++)
				ACE_DEBUG((LM_DEBUG,"  %s\n", arguments->argv()[i]));
		}
		orb_ = CORBA::ORB_init (argc, arguments->argv());//, "ProxyORB");

		// test that we have any security functionality

		ACE_Service_Repository * repo = orb_->orb_core()->configuration()->current_service_repository();
		config.secure_available (repo->find("SSLIOP_Factory") == 0);

		CORBA::Object_var obj = orb_->resolve_initial_references("RootPOA");
		root_poa_ = PortableServer::POA::_narrow (obj.in ());
		if (CORBA::is_nil (root_poa_.in ())) {
			ACE_ERROR ((LM_ERROR,
				    "(%P|%t) Lorica::Proxy::configure "
				    "Could not get root POA\n"));
			throw InitError ();
		}

		pmf_ = root_poa_->the_POAManagerFactory ();
		if (CORBA::is_nil (pmf_.in ())) {
			ACE_ERROR ((LM_ERROR,
				    "(%P|%t) Lorica::Proxy::configure "
				    "Could not get PMF\n"));
			throw InitError ();
		}

		obj = orb_->resolve_initial_references ("IORTable");
		iorTable_ = IORTable::Table::_narrow (obj.in());
		if (CORBA::is_nil (iorTable_.in ())) {
			ACE_ERROR ((LM_ERROR,
				    "(%P|%t) Lorica::Proxy::configure "
				    "Could not get IORTable\n"));
			throw InitError ();
		}

		CORBA::PolicyList policies;
		EndpointPolicy::EndpointList list;
		CORBA::Any policy_value;

		// Create external POA manager
		Config::Endpoints ex_points = config.get_endpoints (true);
		policies.length (1);
		list.length (ex_points.size());
		if (Lorica_debug_level > 2)
			ACE_DEBUG ((LM_DEBUG,
				    "(%P|%t) Lorica::Proxy::configure "
				    "Setting up External PM with %d endpoints\n",
				    ex_points.size()));
		for (size_t count = 0; count < ex_points.size(); count++) {
			std::string inter = ex_points[count].hostname_;
			if (!ex_points[count].alias_.empty())
				inter = ex_points[count].alias_;

			list[count] = new IIOPEndpointValue_i (inter.c_str(),
							       ex_points[count].port_);
		}
		policy_value <<= list;
		policies[0] = orb_->create_policy (EndpointPolicy::ENDPOINT_POLICY_TYPE,
						   policy_value);
		outside_pm_ = pmf_->create_POAManager ("OutsidePOAManager",
						       policies);

		// Create internal POA manager.
		Config::Endpoints in_points = config.get_endpoints (false);
		policies.length (1);
		list.length (in_points.size());
		if (Lorica_debug_level > 2)
			ACE_DEBUG ((LM_DEBUG,
				    "(%P|%t) Lorica::Proxy::configure "
				    "Setting up Internal PM with %d endpoints\n",
				    in_points.size()));
		for (size_t count = 0; count < in_points.size(); count++) {
			std::string inter = in_points[count].hostname_;
			if (!in_points[count].alias_.empty())
				inter = in_points[count].alias_;

			list[count] = new IIOPEndpointValue_i (inter.c_str(),
							       in_points[count].port_);
		}
		policy_value <<= list;
		policies[0] = orb_->create_policy (EndpointPolicy::ENDPOINT_POLICY_TYPE,
						   policy_value);
		inside_pm_ = pmf_->create_POAManager ("InsidePOAManager",
						      policies);


		if (Lorica_debug_level > 2)
			ACE_DEBUG ((LM_DEBUG,
				    "(%P|%t) Lorica::Proxy::configure "
				    "Creating admin POA with internal PM\n"));
		policies.length (0);
		admin_poa_ = root_poa_->create_POA ("adminPOA",
						    inside_pm_.in(),
						    policies);

		try {
			ReferenceMapper_i *refMapper = new ReferenceMapper_i (orb_.in(),
									      iorTable_.in(),
									      config.secure_available());
			PortableServer::ServantBase_var refMapperServant = refMapper;

			PortableServer::ObjectId_var oid = admin_poa_->activate_object (refMapperServant.in());
			obj = admin_poa_->id_to_reference (oid.in());

			//          refMapper->allow_insecure_access (obj.in());
		}
		catch (CORBA::Exception &ex) {
			ex._tao_print_exception("activating mapper object");
			throw InitError();
		}

		ReferenceMapper_var refMapper_obj = ReferenceMapper::_narrow (obj.in());

		std::string ior = orb_->object_to_string (refMapper_obj.in ());
		iorTable_->bind ("lorica_reference_mapper", ior.c_str());

		this->ior_file_ = config.get_value("IOR_FILE");
		if (this->ior_file_.length() == 0)
#ifdef ACE_WIN32
			this->ior_file_ = "lorica.ior";
#else
		this->ior_file_ = LORICA_IOR_FILE;
#endif
		FILE *output_file= ACE_OS::fopen (this->ior_file_.c_str(), "w");
		if (output_file == 0) {
			ACE_ERROR ((LM_ERROR,
				    "(%P|%t) Lorica::Proxy::configure "
				    "Cannot open output file for writing IOR: %s\n",
				    "lorica.ior"));
		} else {
			ACE_OS::fprintf (output_file, "%s", ior.c_str());
			ACE_OS::fclose (output_file);
		}

		if (!setup_shutdown_handler ()) {
			ACE_ERROR ((LM_ERROR,
				    "(%P|%t) Lorica::Proxy::configure "
				    "Could not set up shutdown handler\n"));
			throw InitError ();
		}


		// Initialize the mapper registry
		Lorica_MapperRegistry *mreg = ACE_Dynamic_Service<Lorica_MapperRegistry>::instance
			(this->orb_->orb_core()->configuration(),"MapperRegistry");

		std::string ne_ids = config.null_eval_type_ids();
		if (!ne_ids.empty()) {

			if (Lorica_debug_level > 2) {
				ACE_DEBUG ((LM_DEBUG,
					    "(%P|%t) Lorica::Proxy::configure "
					    "adding type ids for null evaluator: %s\n",
					    ne_ids.c_str()));
			}

			size_t space = ne_ids.find(' ');
			size_t start = 0;
			while (space != std::string::npos) {
				mreg->add_null_mapper_type(ne_ids.substr(start,space - start));
				start = space+1;
				space =  ne_ids.find(' ',start);
			}
			mreg->add_null_mapper_type(ne_ids.substr(start));
		} else if (config.null_eval_any()) {
			if (Lorica_debug_level > 2)
				ACE_DEBUG ((LM_DEBUG,
					    "(%P|%t) Lorica::Proxy::configure "
					    "creating default null mapper\n"));
			mreg->create_default_null_mapper();
		}

		if (config.generic_evaluator()) {
			if (Lorica_debug_level > 0)
				ACE_DEBUG ((LM_DEBUG,
					    "(%P|%t) Lorica::Proxy::configure "
					    "Loading generic evaluator\n"));
#ifdef ACE_WIN32
			this->orb_->orb_core()->configuration()->process_directive(ACE_TEXT_ALWAYS_CHAR
										   (ACE_DYNAMIC_SERVICE_DIRECTIVE("Lorica_GenericLoader",
														  "lorica_GenericEvaluator",
														  "_make_Lorica_GenericLoader",
														  "")));
#else
			Lorica::GenericMapper::GenericMapper *gen_eval = new Lorica::GenericMapper::GenericMapper(*mreg);
			mreg->add_proxy_mapper(gen_eval);
#endif
		}

		// add generic or other mappers...
		mreg->init_mappers(this->outside_pm_,
				   this->inside_pm_,
				   this->orb_,
				   config.secure_available());
	}
	catch (CORBA::Exception &ex) {
		ex._tao_print_exception ("(%P|%t) Lorica::Proxy::configure "
					 "Caught a CORBA exception\n");
		throw InitError();
	}
	catch (...) {
		ACE_ERROR ((LM_ERROR,
			    "(%P|%t) Lorica::Proxy::configure "
			    "Caught an otherwise unknown exception\n"));
		throw InitError();
	}
}

Lorica::Proxy::~Proxy (void)
{
}

int
Lorica::Proxy::svc (void)
{
	outside_pm_->activate ();
	inside_pm_->activate ();

	// Output the pid file indicating we are running
	FILE *output_file= ACE_OS::fopen (this->pid_file_.c_str(), "w");
	if (output_file == 0) {
		ACE_ERROR_RETURN ((LM_ERROR,
				   "Cannot open output file for writing PID: %s\n",
				   this->pid_file_.c_str()),
				  1);
	}
	ACE_OS::fprintf (output_file, "%d\n", ACE_OS::getpid());
	ACE_OS::fclose (output_file);

	while (! this->must_shutdown_) {
		ACE_Time_Value timeout(1,0);
		this->orb_->run(timeout);
	}
	ACE_OS::unlink (this->pid_file_.c_str());

	return 0;
}
