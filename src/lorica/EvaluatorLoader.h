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

#ifndef EVALUATOR_LOADER_H
#define EVALUATOR_LOADER_H

#include "ace/Service_Config.h"
#include "ace/Service_Object.h"
#include "tao/PortableServer/POAManagerC.h"
#include "Lorica_EvaluatorBase_Export.h"

class Lorica_MapperRegistry;

class Lorica_EvaluatorBase_Export  Lorica_EvaluatorLoader :
public ACE_Service_Object
{
public:
	Lorica_EvaluatorLoader ();
	virtual ~Lorica_EvaluatorLoader();

	/// Shared object initializer
	int init (int argc, ACE_TCHAR *argv[]);

	/// Shared object finalizer
	int fini (void);

protected:
	Lorica_MapperRegistry *mapperRegistry_;
};

ACE_STATIC_SVC_DECLARE_EXPORT (Lorica_EvaluatorBase, Lorica_EvaluatorLoader)
ACE_FACTORY_DECLARE (Lorica_EvaluatorBase, Lorica_EvaluatorLoader)

#endif // EVALUATOR_LOADER_H
