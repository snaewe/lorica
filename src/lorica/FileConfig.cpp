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

#include "FileConfig.h"

#include <ace/ACE.h>
#include <ace/Log_Msg.h>
#include <ace/OS_NS_strings.h>
#include <fstream>

Lorica::FileConfig::FileConfig(void)
	: debug_level_(0),
	  COMMENT_CHAR('#')
{
}

Lorica::FileConfig *
Lorica::FileConfig::instance(void)
{
	return FILECONFIG::instance();
}

void
Lorica::FileConfig::init(const std::string & file_name,
			 int debug_level)
	throw (InitError)
{
	file_name_ = file_name;
	debug_level_ = debug_level;

	if (!this->load())
		throw InitError();
}

Lorica::FileConfig::~FileConfig(void)
{
	this->unload();
}

int
Lorica::FileConfig::debug_level(void)
{
	return debug_level_;
}

bool
Lorica::FileConfig::load(void)
{
	if (file_name_.empty())
		ACE_ERROR_RETURN((LM_ERROR,
				  "FileConfig::load: file_name_ is empty\n"),
				 false);
	this->unload();

	std::ifstream config_file(file_name_.c_str());
	if (!config_file)
		ACE_ERROR_RETURN((LM_ERROR,
				  "FileConfig::load: could not open file %s\n",
				  file_name_.c_str()),
				 false);

	std::string line;
	std::string::size_type pos;
	while (!config_file.eof()) {
		std::getline(config_file, line);
		if (line.empty())
			continue;

		// handle comments
		if (COMMENT_CHAR == line[0])
			continue;
		pos = line.find(COMMENT_CHAR, 0);
		if (std::string::npos != pos)
			line.erase(pos);
		if (line.empty())
			continue;

		// correct for CRLF and other whitespace issues
		pos = 0;
		while (true) {
			pos = line.find('\r', pos);
			if (std::string::npos == pos)
				break;
			line[pos] = ' ';
		}
		pos = 0;
		while (true) {
			pos = line.find('\t', pos);
			if (std::string::npos == pos)
				break;
			line[pos] = ' ';
		}

		pos = line.find(' ', 0); // "token cur_val"
		if (std::string::npos == pos)
			continue;

		std::string token = line.substr(0, pos); // "token"
		if ((pos+2) > line.length())
			continue;
		std::string cur_val = line.substr(pos+1); // "cur_val"

		// get any value currently assocuated with this token.
		std::string accru_val = this->get_value(token);

		// Add new value in.
		if (accru_val.empty())
			accru_val = cur_val;
		else
			accru_val += " " + cur_val;

		this->insert (token, accru_val);
	}

	return true;
}

bool
Lorica::FileConfig::unload(void)
{
	configs_.clear();

	return true;
}

std::string
Lorica::FileConfig::get_value(const std::string & token) const
{
	return this->extract(token);
}

bool
Lorica::FileConfig::getBooleanValue(const std::string & token,
				    bool default_flag)
{
	std::string result_string = this->get_value(token);
	bool result = default_flag;

	if (ACE_OS::strcasecmp(result_string.c_str(), "yes")
	    || ACE_OS::strcasecmp(result_string.c_str(), "true"))
		result = true;
	else if (ACE_OS::strcasecmp(result_string.c_str(), "no")
		 || ACE_OS::strcasecmp(result_string.c_str(), "false"))
		result = false;

	return result;
}

bool
Lorica::FileConfig::insert(const std::string & token,
			   const std::string & value)
{
	configs_[ACE::hash_pjw(token.c_str())] = value.c_str();

	return true;
}

std::string
Lorica::FileConfig::extract(const std::string& token) const
{
	std::string value;

	// voodoo :-(
	if (configs_.find(ACE::hash_pjw(token.c_str())) != configs_.end())
		value = configs_.find(ACE::hash_pjw(token.c_str()))->second;

	return value;
}
