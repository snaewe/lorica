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

#include "defines/build-system-defs.h"

#include <iostream>
#include <ace/os_include/os_limits.h>
#include <ace/OS_NS_stdlib.h>
#include <ace/OS_NS_unistd.h>

// used by get_ip_from_ifname()
#ifndef ACE_WIN32
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <ifaddrs.h>
#endif

#include "ConfigBase.h"
#include "debug.h"

const int Lorica::Config::SYS_PORT = -1;

#ifdef ACE_WIN32
static char*
get_ip_from_ifname(const int inet_family,
		   const char *ifname)
{
	return NULL;
}
#else
static char*
get_ip_from_ifname(const int inet_family,
		   const char *ifname)
{
	char *retv = NULL;
	struct ifaddrs *myaddrs;
	struct ifaddrs *ifa;
	struct sockaddr_in *s4;
	struct sockaddr_in6 *s6;
	char buf[64] = { '\0' }; 

	if (getifaddrs(&myaddrs))
		return NULL;

	switch (inet_family) {
	case AF_INET :
	case AF_INET6 :
		break;
	default :
		return NULL;
	}

	for (ifa = myaddrs; ifa != NULL; ifa = ifa->ifa_next) {
		if (NULL == ifa->ifa_addr)
			continue;
		if (0 == (ifa->ifa_flags & IFF_UP)) 
			continue;
		if (strcmp(ifname, ifa->ifa_name))
			continue;

		if (AF_INET == inet_family) {
			if (ifa->ifa_addr->sa_family == AF_INET) {
				s4 = (struct sockaddr_in *)(ifa->ifa_addr);
				if (inet_ntop(ifa->ifa_addr->sa_family, (void *)&(s4->sin_addr), buf, sizeof(buf)) == NULL) {
					break;
				} else {
					retv = strdup(buf);
					break;
				}
			}
		}
 
		if (AF_INET6 == inet_family) {
			if (ifa->ifa_addr->sa_family == AF_INET6) {
				s6 = (struct sockaddr_in6 *)(ifa->ifa_addr);
				if (inet_ntop(ifa->ifa_addr->sa_family, (void *)&(s6->sin6_addr), buf, sizeof(buf)) == NULL) {
					break;
				} else {
					retv = strdup(buf);
					break;
				}
			}
		}
	}
	freeifaddrs(myaddrs);
	
	return retv;
} 
#endif

Lorica::Config::Endpoint::Endpoint ()
	: external_(false),
	  port_(0),
	  is_ipv6_(false),
	  ssl_port_(0)
{
}

Lorica::Config::Endpoint::Endpoint(const Lorica::Config::Endpoint& a)
{
	operator= (a);
}

