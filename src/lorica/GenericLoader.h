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

#ifndef GENERIC_LOADER_H
#define GENERIC_LOADER_H

#include "lorica/EvaluatorLoader.h"
#include "lorica/Lorica_GenericEvaluator_Export.h"

class Lorica_GenericEvaluator_Export  Lorica_GenericLoader :
public Lorica_EvaluatorLoader
{
public:
	Lorica_GenericLoader ();
	virtual ~Lorica_GenericLoader();

	/// Shared object initializer
	int init (int argc, ACE_TCHAR *argv[]);

	/// Shared object finalizer
	int fini (void);
};

ACE_STATIC_SVC_DECLARE_EXPORT (Lorica_GenericEvaluator, Lorica_GenericLoader)
ACE_FACTORY_DECLARE (Lorica_GenericEvaluator, Lorica_GenericLoader)

#endif // GENERIC_LOADER_H
