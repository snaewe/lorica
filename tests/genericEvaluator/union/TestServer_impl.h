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

#ifndef TEST_SERVER_IMPL_H
#define TEST_SERVER_IMPL_H

#include "testS.h"

/**
 * Implements the UnionTest::Server interface
 * The server recives a reference to a client object, which contains a method
 * which has a union as a out paramter. The server execute this method and read
 * the value of the union.
 */
class TestServer_impl : public POA_UnionTest::TestServer, public virtual PortableServer::RefCountServantBase {
public:
	TestServer_impl(CORBA::ORB_ptr orb);
	void Restrict(UnionTest::RestrictionContainer_ptr restriction_container);
	void shutdown();

private:
	CORBA::ORB_var orb_;
};

#endif /* TEST_SERVER_IMPL_H */
