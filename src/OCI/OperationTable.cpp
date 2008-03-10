/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 *  This file and all software in this directory and any subdirectories
 *  (henceforth  referred to as "OCI software") is copyrighted
 *  by [1]Object Computing, Inc (OCI), Copyright (C) 2007, all rights
 *  reserved.
 *
 *  Rights Granted By This License
 *
 *  This software is open-source, freely available software. You are
 *  free to use, modify, copy, and distribute -- perpetually and
 *  irrevocably -- the source code in this directory and any object
 *  code produced from this source, as well as copy and distribute
 *  modified versions of this software. You must, however, include this
 *  copyright statement along with any code built using OCI software
 *  that you release. No copyright statement need be provided if you
 *  just ship binary executables of your software products.
 *
 *  You may use OCI software in commercial and/or binary software
 *  releases and are under no obligation to redistribute any of your
 *  source code that is built using OCI software. Note, however, that
 *  you may not do anything to the OCI software code, such as
 *  copyrighting it yourself or claiming authorship of the OCI software
 *  code, that will prevent OCI software from being distributed freely
 *  using an open-source development model. You needn't inform anyone
 *  that you are using OCI software in your software, though we
 *  encourage you to let us [2]know about it.
 *
 *  You are welcome to contribute bug fixes and enhancements to this
 *  [3]software. Contributions, if accepted by OCI or its delegate and
 *  incorporated into this software, will be subject to the same
 *  copyright as the rest of this software. The contributor
 *  relinquishes all other rights and claims to the contribution.
 *  Contributions will be attributed in the ChangeLog.
 *
 *  OCI software is provided as is with no warranties of any kind,
 *  including the warranties of design, merchantability, and fitness
 *  for a particular purpose, noninfringement, or arising from a course
 *  of dealing, usage or trade practice. Object Computing, Inc., and
 *  its employees shall have no liability with respect to the
 *  infringement of copyrights, trade secrets or any patents by OCI
 *  software or any part thereof. Moreover, in no event will Object
 *  Computing, Inc. or its employees be liable for any lost revenue or
 *  profits or other special, indirect and consequential damages.
 *
 *  OCI software is provided with commercial support, available directly
 *  from Object Computing, Inc.
 *
 *   ------------------------------------------------------------------
 *
 *  References
 *
 *  1. http://www.ociweb.com
 *  2. mailto:sales@ociweb.com
 *  3. mailto:support@ociweb.com
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "OperationTable.h"
#include <tao/IFR_Client/IFR_BasicC.h>
#include <tao/corbafwd.h>
#include <tao/CORBA_String.h>
#include <tao/AnyTypeCode/Any_Unknown_IDL_Type.h>
#include <ace/OS_NS_string.h>


//---------------------------------------------------------------------------

OCI_APT::Arg::Arg ()
	: tc_ (CORBA::TypeCode::_nil ()),
	  mode_ (0),
	  must_proxy_ (-1)
{
}

OCI_APT::Arg::~Arg (void)
{
	CORBA::release (this->tc_);
}

void
OCI_APT::Arg::init (CORBA::TypeCode_ptr tc, CORBA::Flags m)
{
	CORBA::release (this->tc_);
	this->tc_ = CORBA::TypeCode::_duplicate (tc);
	this->mode_ = m;
	this->must_proxy_ = -1;
}

//---------------------------------------------------------------------------

OCI_APT::ArgList::ArgList ()
	: result_ (),
	  num_args_ (0),
	  args_ (0),
	  num_exceptions_ (0),
	  exceptions_ (0),
	  is_oneway_ (0),
	  refcount_ (1)
{
	this->must_proxy_[0] = this->must_proxy_[1] = -1;
}

OCI_APT::ArgList::ArgList (CORBA::ULong argc,
			   CORBA::ULong excepc,
			   int oneway)
	: result_ (),
	  num_args_ (0),
	  args_ (0),
	  num_exceptions_ (0),
	  exceptions_ (0),
	  is_oneway_ (0),
	  refcount_ (1)
{
	this->must_proxy_[0] = this->must_proxy_[1] = -1;
	this->init (argc,
		    excepc, oneway);
}

OCI_APT::ArgList::~ArgList (void)
{
	delete [] args_;
	delete [] exceptions_;
}

void
OCI_APT::ArgList::init (CORBA::ULong argc,
			CORBA::ULong excepc,
			int oneway)
{
	this->args_ = new Arg [argc];
	this->num_args_ = argc;
	this->exceptions_ = new Arg [excepc];
	this->num_exceptions_ = excepc;
	this->is_oneway_ = oneway;
}

