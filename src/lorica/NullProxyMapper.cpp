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

#include "defines/build-system-defs.h"

#include "NullProxyMapper.h"
#include "EvaluatorBase.h"
#include "ProxyServant.h"

Lorica::NullProxyMapper::NullProxyMapper(Lorica_MapperRegistry & mr)
	:Lorica::ProxyMapper(mr, "_lorica_null"),
	 typeIdList()
{
	this->add_evaluator(new Lorica::EvaluatorBase(*this));
}

Lorica::NullProxyMapper::~NullProxyMapper(void)
{
}

void
Lorica::NullProxyMapper::add_type_id(const std::string & typeId)
{
	this->typeIdList.push_back(typeId);
}

Lorica::EvaluatorBase *
Lorica::NullProxyMapper::evaluator_for(const std::string & typeId)
{
	if (typeIdList.size() == 0)
		return this->evaluator_head_;

	for (ListType::const_iterator iter = typeIdList.begin(); iter != typeIdList.end(); iter++) {
		if (typeId == *iter)
			return this->evaluator_head_;
	}

	return 0;
}

Lorica::ProxyServant *
Lorica::NullProxyMapper::make_default_servant(void)
{
	return new Lorica::ProxyServant(*this);
}
