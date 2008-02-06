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

#include <ace/Dynamic_Service.h>

#include "GenericLoader.h"
#include "GenericMapper.h"
#include "MapperRegistry.h"

Lorica_GenericLoader::Lorica_GenericLoader(void)
{
}

Lorica_GenericLoader::~Lorica_GenericLoader(void)
{
}

int
Lorica_GenericLoader::init(int argc,
			   ACE_TCHAR *argv[])
{
	this->Lorica_EvaluatorLoader::init(argc, argv);

	Lorica::ProxyMapper *mapper = new Lorica::GenericMapper(false, *this->mapperRegistry_);

	this->mapperRegistry_->set_generic_mapper(mapper);

	return 1;
}

// Shared object finalizer
int
Lorica_GenericLoader::fini(void)
{
	return this->Lorica_EvaluatorLoader::fini();
}

ACE_STATIC_SVC_DEFINE(Lorica_GenericLoader,
		      ACE_TEXT ("Lorica_GenericLoader"),
		      ACE_SVC_OBJ_T,
		      &ACE_SVC_NAME (Lorica_GenericLoader),
		      ACE_Service_Type::DELETE_THIS | ACE_Service_Type::DELETE_OBJ,
		      0)

ACE_FACTORY_DEFINE(Lorica_GenericEvaluator, Lorica_GenericLoader)