void
OCI_APT::ArgList::prepare_request(CORBA::NVList_ptr args,
				  CORBA::TypeCode_ptr &result_tc,
				  bool &is_oneway)
{
	is_oneway = this->is_oneway_;
	if (!is_oneway)
		result_tc = CORBA::TypeCode::_duplicate (this->result_.tc_);
	for (CORBA::ULong i = 0; i < this->num_args_; i++)
	{
		TAO::Unknown_IDL_Type *ut = 0;
		ACE_NEW(ut, TAO::Unknown_IDL_Type(this->args_[i].tc_));
		CORBA::Any_var value = new CORBA::Any;
		value->replace(ut);
		args->add_value("",value.in(), this->args_[i].mode_);
	}
}

OCI_APT::Arg *
OCI_APT::ArgList::find_exception (const char *id,
				  CORBA::Any *& value)
{
	OCI_APT::Arg *result = 0;
	ACE_NEW_RETURN (value,CORBA::Any,0);

	for (CORBA::ULong i = 0; i < this->num_exceptions_; i++)
	{
		if (ACE_OS::strcmp(id,
				   this->exceptions_[i].tc_->id()) == 0)
		{
			result = &this->exceptions_[i];
			TAO::Unknown_IDL_Type *ut = 0;
			ACE_NEW_RETURN(ut,
				       TAO::Unknown_IDL_Type(this->exceptions_[i].tc_),
				       0);
			value->replace (ut);
		}
	}
	return result;
}

// Reference count manipulators. Object will destroy self when count
// goes to zero
int
OCI_APT::ArgList::add_ref (void)
{
	return ++this->refcount_;
}

int
OCI_APT::ArgList::remove_ref (void)
{
	int r = --this->refcount_;
	if (r == 0)
		delete this;
	return r;
}

int
OCI_APT::ArgList::must_proxy(CORBA::Flags mode) const
{
	return this->must_proxy_[mode == CORBA::ARG_IN ? 0 : 1];
}

void
OCI_APT::ArgList::must_proxy (CORBA::Flags mode, int p)
{
	this->must_proxy_[mode == CORBA::ARG_IN ? 0 : 1] = p;
}

int
OCI_APT::ArgList::is_oneway (void) const
{
	return this->is_oneway_;
}


// notice that this assignment takes ownership of the value.
void
OCI_APT::ArgList::result (CORBA::TypeCode_ptr tc)
{
	this->result_.init(tc,0);
}

OCI_APT::Arg &
OCI_APT::ArgList::result ()
{
	return this->result_;
}

CORBA::ULong
OCI_APT::ArgList::num_args(void) const
{
	return this->num_args_;
}

void
OCI_APT::ArgList::set_arg (CORBA::ULong ndx,
			   CORBA::TypeCode_ptr tc,
			   CORBA::Flags mode)
{
	this->args_[ndx].init(tc,mode);
}

OCI_APT::Arg *
OCI_APT::ArgList::args ()
{
	return this->args_;
}

CORBA::ULong
OCI_APT::ArgList::num_exceps(void) const
{
	return this->num_exceptions_;
}

void
OCI_APT::ArgList::set_excep (CORBA::ULong ndx,
			     CORBA::TypeCode_ptr tc)
{
	this->exceptions_[ndx].init(tc,0);
}

OCI_APT::Arg *
OCI_APT::ArgList::exceps ()
{
	return this->exceptions_;
}

//---------------------------------------------------------------------------

OCI_APT::OpArgs::OpArgs (const char *ref_id, ArgList *args)
	:next_ (0),
	 args_(0),
	 id_(0)
{
	if (args != 0)
	{
		args_ = args;
		args_->add_ref();
	}
	id_ = ACE_OS::strdup (ref_id);
}

OCI_APT::OpArgs::~OpArgs (void)
{
	args_->remove_ref();
	free (id_);
	delete this->next_;
}

OCI_APT::ArgList *
OCI_APT::OpArgs::args (void)
{
	this->args_->add_ref();
	return this->args_;
}

const char *
OCI_APT::OpArgs::id () const
{
	return this->id_;
}

OCI_APT::OpArgs *
OCI_APT::OpArgs::next ()
{
	return this->next_;
}

void
OCI_APT::OpArgs::next (OpArgs *n)
{
	this->next_ = n;
}

//---------------------------------------------------------------------------

OCI_APT::Operation::Operation (const char *opname)
	: name_(0),
	  left_(0),
	  right_(0),
	  arg_set_ (0)
{
	if (opname)
		name_ = ACE_OS::strdup(opname);
}

OCI_APT::Operation::~Operation()
{
	if (name_)
		free(name_);
	delete left_;
	delete right_;
	delete arg_set_;
}

OCI_APT::ArgList *
OCI_APT::Operation::arg_list (const char * rep_id)
{
	OpArgs *a = this->args (rep_id);
	if (a == 0)
		return 0;
	return a->args();
}

const char *
OCI_APT::Operation::name() const
{
	return this->name_;
}

