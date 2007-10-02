/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 *    Lorica header file. 
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

#ifndef REFERENCE_MAP_VALUE_H
#define REFERENCE_MAP_VALUE_H

#include <string>

#include "lorica/ServerAgentC.h"
#include "lorica/Lorica_EvaluatorBase_Export.h"

namespace Lorica
{
	class Lorica_EvaluatorBase_Export ReferenceMapValue
	{
	public:
		ReferenceMapValue (void);
		~ReferenceMapValue (void);
		CORBA::Object_var orig_ref_;
		CORBA::Object_var mapped_ref_;
		std::string ior_table_name_;
		bool        require_secure_;
		Lorica::ServerAgent_var agent_;

		ReferenceMapValue *reverse(void);

		void incr_refcount();
		long decr_refcount();

		long refcount() const;

	private:
		long ref_count_;
	};

	class Lorica_EvaluatorBase_Export ReferenceMapValue_var
	{
	public:
		ReferenceMapValue_var (void);
		ReferenceMapValue_var (ReferenceMapValue *rmv);
		ReferenceMapValue_var (const ReferenceMapValue_var &rmv);
		~ReferenceMapValue_var (void);

		ReferenceMapValue_var &operator= (const ReferenceMapValue_var &rmv);

		void set (ReferenceMapValue *rmv);
		ReferenceMapValue* get (void) const;

		const ReferenceMapValue* in (void) const;
		ReferenceMapValue*& out (void);
		ReferenceMapValue*& inout (void);

		ReferenceMapValue* release (void);

		ReferenceMapValue_var &operator= (ReferenceMapValue *rmv);
		const ReferenceMapValue * operator->() const;
		ReferenceMapValue * operator->();

	private:
		ReferenceMapValue *ptr_;
	};
}

#endif // REFERENCE_MAP_VALUE_H
