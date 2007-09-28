/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 *    Lorica source file. 
 *    Copyright (C) 2007 OMC Denmark ApS.
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

#include "ace/Mutex.h"
#include "lorica/RMVByNative.h"

#include "tao/Stub.h"
#include "tao/MProfile.h"
#include "tao/Profile.h"
#include "ace/Min_Max.h"

Lorica::RMVByNative::RMVByNative ()
{
}

Lorica::RMVByNative::~RMVByNative (void)
{
}

bool
Lorica::RMVByNative::bind (CORBA::Object_ptr key, ReferenceMapValue *value)
{
	value->incr_refcount();
	CORBA::Object_ptr dupkey = CORBA::Object::_duplicate(key);
	int result = table_.bind(key,value);
	if (result != 0)
	{
		value->decr_refcount();
		CORBA::release(dupkey);
	}
	return result == 0;
}

bool
Lorica::RMVByNative::rebind (CORBA::Object_ptr key, ReferenceMapValue *value)
{
	value->incr_refcount();
	ReferenceMapValue_var old;
	int result = table_.rebind(key,value,old.out());
	if (result == -1)
		value->decr_refcount();
	return result != -1;
}

bool
Lorica::RMVByNative::find (CORBA::Object_ptr key, ReferenceMapValue *&value)
{
	int result = table_.find(key,value);
	if (result == 0)
	{
		value->incr_refcount();
		return true;
	}
	return false;
}

bool
Lorica::RMVByNative::unbind (CORBA::Object_ptr key, ReferenceMapValue *&value)
{
	return table_.unbind(key,value) == 0;
}


int
Lorica::RMVByNative::ObjRefLessThan::operator ()
	(const CORBA::Object_ptr lhs, const CORBA::Object_ptr rhs) const
{
	if (lhs == rhs)
		return 0;
	if (lhs == 0 || rhs == 0)
		return lhs == 0;

	int idtest = ACE_OS::strcmp (lhs->_interface_repository_id(),
				     rhs->_interface_repository_id());
	if (idtest == -1)
		return 1;
	if (idtest == 1)
		return 0;

	const TAO_MProfile &l_base = lhs->_stubobj()->base_profiles();
	const TAO_MProfile &r_base = rhs->_stubobj()->base_profiles();

	CORBA::ULong maxcount = ACE_MAX(l_base.profile_count(),
					r_base.profile_count());

	for (CORBA::ULong i = 0; i < maxcount; i++)
	{
		TAO_Profile *lp = const_cast<TAO_Profile *>(l_base.get_profile (i));
		TAO_Profile *rp = const_cast<TAO_Profile *>(r_base.get_profile (i));
		CORBA::ULong lhash = lp->hash (CORBA::ULong(-1));
		CORBA::ULong rhash = rp->hash (CORBA::ULong(-1));
		if (lhash < rhash)
			return 1;
		if (lhash > rhash)
			return 0;
	}
	return 0;
}
