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

#ifndef _BUILD_SYSTEM_DEFS_H_
#define _BUILD_SYSTEM_DEFS_H_

// Using GNU autotools
#ifdef HAVE_CONFIG_H
#include "config.h"
#include "defines/pathdefs.h"
#endif

// Using DOC MPC build system
#ifdef LORICA_HAVE_DOC_MPC
#include "defines/hardcoded-defs.h"
#else
#include "defines/hardcoded-defs.h"
#endif


#ifdef ACE_WIN32
# ifndef LORICA_LACKS_IFADDRS
#  define LORICA_LACKS_IFADDRS (1)
# endif
#endif


#endif // _BUILD_SYSTEM_DEFS_H_
