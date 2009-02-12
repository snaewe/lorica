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

#include "lorica/ConfigBase.h"
#include "lorica/FileConfig.h"

#include <string>
#include <iostream>

int main (int, char *[])
{
	/************
	 * The values below should match the config file.
	 */
	const std::string test_orb_options ("-ORBDottedDecimalAddresses 1 -ORBObjRefStyle IOR");

	Lorica::Config::Endpoint ep1;
	ep1.external_ = true;
	ep1.hostname_ = "proxyhost";
	ep1.port_ = 951;
	ep1.alias_ = "ociweb.com";
	ep1.is_ipv6_ = false;
	ep1.ssl_port_ = 952;

	Lorica::Config::Endpoint ep2;
	ep2.external_ = false;
	ep2.hostname_ = "proxyhost";
	ep2.port_ = 1951;
	ep2.alias_ = "";
	ep2.is_ipv6_ = false;
	ep2.ssl_port_ = 1952;


	Lorica::Config::Endpoints test_eps;
	test_eps.push_back (ep1);
	test_eps.push_back (ep2);

	ACE_ARGV test_args ("lorica "
			    "-ORBUseSharedProfile 0 -ORBDottedDecimalAddresses 1 -ORBObjRefStyle IOR "
			    "-ORBListenEndpoints iiop://1.2@proxyhost:951/ssl_port=952,"
			    "hostname_in_ior=ociweb.com "
			    "-ORBListenEndpoints iiop://1.2@proxyhost:1951/ssl_port=1952");
	bool test_generic_eval (false);
	bool test_null_eval_unknown (false);
	std::string test_null_ids ("IDL:someID:1.0 IDL:someOtherID:1.0");
	/***
	 * End of test configuration
	 */

	const std::string config_file ("test.conf");

	Lorica::FileConfig *config = Lorica::FileConfig::instance ();
	try
	{
		config->init (config_file);
	}
	catch (const Lorica::FileConfig::InitError& )
	{
		std::cerr << "Failed to load config file: "
			  << config_file << std::endl;

		return -1;
	}

	std::string orb_option = config->get_value ("ORB_Option");
	if (test_orb_options != orb_option)
	{
		std::cerr << "ORB_OPTION mismatch." << std::endl;
		return -1;
	}
	std::cout << "ORB_Option: " << orb_option << std::endl;

	Lorica::Config::Endpoints eps = config->get_endpoints (true);
	Lorica::Config::Endpoints eps2 = config->get_endpoints (false);

	for (size_t count = 0; count < eps2.size(); count++)
		eps.push_back(eps2[count]);



	for (size_t count = 0; count < test_eps.size(); count++)
	{
		if (test_eps[count] != eps[count])
		{
			std::cout <<"eps[" << count <<"] = "
				  << eps[count].hostname_ << ":" << eps[count].port_
				  << "/ssl_port=" << eps[count].ssl_port_
				  << ",alias=" << eps[count].alias_
				  << " (" << (eps[count].external_ ? "ex" : "in")
				  << "ternal" << std::endl;
			std::cout <<"test_eps[" << count <<"] = "
				  << test_eps[count].hostname_ << ":" << test_eps[count].port_
				  << "/ssl_port=" << test_eps[count].ssl_port_
				  << ",alias=" << test_eps[count].alias_
				  << " (" << (test_eps[count].external_ ? "ex" : "in")
				  << "ternal" << std::endl;

			std::cerr << "Endpoints mismatch." << std::endl;
			return 0;
		}
	}
	std::cout << "Endpoint info: ";
	for (unsigned int count = 0; count < eps.size(); count++)
		std::cout << eps[count].hostname_ << ":" << eps[count].port_ << " <> ";

	std::cout << std::endl;

	std::auto_ptr <ACE_ARGV> args (config->get_orb_options ());
	if (args.get() == 0)
		std::cerr << "No ORB args found" << std::endl;


	ACE_TCHAR **test_argv = test_args.argv();
	ACE_TCHAR **the_argv = args->argv();

	if (test_args.argc() != args->argc ())
	{
		std::cerr << "ORB argv count mismatch" << std::endl;
		return -1;
	}

	for (int count = 0; count < test_args.argc(); count++)
	{
		if (ACE_OS::strcmp (test_argv[count], the_argv[count]) != 0)
		{
			std::cerr << "ORB argv[" << count << "] mismatch, "
				  << test_argv[count] << " != "
				  << the_argv[count] <<  std::endl;
			return -1;
		}
	}

	std::cout << "ORB Options: ";
	size_t the_argc = args->argc ();
	for (size_t count = 0; count < the_argc; count++)
		std::cout << the_argv [count] << " ";
	std::cout << std::endl;

	bool generic_eval = config->generic_evaluator ();
	if (test_generic_eval != generic_eval) {
		std::cerr << "Incorrect general evaluator configuration." << std::endl;
	}
	std::cout << "Generic evaluator " << (generic_eval ? "" : "not ") << "applied." << std::endl;

	bool null_eval_unknown = config->null_eval_any ();
	if (test_null_eval_unknown != null_eval_unknown)
		std::cerr << "Incorrect null evaluator any configuration." << std::endl;

	std::cout << "Null evaluator any "
		  << (null_eval_unknown ? "" : "not ")
		  << "applied." << std::endl;

	std::string null_ids = config->null_eval_type_ids ();
	if (test_null_ids != null_ids)
		std::cerr << "Incorrect null evaluator ID string." << std::endl;

	std::cout << "Null evaluator ID's: " << null_ids << std::endl;

	return 0;
}
