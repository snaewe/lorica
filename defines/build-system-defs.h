/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 *    Lorica header file.
 *    Copyright (C) 2008 OMC Denmark ApS.
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
#endif

// Hardcoded fallback values
#include "defines/hardcoded-defs.h"

#endif // _BUILD_SYSTEM_DEFS_H_