OCI_APT::Operation *
OCI_APT::Operation::left()
{
	return this->left_;
}

OCI_APT::Operation *
OCI_APT::Operation::right ()
{
	return this->right_;
}

void
OCI_APT::Operation::left(Operation *o)
{
	this->left_ = o;
}

void
OCI_APT::Operation::right (Operation *o)
{
	this->right_ = o;
}

OCI_APT::OpArgs *
OCI_APT::Operation::args (const char *rep_id)
{
	OCI_APT::OpArgs *a = this->arg_set_;
	while (a != 0)
	{
		if (ACE_OS::strcmp (rep_id,a->id()) == 0)
			return a;
		a = a->next();
	}
	return a;
}

// this assignment takes ownership of argument.
void
OCI_APT::Operation::add_set (OCI_APT::OpArgs *arglist)
{
	if (this->arg_set_ == 0)
	{
		this->arg_set_ = arglist;
		return;
	}

	OCI_APT::OpArgs *a = this->arg_set_;
	while (a->next() != 0)
		a = a->next();
	a->next (arglist);
}

//----------------------------------------------------------------------------
OCI_APT::OperationTable::OperationTable (/* const char * persistence_file? */)
	: head_ (0)
{
	OCI_APT::ArgList *al = this->add_is_a("IDL:org.omg/CORBA/Object:1.0");
	al->remove_ref();
	al = this->add_non_existent ("IDL:org.omg/CORBA/Object:1.0");
	al->remove_ref();
}

OCI_APT::OperationTable::~OperationTable ()
{
	delete head_;
}

OCI_APT::ArgList *
OCI_APT::OperationTable::find (const char *opname,
			       const char *rep_id)
{
	ACE_Read_Guard<ACE_RW_Thread_Mutex>(this->lock_);
	OCI_APT::Operation *op = head_;
	while (op != 0)
	{
		int match = ACE_OS::strcmp(opname,op->name());
		if (match == 0)
		{
			OCI_APT::OpArgs *oa = op->args (rep_id);
			if (oa == 0)
				return 0;
			return oa->args();
		}
		op = match == 1 ? op->right() : op->left();
	}
	return 0;
}

OCI_APT::Operation *
OCI_APT::OperationTable::find_or_add (const char *opname)
{
	if (head_ == 0)
	{
		head_ = new OCI_APT::Operation (opname);
		return head_;
	}
	OCI_APT::Operation *result = head_;
	while (result != 0)
	{
		int match = ACE_OS::strcmp(opname,result->name());
		if (match == 0)
			return result;
		if (match == 1)
			if (result->right() == 0)
			{
				OCI_APT::Operation *temp = new OCI_APT::Operation(opname);
				result->right(temp);
				return temp;
			}
			else
				result = result->right();
		else
			if (result->left() == 0)
			{
				OCI_APT::Operation *temp = new OCI_APT::Operation(opname);
				result->left(temp);
				return temp;
			}
			else
				result = result->left();
	}
	return result;
}

