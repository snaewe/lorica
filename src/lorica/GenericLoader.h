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

#ifndef GENERIC_LOADER_H
#define GENERIC_LOADER_H

#include "EvaluatorLoader.h"
#include "Lorica_GenericEvaluator_Export.h"

class Lorica_GenericEvaluator_Export Lorica_GenericLoader : public Lorica_EvaluatorLoader {
public:
	Lorica_GenericLoader(void);
	virtual ~Lorica_GenericLoader(void);

	// Shared object initializer
	int init(int argc, ACE_TCHAR *argv[]);

	// Shared object finalizer
	int fini(void);
};

ACE_STATIC_SVC_DECLARE_EXPORT(Lorica_GenericEvaluator, Lorica_GenericLoader)
ACE_FACTORY_DECLARE(Lorica_GenericEvaluator, Lorica_GenericLoader)

#endif // GENERIC_LOADER_H