Lorica::Config::Endpoint&
Lorica::Config::Endpoint::operator=(const Lorica::Config::Endpoint& a)
{
	if (&a != this) {
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
Lorica::Config::Endpoint::operator!=(const Lorica::Config::Endpoint& a)
{
	return ((external_ != a.external_)
		|| (hostname_ != a.hostname_)
		|| (port_ != a.port_)
		|| (alias_ != a.alias_)
		|| (is_ipv6_ != is_ipv6_)
		|| (ssl_port_ != a.ssl_port_));
}
void
Lorica::Config::Endpoint::as_tao_endpoint(std::string &arg)
{
	arg += "iiop://";
	std::string options;

	if (this->is_ipv6_)
		arg += "[" + this->hostname_ + "]:";
	else
		arg += this->hostname_ + ":";

	if (SYS_PORT != this->port_) {
		ACE_TCHAR tmp[15];

		ACE_OS::itoa(this->port_, tmp, 10);
		arg += tmp;
	}

	if (this->ssl_port_) {
		ACE_TCHAR tmp[15];

		ACE_OS::itoa(this->ssl_port_, tmp, 10);
		options += "/ssl_port=";
		options += tmp;
	}

	if (!this->alias_.empty()) {
		options += options.empty() ? '/' : ',';
		options += "hostname_in_ior=" + this->alias_;
	}

	arg += options;
}

bool
Lorica::Config::Endpoint::parse_string(const std::string &ep_str,
				       const bool outside_facing)
{
	this->is_ipv6_ = false;
	this->ssl_port_ = 0;

	size_t option_pos = ep_str.find('/');
	std::string primary_addr = (option_pos != std::string::npos) ? ep_str.substr(0,option_pos) : ep_str;

	size_t port_sep = primary_addr.rfind(':');
	if (port_sep == std::string::npos) {
		ACE_ERROR((LM_ERROR,
			   "%N:%l - configured endpoint \"%s\" lacks port separator\n",
			   primary_addr.c_str()));
		return false;
	}

	this->port_ = static_cast<int> (ACE_OS::strtol(primary_addr.substr(port_sep+1).c_str(), 0, 10));
	if (0 >= this->port_) {
		if (outside_facing)
			this->port_ = LORICA_DEFAULT_OUTSIDE_FACING_PORT;
		else
			this->port_ = LORICA_DEFAULT_INSIDE_FACING_PORT;
	}

	size_t start = 0;
	size_t end = port_sep;

	// "[]:123" and ":123" are both accepted
	if (!port_sep) {
		this->hostname_ = "";
	} else if ((2 == port_sep)
		   && ((primary_addr[0] == '[') && (primary_addr[1] == ']'))) {
		this->is_ipv6_ = true;
		this->hostname_ = "";
	} else {
		if ((primary_addr[start] == '[') && (primary_addr[end-1] == ']')) {
			this->is_ipv6_ = true;
			++start;
			end -= 2;
		}
		this->hostname_ = primary_addr.substr(start, end);
	}

	// IPv4 - check for the interface name extension
	if (!this->hostname_.empty() && '%' == this->hostname_[0]) {
		std::string ifname = this->hostname_.substr(1);
		
		if (ifname.empty())
			this->hostname_ = "";
		else {
			char *ip_addr = get_ip_from_ifname(AF_INET, ifname.c_str());
			this->hostname_ = ip_addr ? "" : (const char*)ip_addr;
			if (ip_addr)
				free(ip_addr);
		}
	}

	// IPv6 - check for the interface name extension
	if (!this->hostname_.empty() && '+' == this->hostname_[0]) {
		std::string ifname = this->hostname_.substr(1);

		this->is_ipv6_ = true;
		
		if (ifname.empty())
			this->hostname_ = "";
		else {
			char *ip_addr = get_ip_from_ifname(AF_INET6, ifname.c_str());
			this->hostname_ = ip_addr ? "" : (const char*)ip_addr;
			if (ip_addr)
				free(ip_addr);
		}
	}

	// check for empty hostname and fix it up so that
	// we don't throw the MProfile exception
	if (this->hostname_.empty()) {
		char host_name[HOST_NAME_MAX] = { '\0' };

		ACE_OS::hostname(host_name, sizeof(host_name));
		this->hostname_ = (const char*)host_name;

		if (outside_facing)
			ACE_DEBUG((LM_WARNING, ACE_TEXT("%N:%l - DEPRECATED: Using default hostname for outside facing endpoint - %s\n"), this->hostname_.c_str()));
		else
			ACE_DEBUG((LM_WARNING, ACE_TEXT("%N:%l - DEPRECATED: Using default hostname for inside facing endpoint - %s\n"), this->hostname_.c_str()));
	}

	if (option_pos != std::string::npos) {
		// The rest are optional modifiers, ssl port, hostname alias
		++option_pos;
		std::string test("alias=");
		size_t opt = ep_str.find (test, option_pos);

		if (opt != std::string::npos) {
			size_t comma = ep_str.find (',',opt);
			size_t begin = opt + test.length();

			this->alias_ = (comma == std::string::npos) ? ep_str.substr(begin) : ep_str.substr(begin, comma - begin);
		}
		test = "ssl_port=";
		opt = ep_str.find (test, option_pos);

		if (opt != std::string::npos) {
			size_t comma = ep_str.find (',',opt);
			size_t begin = opt + test.length();

			std::string portstr = (comma == std::string::npos) ? ep_str.substr(begin) : ep_str.substr(begin, comma - begin);
			this->ssl_port_ = static_cast<int> (ACE_OS::strtol(portstr.c_str(), 0, 10));
			if (0 >= this->ssl_port_) {
				if (outside_facing)
					this->ssl_port_ = LORICA_DEFAULT_OUTSIDE_FACING_PORT_SEC;
				else
					this->ssl_port_ = LORICA_DEFAULT_INSIDE_FACING_PORT_SEC;
			}
		}
	}

	return true;
}

Lorica::Config::~Config(void)
{
}

ACE_ARGV*
Lorica::Config::get_orb_options_copy(void)
{
	ACE_ARGV *args;
	ACE_NEW_RETURN(args,
		       ACE_ARGV(),
		       0);
	ssize_t len = this->orb_args_.size();

	for (ssize_t count = 0; count < len; count++)
		args->add (this->orb_args_[count].c_str());

	return args;
}

ACE_ARGV*
Lorica::Config::get_ifr_options_copy(void)
{
	ACE_ARGV *args;
	ACE_NEW_RETURN(args,
		       ACE_ARGV(),
		       0);
	ssize_t len = this->ifr_args_.size();

	for (ssize_t count = 0; count < len; count++)
		args->add (this->ifr_args_[count].c_str());

	return args;
}

ACE_ARGV*
Lorica::Config::get_orb_options(void)
{
	if (orb_args_.size() > 0)
		return get_orb_options_copy ();

	// app name required to ensure proper argument alignment
	orb_args_.push_back(LORICA_EXE_NAME);

	// use shared profiles by default
	orb_args_.push_back("-ORBUseSharedProfile");
	orb_args_.push_back("1");

	std::string opts = this->get_value("ORB_Option");
	if (!opts.empty()) {
		size_t pos = 0;

		while (true) {
			size_t tpos = opts.find(' ', pos);
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
	if (this->endpoints_.empty()
	    && (!this->init_endpoints(false)
		|| !this->init_endpoints(true))) {
		endpoints_.clear();
		orb_args_.clear();
		ACE_ERROR_RETURN((LM_ERROR,
				  "%N:%l - Unable to parse endpoint configuration values\n"),
				 0);
	}

	for (size_t count = 0; count < this->endpoints_.size(); count++) {
		std::string tao_eparg;

		this->endpoints_[count].as_tao_endpoint(tao_eparg);
		this->orb_args_.push_back("-ORBListenEndpoints");
		this->orb_args_.push_back(tao_eparg.c_str());
	}

	if (this->debug_level() > 0) {
		ACE_TCHAR tmp[15];

		orb_args_.push_back ("-ORBDebuglevel");
		ACE_OS::itoa(this->debug_level(), tmp, 10);
		orb_args_.push_back (tmp);
	}

	return get_orb_options_copy ();
}

ACE_ARGV*
Lorica::Config::get_ifr_options(const bool Debug)
{
	if (ifr_args_.size() > 0)
		return get_ifr_options_copy();

	// app name required to ensure proper argument alignment
	ifr_args_.push_back(IFR_SERVICE_EXE_NAME);

	this->ifr_args_.push_back("-o");
	std::string opt = this->get_value("IFR_IOR_FILE");
	if (!opt.length()) {
#ifdef ACE_WIN32
		opt = IFR_SERVICE_IOR_FILE;
#else
		if (Debug)
			opt = "ifr.ior";
		else
			opt = IFR_SERVICE_IOR_FILE;
#endif
	}
	this->ifr_args_.push_back(opt.c_str());

	opt = this->get_value("IFR_PERSISTENT");
	if ("no" != opt) {
		this->ifr_args_.push_back("-p");

		this->ifr_args_.push_back("-b");
		opt = this->get_value("IFR_CACHE");
		if (!opt.length()) {
#ifdef ACE_WIN32
			opt = IFR_SERVICE_CACHE_FILE;
#else
			if (Debug)
				opt = "ifr.cache";
			else
				opt = IFR_SERVICE_CACHE_FILE;
#endif
		}
		this->ifr_args_.push_back(opt.c_str());
	}

	// command line options which will override the defaults
	std::string cmd_opts = this->get_value("IFR_CmdOption");
	if (!cmd_opts.empty()) {
		size_t pos = 0;

		while (true) {
			size_t tpos = cmd_opts.find(' ', pos);
			std::string substr;

			if (tpos == std::string::npos)
				substr = cmd_opts.substr(pos);
			else
				substr = cmd_opts.substr(pos, tpos-pos);

			if (!substr.empty())
				this->ifr_args_.push_back(substr.c_str());

			if (tpos == std::string::npos)
				break;

			pos = tpos+1;
		}
	}

	return get_ifr_options_copy();
}


bool
Lorica::Config::init_endpoints(bool do_extern)
{
	std::string eps_str = do_extern ? this->get_value("External_Address") : this->get_value("Internal_Address");
	size_t pos = 0;
	size_t tpos = 0;

	if (eps_str.empty() || !eps_str.length()) {
		if (do_extern) {
			char host_name[HOST_NAME_MAX] = { '\0' };
			ACE_OS::hostname(host_name, sizeof(host_name));

			eps_str = (const char*)host_name;
			eps_str += ":"LORICA_DEFAULT_OUTSIDE_FACING_PORT_STR;
			ACE_DEBUG((LM_WARNING, ACE_TEXT("%N:%l - DEPRECATED: Using default endpoint value (%s) for outside facing endpoint\n"), eps_str.c_str()));
		} else {
			eps_str = "localhost:"LORICA_DEFAULT_INSIDE_FACING_PORT_STR;
			ACE_DEBUG((LM_WARNING, ACE_TEXT("%N:%l - DEPRECATED: Using default endpoint value (%s) for inside facing endpoint\n"), eps_str.c_str()));
		}
	}

	for (; tpos != std::string::npos; pos = tpos + 1) {
		tpos = eps_str.find(' ', pos);
		std::string ep_str = (tpos == std::string::npos) ? eps_str.substr(pos) : eps_str.substr(pos, tpos-pos);
		if (ep_str.empty())
			break;

		if (do_extern)
			ACE_DEBUG((LM_DEBUG, ACE_TEXT("%N:%l - External Endpoint String - %s\n"), ep_str.c_str()));
		else
			ACE_DEBUG((LM_DEBUG, ACE_TEXT("%N:%l - Internal Endpoint String - %s\n"), ep_str.c_str()));

		Endpoint ep;
		ep.external_ = do_extern;
		if (ep.parse_string(ep_str, do_extern))
			endpoints_.push_back(ep);
		else
			return false;
	}

	return true;
}

Lorica::Config::Endpoints
Lorica::Config::get_endpoints(bool ext)
{
	Endpoints eps;

	if (this->endpoints_.empty()
	    && (!this->init_endpoints(false)
		|| !this->init_endpoints(true))) {
		this->endpoints_.clear();
		return eps;
	}

	for (Endpoints::const_iterator iter = endpoints_.begin(); iter != endpoints_.end(); iter++) {
		if (iter->external_ == ext)
			eps.push_back (*iter);
	}

	return eps;
}

bool
Lorica::Config::null_eval_any (void) const
{
	// First check if a NULL evaluator ID list has been provided.
	std::string ids = this->null_eval_type_ids();
	if (!ids.empty())
		return false;

	std::string eval_any = this->get_value ("Null_Evaluator_Any");

	return (0 == eval_any.compare("yes"));
}

std::string
Lorica::Config::null_eval_type_ids (void) const
{
	return this->get_value ("Null_Eval_Type_ID");
}

bool
Lorica::Config::generic_evaluator (void) const
{
	std::string gen_eval = this->get_value("Generic_Evaluator");

	return (0 == gen_eval.compare("yes"));
}

bool
Lorica::Config::collocate_ifr (void) const
{
	std::string coll_ifr = this->get_value("Collocate_IFR");

	return (0 != coll_ifr.compare("no"));
}

bool
Lorica::Config::secure_available (void) const
{
	return this->secure_available_;
}

void
Lorica::Config::secure_available(bool sa)
{
	this->secure_available_ = sa;
}
