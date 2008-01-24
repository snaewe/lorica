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

#include "ConfigBase.h"
#include "debug.h"
#include <ace/OS_NS_stdlib.h>

#include <iostream>

const int Lorica::Config::SYS_PORT = -1;

Lorica::Config::Endpoint::Endpoint ()
	: external_ (false)
	, port_ (0)
	, is_ipv6_ (false)
	, ssl_port_ (0)
{ }

Lorica::Config::Endpoint::Endpoint (const Lorica::Config::Endpoint& a)
{
	operator= (a);
}

Lorica::Config::Endpoint&
Lorica::Config::Endpoint::operator= (const Lorica::Config::Endpoint& a)
{
	if (&a != this)
	{
		this->external_ = a.external_;
		this->hostname_ = a.hostname_;
		this->port_ = a.port_;
		this->is_ipv6_ = a.is_ipv6_;
		this->alias_ = a.alias_;
		this->ssl_port_ = a.ssl_port_;
	}
	return *this;
}

bool
Lorica::Config::Endpoint::operator!= (const Lorica::Config::Endpoint& a)
{
	return (external_ != a.external_
		|| hostname_ != a.hostname_
		|| port_ != a.port_
		|| alias_ != a.alias_
		|| is_ipv6_ != is_ipv6_
		|| ssl_port_ != a.ssl_port_
		);
}
void
Lorica::Config::Endpoint::as_tao_endpoint (std::string &arg)
{
	arg += "iiop://";
	std::string options;
	if (this->is_ipv6_)
		arg += "[" + this->hostname_ + "]:";
	else
		arg += this->hostname_ + ":";

	if (this->port_ != SYS_PORT)
	{
		ACE_TCHAR tmp [15];
		ACE_OS::itoa (this->port_, tmp, 10);
		arg += tmp;
	}

	if (this->ssl_port_ != 0)
	{
		ACE_TCHAR tmp [15];
		ACE_OS::itoa (this->ssl_port_, tmp, 10);
		options += "/ssl_port=";
		options += tmp;
	}

	if (!this->alias_.empty())
	{
		options += options.empty() ? '/' : ',';
		options += "hostname_in_ior=" + this->alias_;
	}

	arg += options;
}

bool
Lorica::Config::Endpoint::parse_string (const std::string &ep_str)
{
	this->is_ipv6_ = false;
	this->ssl_port_ = 0;

	size_t option_pos = ep_str.find ('/');
	std::string primary_addr =
		option_pos != std::string::npos ?
		ep_str.substr(0,option_pos) : ep_str;

	size_t port_sep = primary_addr.rfind(':');
	if (port_sep == std::string::npos)
	{
		ACE_ERROR ((LM_ERROR,
			    "configured endpoint \"%s\" lacks port separator\n",
			    primary_addr.c_str()));
		return false;
	}
	this->port_ = ACE_OS::atoi (primary_addr.substr(port_sep+1).c_str());

	size_t start = 0;
	size_t end = port_sep;
	if (primary_addr[start] == '[' && primary_addr[end-1] == ']')
	{
		this->is_ipv6_ = true;
		++start;
		end -= 2;
	}

	this->hostname_ = primary_addr.substr(start,end);

	if (option_pos != std::string::npos)
	{
		// The rest are optional modifiers, ssl port, hostname alias
		++option_pos;
		std::string test ("alias=");
		size_t opt = ep_str.find (test, option_pos);
		if (opt != std::string::npos)
		{
			size_t comma = ep_str.find (',',opt);
			size_t start = opt + test.length();
			this->alias_ = comma == std::string::npos ?
				ep_str.substr(start) :
				ep_str.substr(start, comma - start);
		}
		test = "ssl_port=";
		opt = ep_str.find (test, option_pos);
		if (opt != std::string::npos)
		{
			size_t comma = ep_str.find (',',opt);
			size_t start = opt + test.length();
			std::string portstr = comma == std::string::npos ?
				ep_str.substr(start) :
				ep_str.substr(start, comma - start);
			this->ssl_port_ = ACE_OS::atoi(portstr.c_str());
		}
	}

	return true;
}

Lorica::Config::~Config (void)
{
}

ACE_ARGV*
Lorica::Config::get_orb_options_copy (void)
{
	ACE_ARGV* args;
	ACE_NEW_RETURN (args
			, ACE_ARGV ()
			, 0);
	ssize_t len = this->orb_args_.size();
	for (ssize_t count = 0; count < len; count++)
		args->add (this->orb_args_[count].c_str());

	return args;
}

ACE_ARGV*
Lorica::Config::get_ifr_options_copy (void)
{
	ACE_ARGV* args;
	ACE_NEW_RETURN (args
			, ACE_ARGV ()
			, 0);

	ssize_t len = this->ifr_args_.size();
	for (ssize_t count = 0; count < len; count++)
		args->add (this->ifr_args_[count].c_str());

	return args;
}

