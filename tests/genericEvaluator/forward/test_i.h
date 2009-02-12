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

#ifndef TEST_I_H
#define TEST_I_H

#include "testS.h"

class Forward_Test_i : public POA_Forward::Test
{
	// = TITLE
	//   Location Forward exception tester
	//
	// = DESCRIPTION
	//   servant throws a location forward exception when constructed with a
	//   target IOR, otherwise it echos the source string.

public:
	Forward_Test_i (CORBA::ORB_ptr orb);

  char * do_forward (const char *text);
	void shutdown ();

private:
	CORBA::ORB_var orb_;
};

#endif /* TEST_I_H */
