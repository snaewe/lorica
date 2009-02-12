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

#ifndef REFERENCE_MAP_VALUE_H
#define REFERENCE_MAP_VALUE_H

#include <string>

#include "ServerAgentC.h"
#include "Lorica_EvaluatorBase_Export.h"

namespace Lorica
{
	class Lorica_EvaluatorBase_Export ReferenceMapValue {
	public:
		ReferenceMapValue(void);

		~ReferenceMapValue(void);

		ReferenceMapValue *reverse(void);

		void incr_refcount(void);

		long decr_refcount(void);

		long refcount(void) const;

		CORBA::Object_var orig_ref_;
		CORBA::Object_var mapped_ref_;
		std::string ior_table_name_;
		bool require_secure_;
		Lorica::ServerAgent_var agent_;

	private:
		long ref_count_;
	};

	class Lorica_EvaluatorBase_Export ReferenceMapValue_var {
	public:
		ReferenceMapValue_var(void);

		ReferenceMapValue_var(ReferenceMapValue *rmv);

		ReferenceMapValue_var(const ReferenceMapValue_var &rmv);

		~ReferenceMapValue_var(void);

		ReferenceMapValue_var &operator= (const ReferenceMapValue_var & rmv);

		void set(ReferenceMapValue *rmv);

		ReferenceMapValue *get(void) const;

		const ReferenceMapValue *in(void) const;

		ReferenceMapValue *& out(void);

		ReferenceMapValue *& inout(void);

		ReferenceMapValue *release(void);

		ReferenceMapValue_var &operator= (ReferenceMapValue *rmv);

		const ReferenceMapValue *operator->(void) const;

		ReferenceMapValue *operator->(void);

	private:
		ReferenceMapValue *ptr_;
	};
}

#endif // REFERENCE_MAP_VALUE_H