OCI_APT::ArgList *
OCI_APT::OperationTable::add_interface (CORBA::InterfaceDef_ptr intDef,
					const char * desired_op)
{
  bool want_attribute = false;
  if (desired_op && desired_op[0] == '_')
		want_attribute = ACE_OS::strncmp(desired_op+2,"et_",3) == 0;

	ACE_Write_Guard<ACE_RW_Thread_Mutex>(this->lock_);
	OCI_APT::ArgList *result = 0;
	CORBA::InterfaceDefSeq_var bases = intDef->base_interfaces ();
	CORBA::String_var *derived = new CORBA::String_var[bases->length()];
	CORBA::ULong n_bases = bases->length();
	CORBA::ULong i = 0;
	for (; i <= n_bases; i++)
	{
		CORBA::String_var rep_id = intDef->id();
		if (i == 0)
		{
			OCI_APT::ArgList *al = this->add_is_a (rep_id.in());
			if (!want_attribute && desired_op && ACE_OS::strcmp("_is_a",desired_op) == 0)
				result = al;
			else
				al->remove_ref();

			al = this->add_non_existent (rep_id.in());
			if (!want_attribute && desired_op && ACE_OS::strcmp("_non_existent",desired_op) == 0)
				result = al;
			else
				al->remove_ref();
		}

		CORBA::ContainedSeq_var attributes =
			intDef->contents (CORBA::dk_Attribute,1);
		CORBA::ULong n_ats = attributes->length();
		for (CORBA::ULong at = 0; at < n_ats; ++at)
			{
				CORBA::String_var attr_name = attributes[at]->name();
				CORBA::AttributeDef_var attr =
					CORBA::AttributeDef::_narrow (attributes[at]);
				CORBA::String_var attr_op =
					CORBA::string_alloc(ACE_OS::strlen(attr_name.in()) + 5);
				ACE_OS::strcpy(attr_op.inout(),"_get_");
				ACE_OS::strcat(attr_op.inout(),attr_name.in());
				OCI_APT::Operation *op_ptr = this->find_or_add(attr_op.in());
				OCI_APT::ArgList *arg_list =
					new OCI_APT::ArgList (0, 0, false);
				arg_list->result (attr->type());
				if (want_attribute && ACE_OS::strcmp (attr_op.in(),desired_op) == 0)
					{
						arg_list->add_ref();
						result = arg_list;
					}

				op_ptr->add_set (new OpArgs(rep_id.in(),arg_list));
				for (CORBA::ULong d = 0; d < i; d++)
					op_ptr->add_set (new OpArgs(derived[d],arg_list));
				arg_list->remove_ref();

				if (attr->mode() == CORBA::ATTR_READONLY)
					continue;

				attr_op.inout()[1] = 's'; // from _get_ to _set_
				op_ptr = this->find_or_add(attr_op.in());
				arg_list =
					new OCI_APT::ArgList (1, 0, false);
				arg_list->set_arg(0, attr->type(),CORBA::ARG_IN);
				arg_list->result (CORBA::_tc_void);
				if (want_attribute && ACE_OS::strcmp (attr_op.in(),desired_op) == 0)
					{
						arg_list->add_ref();
						result = arg_list;
					}

				op_ptr->add_set (new OpArgs(rep_id.in(),arg_list));
				for (CORBA::ULong d = 0; d < i; d++)
					op_ptr->add_set (new OpArgs(derived[d],arg_list));
				arg_list->remove_ref();

			}

		CORBA::ContainedSeq_var operations =
			intDef->contents (CORBA::dk_Operation,1);

		CORBA::ULong n_ops = operations->length ();
		for (CORBA::ULong op = 0; op < n_ops; ++op)
			{
				CORBA::String_var op_name = operations[op]->name();
				OCI_APT::Operation *op_ptr = this->find_or_add(op_name.in());

				CORBA::OperationDef_var opDef =
					CORBA::OperationDef::_narrow (operations[op]);
				CORBA::ParDescriptionSeq_var params = opDef->params ();
				CORBA::ExceptionDefSeq_var excepts = opDef->exceptions();

				int is_oneway = opDef->mode() == CORBA::OP_ONEWAY;
				OCI_APT::ArgList *arg_list =
					new OCI_APT::ArgList (params->length(),
																excepts->length(),
																is_oneway);
				if (!is_oneway)
					arg_list->result (opDef->result());

				for (CORBA::ULong p = 0; p < params->length (); ++p)
					{
						CORBA::Flags mode = CORBA::ARG_IN;
						if (params[p].mode == CORBA::PARAM_OUT)
							mode = CORBA::ARG_OUT;
						else if (params[p].mode == CORBA::PARAM_INOUT)
							mode = CORBA::ARG_INOUT;

						arg_list->set_arg(p, params[p].type.in(),mode);
					}

				for (CORBA::ULong e = 0; e < excepts->length (); ++e)
					{
						CORBA::TypeCode_var tc = excepts[e]->type();
						arg_list->set_excep(e, tc.in());
					}

				if (!want_attribute && desired_op && ACE_OS::strcmp(op_name,desired_op) == 0)
					{
						arg_list->add_ref();
						result = arg_list;
					}

				op_ptr->add_set (new OpArgs(rep_id.in(),arg_list));
				for (CORBA::ULong d = 0; d < i; d++)
					op_ptr->add_set (new OpArgs(derived[d],arg_list));
				arg_list->remove_ref();
			}

		if (i < bases->length())
		{
			derived[i] = rep_id;
			intDef = bases[i];
		}
	}
	delete [] derived;
	return result;
}

OCI_APT::ArgList *
OCI_APT::OperationTable::add_is_a (const char *rep_id)
{
	OCI_APT::Operation *op = this->find_or_add("_is_a");
	int is_oneway = 0;
	OCI_APT::ArgList *arg_list = new OCI_APT::ArgList (1,0, is_oneway);
	arg_list->result (CORBA::_tc_boolean);

	arg_list->set_arg(0, CORBA::_tc_string,CORBA::ARG_IN);
	op->add_set (new OpArgs(rep_id,arg_list));
	return arg_list;
}

OCI_APT::ArgList *
OCI_APT::OperationTable::add_non_existent (const char *rep_id)
{
	OCI_APT::Operation *op = this->find_or_add("_non_existent");
	int is_oneway = 0;
	OCI_APT::ArgList *arg_list = new OCI_APT::ArgList (0,0, is_oneway);
	arg_list->result (CORBA::_tc_boolean);

	op->add_set (new OCI_APT::OpArgs(rep_id,arg_list));
	return arg_list;
}
