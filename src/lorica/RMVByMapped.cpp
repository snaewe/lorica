/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: t; c-basic-offset: 2 -*- */

/*
 *    Lorica source file.
 *    Copyright (C) 2007-2008 OMC Denmark ApS.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ace/OS_NS_time.h>

#include "RMVByMapped.h"
#include "debug.h"

Lorica::RMVByMapped::RMVByMapped(time_t gc_period, ACE_UINT32 page_size)
	: high_index_(0),
	  gc_terminate_(gc_control_lock_),
	  gc_period_secs_(gc_period),
	  gc_terminated_ (false),
	  page_size_(page_size),
	  free_stack_ (0),
	  num_pages_(1)
{
	pages_ = new page_list_node(page_size_);
	this->activate();
}

Lorica::RMVByMapped::~RMVByMapped(void)
{
	gc_terminated_ = true;
	gc_terminate_.broadcast();
	this->wait();
	delete pages_;
	while (free_stack_ != 0)
		free_stack_ = free_stack_->pop();
}

ACE_UINT32
Lorica::RMVByMapped::next_index(void)
{
	ACE_Guard<ACE_Thread_Mutex> guard(this->high_index_lock_);
	ACE_UINT32 rtn = 0;

	if (free_stack_ != 0) {
		rtn = free_stack_->index_;
		free_stack_ = free_stack_->pop();
	} else {
		rtn = high_index_;
		++high_index_;
	}

	if (rtn >= num_pages_ * page_size_) {
		++num_pages_;
		pages_->add_page(page_size_);
	}

	return rtn;
}

bool
Lorica::RMVByMapped::bind(ACE_UINT32 index,
			  ReferenceMapValue *value)
{
	ACE_Guard<ACE_Thread_Mutex> guard (this->gc_control_lock_);
	if (index >= num_pages_ * page_size_)
		return false;

	ACE_UINT32 ndx = index % page_size_;
	ReferenceMapValue **page = pages_->find_page(index / page_size_);
	if (page[ndx] != 0)
		return false;

	value->incr_refcount();
	page[ndx] = value;

	return true;

}

bool
Lorica::RMVByMapped::rebind(ACE_UINT32 index,
			    ReferenceMapValue *value)
{
	ACE_Guard<ACE_Thread_Mutex> guard (this->gc_control_lock_);
	if (index >= num_pages_ * page_size_)
		return false;

	ACE_UINT32 ndx = index % page_size_;
	ReferenceMapValue **page = pages_->find_page(index / page_size_);
	if (page[ndx] != 0)
		page[ndx]->decr_refcount();

	value->incr_refcount();
	page[ndx] = value;

	return true;
}

bool
Lorica::RMVByMapped::find(ACE_UINT32 index,
			  ReferenceMapValue *& value)
{
	ACE_Guard<ACE_Thread_Mutex> guard (this->gc_control_lock_);
	if (index >= num_pages_ * page_size_)
		return false;

	ACE_UINT32 ndx = index % page_size_;
	ReferenceMapValue **page = pages_->find_page(index / page_size_);
	if (page[ndx] == 0)
		return false;

	page[ndx]->incr_refcount();
	value = page[ndx];

	return true;
}

bool
Lorica::RMVByMapped::unbind(ACE_UINT32 index,
			    ReferenceMapValue *& value)
{
	ACE_Guard<ACE_Thread_Mutex> guard (this->gc_control_lock_);
	if (index >= num_pages_ * page_size_)
		return false;

	ACE_UINT32 ndx = index % page_size_;
	ReferenceMapValue **page = pages_->find_page(index / page_size_);
	if (page[ndx] == 0)
		return false;

	value = page[ndx];
	page[ndx] = 0;
	this->free_stack_ = new free_stack_node(index, this->free_stack_);

	return true;
}

int
Lorica::RMVByMapped::svc (void)
{
	if (Lorica_debug_level > 0)
		ACE_DEBUG ((LM_DEBUG,
			    "(%P|%t) RMVByMapped::svc, garbage collection loop "
			    "commensing, wait_period set to %d\n", this->gc_period_secs_));
	while (!this->gc_terminated_)
		{
			ACE_Guard<ACE_Thread_Mutex> guard (this->gc_control_lock_);
			ACE_Time_Value wait_period (this->gc_period_secs_ + ACE_OS::time(0));
			int result = this->gc_terminate_.wait(&wait_period);

			if (result == 0 || errno != ETIME)
				break;
			
			if (Lorica_debug_level > 4)
				ACE_DEBUG ((LM_DEBUG,
					    "(%P|%t) RMVByMapped::svc, invoking "
					    "collection, result = %d %p\n", result,"wait" ));
			size_t page_offset = 0;
			size_t test_count = 0;
			size_t reap_count = 0;
			for (page_list_node *iter = this->pages_;
					 iter != 0 && !this->gc_terminated_;
					 iter = iter->next_)
				{
					ReferenceMapValue **rmv = iter->page_;
					for (size_t index = 0; 
					     index < this->page_size_ && !this->gc_terminated_; 
					     index++)
						{
							if (rmv[index] != 0)
								{
									test_count++;
									ReferenceMapValue *value = rmv[index];
									bool expired = false;
									try
										{
											CORBA::Object_var obj = value->orig_ref_;
											guard.release();
											expired = obj->_non_existent();
										}
									catch (CORBA::Exception &)
										{
											expired = true;
										}
									guard.acquire();
									// recheck, since we released the lock, an unbind
									// may have occured.
									if (expired && rmv[index] == value)
										{
											reap_count++;
											rmv[index]->decr_refcount();
											rmv[index] = 0;
											this->free_stack_ =
												new free_stack_node(page_offset + index,
																						this->free_stack_);
												
										}
								}
						}
					page_offset += this->page_size_;
				}
			if (!this->gc_terminated_ && reap_count > 0 && Lorica_debug_level > 0)
				ACE_DEBUG ((LM_DEBUG,
										"(%P|%t) RMVByMapped::svc, garbage collector reaped "
										"%d out of %d references\n", reap_count, test_count));

		}
  if (Lorica_debug_level > 0)
		ACE_DEBUG ((LM_DEBUG,
								"(%P|%t) RMVByMapped::svc, garbage collection loop "
								"terminating\n"));

	return 0;
}
