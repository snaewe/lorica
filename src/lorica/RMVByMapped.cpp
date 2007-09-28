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
#include "lorica/RMVByMapped.h"

Lorica::RMVByMapped::RMVByMapped (ACE_UINT32  page_size)
	: high_index_(0),
	  page_size_(page_size),
	  free_stack_ (0),
	  num_pages_(1)
{
	pages_ = new page_list_node(page_size_);

}

Lorica::RMVByMapped::~RMVByMapped (void)
{
	delete pages_;
	while (free_stack_ != 0)
		free_stack_ = free_stack_->pop();
}

ACE_UINT32
Lorica::RMVByMapped::next_index(void)
{
	ACE_Guard<ACE_Thread_Mutex> guard(this->high_index_lock_);
	ACE_UINT32 rtn = 0;
	if (free_stack_ != 0)
	{
		rtn = free_stack_->index_;
		free_stack_ = free_stack_->pop();
	}
	else
	{
		rtn = high_index_;
		++high_index_;
	}

	if (rtn >= num_pages_ * page_size_)
	{
		++num_pages_;
		pages_->add_page(page_size_);
	}
	return rtn;
}

bool
Lorica::RMVByMapped::bind (ACE_UINT32 index, ReferenceMapValue *value)
{
	if (index >= num_pages_ * page_size_)
		return false;

	ACE_UINT32 ndx = index % page_size_;
	ReferenceMapValue **page = pages_->find_page (index / page_size_);
	if (page[ndx] != 0)
		return false;

	value->incr_refcount();
	page[ndx] = value;

	return true;

}

bool
Lorica::RMVByMapped::rebind (ACE_UINT32 index, ReferenceMapValue *value)
{
	if (index >= num_pages_ * page_size_)
		return false;

	ACE_UINT32 ndx = index % page_size_;
	ReferenceMapValue **page = pages_->find_page (index / page_size_);
	if (page[ndx] != 0)
		page[ndx]->decr_refcount();

	value->incr_refcount();
	page[ndx] = value;

	return true;
}

bool
Lorica::RMVByMapped::find (ACE_UINT32 index, ReferenceMapValue *&value)
{
	if (index >= num_pages_ * page_size_)
		return false;

	ACE_UINT32 ndx = index % page_size_;
	ReferenceMapValue **page = pages_->find_page (index / page_size_);
	if (page[ndx] == 0)
		return false;
	page[ndx]->incr_refcount();
	value = page[ndx];
	return true;
}

bool
Lorica::RMVByMapped::unbind (ACE_UINT32 index, ReferenceMapValue *&value)
{
	if (index >= num_pages_ * page_size_)
		return false;

	ACE_UINT32 ndx = index % page_size_;
	ReferenceMapValue **page = pages_->find_page (index / page_size_);
	if (page[ndx] == 0)
		return false;
	value = page[ndx];
	page[ndx] = 0;
	free_stack_ = new free_stack_node (index,free_stack_);
	return true;
}
