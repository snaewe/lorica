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

#ifndef _WIN_DEFS_H_
#define _WIN_DEFS_H_

#ifdef ACE_WIN32

#define LORICA_DEFAULT_INSIDE_FACING_PORT (4080)     // Default Lorica inside facing port
#define LORICA_DEFAULT_INSIDE_FACING_PORT_STR "4080" // Default Lorica inside facing port (stringified)

#define LORICA_DEFAULT_INSIDE_FACING_PORT_SEC (4081)     // Default Lorica inside facing secure port
#define LORICA_DEFAULT_INSIDE_FACING_PORT_SEC_STR "4081" // Default Lorica inside facing secure port (stringified)

#define LORICA_DEFAULT_OUTSIDE_FACING_PORT (4082)     // Default Lorica outside facing port
#define LORICA_DEFAULT_OUTSIDE_FACING_PORT_STR "4082" // Default Lorica outside facing port (stringified)

#define LORICA_DEFAULT_OUTSIDE_FACING_PORT_SEC (4083)     // Default Lorica outside facing secure port
#define LORICA_DEFAULT_OUTSIDE_FACING_PORT_SEC_STR "4083" // Default Lorica outside facing secure port (stringified)

#define LORICA_EXE_NAME        "lorica"
#define LORICA_IOR_FILE        "lorica.ior"
#define LORICA_PID_FILE        "lorica.pid"
#define LORICA_CONF_FILE       "lorica.conf"
#define IFR_SERVICE_IOR_FILE   "ifr.ior"
#define IFR_SERVICE_CACHE_FILE "ifr.cache"
#define IFR_SERVICE_EXE_NAME   "ifr_service"

#endif // ACE_WIN32

#endif // _WIN_DEFS_H_
