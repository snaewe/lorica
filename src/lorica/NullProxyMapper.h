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

#ifndef NULL_PROXY_MAPPER_H
#define NULL_PROXY_MAPPER_H

#include "ProxyMapper.h"
#include <string>
#include <vector>

#include "Lorica_EvaluatorBase_Export.h"

namespace Lorica
{
	class EvaluatorBase;

class Lorica_EvaluatorBase_Export NullProxyMapper : public ProxyMapper
	{
	public:
		NullProxyMapper (Lorica_MapperRegistry &);
		virtual ~NullProxyMapper (void);

		void add_type_id (const std::string &typeId);

		/// Returns the evaluator associated with a given type ID. Returns null
		/// if the type is unknown.
		virtual EvaluatorBase * evaluator_for (const std::string &typeId);

	protected:
		virtual ProxyServant * make_default_servant (void);

	private:
		/// a linked list of evaluators known by this mapper
		typedef std::vector<std::string> ListType;
		ListType typeIdList;

	};
}

#endif // NULL_PROXY_MAPPER_H
