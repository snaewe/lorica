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

#include "ace/Dynamic_Service.h"
#include "MapperRegistry.h"

#include "EvaluatorLoader.h"

Lorica_EvaluatorLoader::Lorica_EvaluatorLoader ()
{
}

Lorica_EvaluatorLoader::~Lorica_EvaluatorLoader()
{
}

int
Lorica_EvaluatorLoader::init (int, ACE_TCHAR *[] )
{

	this->mapperRegistry_ =
		ACE_Dynamic_Service<Lorica_MapperRegistry>::instance("MapperRegistry");

	return 1;
}

/// Shared object finalizer
int
Lorica_EvaluatorLoader::fini (void)
{
	return 1;
}
