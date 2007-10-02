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

#ifndef RMV_BY_NATIVE_H
#define RMV_BY_NATIVE_H

#include "ace/Mutex.h"

#include "lorica/Lorica_EvaluatorBase_Export.h"
#include "lorica/ReferenceMapValue.h"
#include "ace/RB_Tree.h"

namespace Lorica
{

	class Lorica_EvaluatorBase_Export RMVByNative
	{
	public:
		RMVByNative ();
		virtual ~RMVByNative (void);

		bool bind (CORBA::Object_ptr key, ReferenceMapValue *value);

		bool rebind (CORBA::Object_ptr key, ReferenceMapValue *value);

		bool find (CORBA::Object_ptr key, ReferenceMapValue *&value);

		bool unbind (CORBA::Object_ptr key, ReferenceMapValue *&value);


		class ObjRefLessThan
		{
		public:
			int operator () (const CORBA::Object_ptr lhs,
					 const CORBA::Object_ptr rhs) const;
		};

	private:

		typedef ACE_RB_Tree<CORBA::Object_ptr,
			ReferenceMapValue *,
			ObjRefLessThan,
			ACE_Thread_Mutex> Table;

		typedef ACE_RB_Tree_Node <CORBA::Object_ptr,
			ReferenceMapValue> TableEntry;

		Table table_;
	};
}

#endif // RMV_BY_NATIVE_H
