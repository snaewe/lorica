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

#include "ProxyMapper.h"
#include "EvaluatorBase.h"

Lorica::EvaluatorBase::EvaluatorBase (ProxyMapper &pm)
	: mapper_(pm),
	  id_(),
	  next_ (0)
{
}

Lorica::EvaluatorBase::EvaluatorBase (const std::string & typeId,
				      ProxyMapper &pm)
	: mapper_(pm),
	  id_(typeId),
	  next_ (0)
{
}

Lorica::EvaluatorBase::~EvaluatorBase (void)
{
	delete this->next_;
}

void
Lorica::EvaluatorBase::add_evaluator (EvaluatorBase *eb)
{
	if (this->next_ == 0)
		this->next_ = eb;
	else
		this->next_->add_evaluator(eb);
}

Lorica::EvaluatorBase *
Lorica::EvaluatorBase::find_evaluator (const std::string &typeId)
{
	if (this->id_ == typeId)
		return this;
	if (this->next_ == 0)
		return 0;
	return this->next_->find_evaluator(typeId);
}

const std::string &
Lorica::EvaluatorBase::type_id (void) const
{
	return this->id_;
}

bool
Lorica::EvaluatorBase::evaluate_request (const char * ,
					 PortableServer::POA_ptr ,
					 CORBA::ServerRequest_ptr ,
					 CORBA::NVList_ptr,
					 CORBA::NVList_ptr &,
					 CORBA::NamedValue_ptr &result) const
{
	result = 0;
	return true;
}

bool
Lorica::EvaluatorBase::evaluate_reply (const char *,
				       PortableServer::POA_ptr ,
				       CORBA::NVList_ptr,
				       CORBA::NamedValue_ptr ) const
{
	return true;
}

bool
Lorica::EvaluatorBase::evaluate_exception (const char *,
					   PortableServer::POA_ptr ,
					   const char *id,
					   TAO_InputCDR &incoming,
					   TAO_OutputCDR &encap ) const
{
	const ACE_Message_Block *buffer = incoming.start();
	encap << id;
	encap.write_octet_array_mb (buffer);
	return true;

}
