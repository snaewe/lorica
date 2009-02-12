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

#include "TestServer_impl.h"
#include <ace/OS_NS_unistd.h>
#include <tao/debug.h>

#include "testC.h"

TestServer_impl::TestServer_impl(CORBA::ORB_ptr orb) : orb_(CORBA::ORB::_duplicate(orb)) {
	//NOP
}

void TestServer_impl::Restrict(UnionTest::RestrictionContainer_ptr restriction_container) {
	ACE_DEBUG((LM_DEBUG,"Server::Accessing union received from client\n"));
	ACE_DEBUG((LM_DEBUG,"Server::Message read from union => %s\n", restriction_container->content()->comment()->message()));
}

void TestServer_impl::shutdown() {
	ACE_OS::sleep(2);
	this->orb_->shutdown(0);
}
