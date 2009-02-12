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

#ifndef UNBIND_H
#define UNBIND_H
#include /**/ "ace/pre.h"

#include "TestS.h"

/// Implement the Test::First interface
class First : public virtual POA_Test::First, public virtual PortableServer::RefCountServantBase {
public:
	virtual void method(void);
};

/// Implement the Test::Second interface
class Second : public virtual POA_Test::Second, public virtual PortableServer::RefCountServantBase {
public:
	virtual void method(void);
};

/// Implement the Test::Third interface
class Third : public virtual POA_Test::Third, public virtual PortableServer::RefCountServantBase {
public:
	virtual void method(void);
};

#include /**/ "ace/post.h"
#endif /* UNBIND_H */
