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

#ifndef COMMENT_RESTRICTION_IMPL_H
#define COMMENT_RESTRICTION_IMPL_H

#include "testS.h"

/**
 * Implements the UnionTest::CommentRestriction interface
 */
class CommentRestriction_impl : public POA_UnionTest::CommentRestriction, public virtual PortableServer::RefCountServantBase {
public:
	CommentRestriction_impl(CORBA::String_var message);
	char* message();

private:
	CORBA::String_var message_;
};

#endif /* COMMENT_RESTRICTION_IMPL_H */
