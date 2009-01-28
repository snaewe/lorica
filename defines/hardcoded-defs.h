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

#ifndef _HARD_CODED_DEFS_H_
#define _HARD_CODED_DEFS_H_

#ifndef LORICA_DEFAULT_INSIDE_FACING_PORT // Default Lorica inside facing port
#define LORICA_DEFAULT_INSIDE_FACING_PORT (4080) 
#endif

#ifndef LORICA_DEFAULT_INSIDE_FACING_PORT_STR // Default Lorica inside facing port (stringified)
#define LORICA_DEFAULT_INSIDE_FACING_PORT_STR "4080" 
#endif

#ifndef LORICA_DEFAULT_INSIDE_FACING_PORT_SEC // Default Lorica inside facing secure port
#define LORICA_DEFAULT_INSIDE_FACING_PORT_SEC (4081) 
#endif

#ifndef LORICA_DEFAULT_INSIDE_FACING_PORT_SEC_STR // Default Lorica inside facing secure port (stringified)
#define LORICA_DEFAULT_INSIDE_FACING_PORT_SEC_STR "4081" 
#endif

#ifndef LORICA_DEFAULT_OUTSIDE_FACING_PORT // Default Lorica outside facing port
#define LORICA_DEFAULT_OUTSIDE_FACING_PORT (4082)     
#endif

#ifndef LORICA_DEFAULT_OUTSIDE_FACING_PORT_STR // Default Lorica outside facing port (stringified)
#define LORICA_DEFAULT_OUTSIDE_FACING_PORT_STR "4082" 
#endif

#ifndef LORICA_DEFAULT_OUTSIDE_FACING_PORT_SEC // Default Lorica outside facing secure port
#define LORICA_DEFAULT_OUTSIDE_FACING_PORT_SEC (4083)     
#endif

#ifndef LORICA_DEFAULT_OUTSIDE_FACING_PORT_SEC_STR // Default Lorica outside facing secure port (stringified)
#define LORICA_DEFAULT_OUTSIDE_FACING_PORT_SEC_STR "4083" 
#endif

#ifndef LORICA_EXE_NAME // Full path of Lorica executable
#define LORICA_EXE_NAME "lorica.exe"
#endif

#ifndef LORICA_CACHE_DIR // Full path of Lorica cache directory
#define LORICA_CACHE_DIR "."
#endif

#ifndef LORICA_IOR_FILE // Full path of RefernceMapper IOR file
#define LORICA_IOR_FILE "lorica.ior"
#endif

#ifndef LORICA_PID_FILE // Full path of UNIX PID file
#define LORICA_PID_FILE "lorica.pid"
#endif

#ifndef LORICA_CONF_FILE // Full path of configuration file
#define LORICA_CONF_FILE "lorica.conf"
#endif

#ifndef IFR_SERVICE_IOR_FILE // Full path of IFR IOR file
#define IFR_SERVICE_IOR_FILE "ifr.ior"
#endif

#ifndef IFR_SERVICE_CACHE_FILE // Full path of IFR cache file
#define IFR_SERVICE_CACHE_FILE "ifr.cache"
#endif

#ifndef IFR_SERVICE_EXE_NAME // Full path of IFR_Service executable
#define IFR_SERVICE_EXE_NAME "IFR_Service.exe"
#endif

#ifndef VERSION
#define VERSION "1.0.11"
#endif

#endif // _HARD_CODED_DEFS_H_