ACE_ARGV*
Lorica::Config::get_orb_options (void)
{
	if (orb_args_.size() > 0)
	{
		return get_orb_options_copy ();
	}

	orb_args_.push_back("lorica_proxy");
	// app name required to ensure proper argument alignment

	std::string opts = this->get_value ("ORB_Option");

	if (!opts.empty ())
	{
		size_t pos = 0;
		while (true)
		{
			size_t tpos = opts.find (' ', pos);
			std::string substr;
			if (tpos == std::string::npos)
				substr = opts.substr (pos);
			else
				substr = opts.substr (pos, tpos-pos);

			if (!substr.empty())
				this->orb_args_.push_back (substr.c_str());


			if (tpos == std::string::npos)
				break;

			pos = tpos+1;
		}
	}

	// Build & add endpoints
	if (this->endpoints_.empty() &&
	    (!this->init_endpoints (true) ||
	     !this->init_endpoints(false)))
	{
		endpoints_.clear();
		orb_args_.clear();
		ACE_ERROR_RETURN ((LM_ERROR,
				   "Unable to parse endpoint configuration values\n"),
				  0);
	}

	for (size_t count = 0;
	     count < this->endpoints_.size ();
	     count++)
	{
		std::string tao_eparg;
		this->endpoints_[count].as_tao_endpoint(tao_eparg);

		this->orb_args_.push_back ("-ORBEndpoint");
		this->orb_args_.push_back (tao_eparg.c_str());
	}

	if (this->debug_level () > 0)
	{
		orb_args_.push_back ("-ORBDebuglevel");

		ACE_TCHAR tmp [15];
		ACE_OS::itoa (this->debug_level(), tmp, 10);
		orb_args_.push_back (tmp);
	}

	return get_orb_options_copy ();
}

ACE_ARGV*
Lorica::Config::get_ifr_options (void)
{
	if (ifr_args_.size() > 0)
	{
		return get_ifr_options_copy ();
	}

	ifr_args_.push_back("ifr_service");
	// app name required to ensure proper argument alignment

	std::string opts = this->get_value ("IFR_CmdOption");

	if (!opts.empty ())
	{
		size_t pos = 0;
		while (true)
		{
			size_t tpos = opts.find (' ', pos);
			std::string substr;
			if (tpos == std::string::npos)
			{
				substr = opts.substr (pos);
			}
			else
			{
				substr = opts.substr (pos, tpos-pos);
			}

			if (!substr.empty())
			{
				this->ifr_args_.push_back (substr.c_str());
			}


			if (tpos == std::string::npos)
				break;

			pos = tpos+1;
		}
	}

	// This is not supported by IFR_Service!? IFR would not start when i had both Lorica and TAO debug enabled (Rasmus).
	/*
	if (this->debug_level () > 0)
	{
		ifr_args_.push_back ("-ORBDebuglevel");

		ACE_TCHAR tmp [15];
		ACE_OS::itoa (this->debug_level(), tmp, 10);
		ifr_args_.push_back (tmp);
	}
	*/
	return get_ifr_options_copy ();
}


bool
Lorica::Config::init_endpoints (bool do_extern)
{
	std::string eps_str = do_extern ?
		this->get_value ("External_Address") :
		this->get_value ("Internal_Address");
	size_t pos = 0;
	size_t tpos = 0;
	for (; tpos != std::string::npos; pos = tpos + 1)
	{
		tpos = eps_str.find (' ', pos);
		std::string ep_str = (tpos == std::string::npos) ?
			eps_str.substr (pos) :
			eps_str.substr (pos, tpos-pos);

		Endpoint ep;
		ep.external_ = do_extern;
		if (ep.parse_string(ep_str))
			endpoints_.push_back(ep);
		else
			return false;
	}
	return true;
}

Lorica::Config::Endpoints
Lorica::Config::get_endpoints (bool ext)
{
	Endpoints eps;
	if (this->endpoints_.empty() &&
	    (!this->init_endpoints(true) ||
	     !this->init_endpoints(false)))
	{
		this->endpoints_.clear();
		return eps;
	}
	for (Endpoints::const_iterator iter = endpoints_.begin();
	     iter != endpoints_.end();
	     iter++)
	{
		if (iter->external_ == ext)
			eps.push_back (*iter);
	}

	return eps;
}

bool
Lorica::Config::null_eval_any (void) const
{
	// First check if a NULL evaluator ID list has been provided.
	std::string ids = this->null_eval_type_ids ();
	if (!ids.empty())
		return false;

	std::string eval_any = this->get_value ("Null_Evaluator_Any");
	return eval_any.compare ("yes") == 0;
}

std::string
Lorica::Config::null_eval_type_ids (void) const
{
	return this->get_value ("Null_Eval_Type_ID");
}

bool
Lorica::Config::generic_evaluator (void) const
{
	std::string gen_eval = this->get_value ("Generic_Evaluator");
	return gen_eval.compare ("yes") == 0;
}

bool
Lorica::Config::collocate_ifr (void) const
{
	std::string coll_ifr = this->get_value ("Collocate_IFR");
	return coll_ifr.compare ("no") != 0;
}

bool
Lorica::Config::secure_available (void) const
{
	return this->secure_available_;
}

void
Lorica::Config::secure_available (bool sa)
{
	this->secure_available_ = sa;
}
