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

#ifndef _CONFIG_BASE_
#define _CONFIG_BASE_

#include <string>
#include <vector>
#include <ace/ARGV.h>

#include "Lorica_EvaluatorBase_Export.h"

namespace Lorica
{
	/**
	 *  Configuration interface
	 */

	class Lorica_EvaluatorBase_Export Config {
	public:
		struct Lorica_EvaluatorBase_Export  Endpoint
		{
			bool external_;
			std::string hostname_;
			int port_;
			std::string alias_;
			bool is_ipv6_;
			int ssl_port_;

			Endpoint ();
			Endpoint (const Endpoint& a);
			Endpoint& operator= (const Endpoint& a);

			bool operator!= (const Endpoint& a);

			bool parse_string (const std::string &ep_str,
					   const bool outside_facing);

			void as_tao_endpoint (std::string &arg);
		};
		typedef std::vector<Endpoint> Endpoints;

		static const int SYS_PORT;

		virtual ~Config(void);

		/// Get associated configuration option.
		virtual std::string get_value(const std::string& token) const = 0;
		virtual bool getBooleanValue(const std::string& token, bool default_flag) = 0;

		void secure_available(bool sa);
		bool secure_available(void) const;

		/**
		 * Constructs and returns a pointer to an ACE_ARGV object
		 *
		 * @note It is the users responsibility to free the returned ACE_ARGV
		 *       object. User should not free any data internal to ACE_ARGV.
		 *
		 * @retval 0 on failure. Heap allocated ACE_ARGV structure upon success.
		 */
		ACE_ARGV* get_orb_options(void);
		ACE_ARGV* get_ifr_options(const bool Debug);

		/**
		 * Returns externally directed endpoints.
		 *
		 * @external 'true' == external endpoints. 'false' otherwise
		 */
		Endpoints get_endpoints(bool external);

		/**
		 * Should the null evaluator handle any unknown type?
		 *
		 * @note This is preceded by string returned by null_eval_ids()
		 *
		 * @retval true == null evaluator should handle unknown types
		 *         else false.
		 */
		bool null_eval_any(void) const;

		/**
		 * Returns Repository ID's to be handled by the
		 * NULL Evaluator.
		 */
		std::string null_eval_type_ids(void) const;

		/**
		 * Include generic evaluator
		 *
		 * @retval true == include generic evaluator, else false
		 */
		bool generic_evaluator(void) const;

		/**
		 * Explicitly collocate the IFR?
		 *
		 * @retval true == please collocate the IFR with the generic evaluator.
		 */
		bool collocate_ifr(void) const;


		/**
		 * Returns the current debug level (0 - 10)
		 *
		 */
		virtual int debug_level(void) = 0;

	private:
		bool init_endpoints (bool do_extern);

		ACE_ARGV *get_orb_options_copy(void);
		ACE_ARGV *get_ifr_options_copy(void);

		Endpoints endpoints_;
		std::vector<std::string> orb_args_;
		std::vector<std::string> ifr_args_;

		bool secure_available_;

		// We need an iterator here for general traversal.
	};
} /* namespace Lorica */

#endif // _CONFIG_BASE_
