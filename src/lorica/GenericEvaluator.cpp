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

#include "tao/SystemException.h"
#include "tao/Exception.h"
#include "lorica/ProxyMapper.h"
#include "lorica/ProxyServant.h"
#include "lorica/GenericEvaluator.h"
#include "lorica/debug.h"

#include "tao/DynamicAny/DynamicAnyC.h"
#include "tao/AnyTypeCode/Any_Unknown_IDL_Type.h"


Lorica::GenericEvaluator::GenericEvaluator ( const std::string & typeId,
					     CORBA::InterfaceDef_ptr idef,
					     OCI_APT::OperationTable* ot,
					     DynamicAny::DynAnyFactory_ptr df,
					     ProxyMapper &pm)
	: Lorica::EvaluatorBase (typeId, pm),
	  intDef_ (CORBA::InterfaceDef::_duplicate(idef)),
	  optable_(ot),
	  dynAnyFact_ (DynamicAny::DynAnyFactory::_duplicate (df))
{
}

Lorica::GenericEvaluator::~GenericEvaluator ()
{
}

bool
Lorica::GenericEvaluator::evaluate_request (const char * operation,
					    PortableServer::POA_ptr req_poa,
					    CORBA::ServerRequest_ptr request,
					    CORBA::NVList_ptr args,
					    CORBA::NVList_ptr &out_args,
					    CORBA::NamedValue_ptr &result) const
{
	/// get argument list
	const std::string & rep_id = this->type_id ();

	if (Lorica_debug_level > 4)
		ACE_DEBUG ((LM_DEBUG,
			    "(%P|%t) Lorica::GenericEvaluator::evaluate_request "
			    "GenericEvaluator::evaluate_request, op = %s, type = %s\n",
			    operation, rep_id.c_str()));

	OCI_APT::ArgList *arg_list =  this->optable_->find (operation,
							    rep_id.c_str ());
	if (arg_list == 0)
	{
		try
		{
			arg_list = this->optable_->add_interface(this->intDef_.in(),
								 operation);
		}
		catch (CORBA::SystemException &sysEx)
		{
			if (arg_list)
				arg_list->remove_ref();
			sysEx._tao_print_system_exception ();
			throw;
		}
	}

	CORBA::TypeCode_var result_tc;
	bool is_oneway = false;
	arg_list->prepare_request (args, result_tc, is_oneway);

	request->arguments(args);

	/// proxify the IN/INOUT arguments
	PortableServer::POA_var other_poa = this->mapper_.other_POA (req_poa);
	this->proxify_params (req_poa, args, arg_list, CORBA::ARG_IN);

	if (!is_oneway)
	{
		CORBA::ORB_var orb = this->mapper_.orb();
		// The operation is not a one-way, therefore we must prepare
		// a list of inout and out args that will be used to marshal
		// the response to the client after hearing back from the delegate.
		// Since we are using AMI/AMH model, the reply processing will happen
		// in possibly a different thread, but certainly a different stack.
		orb->create_list (0,out_args);
		for (CORBA::ULong i = 0; i < args->count(); i++)
		{
			CORBA::NamedValue_ptr item = args->item(i);
			if (item->flags() == CORBA::ARG_INOUT ||
			    item->flags() == CORBA::ARG_OUT)
				out_args->add_value (item->name(),
						     *item->value(),
						     item->flags());
		}

		if (result_tc->kind() != CORBA::tk_null &&
		    result_tc->kind() != CORBA::tk_void)
		{
			orb->create_named_value (result);

			TAO::Unknown_IDL_Type *ut = 0;
			ACE_NEW_RETURN (ut, TAO::Unknown_IDL_Type(result_tc.in()), false);
			CORBA::Any typed_any;
			typed_any.replace(ut);

			*(result->value()) = typed_any;
		}
	}
	return true;
}

bool
Lorica::GenericEvaluator::evaluate_reply (const char *operation,
					  PortableServer::POA_ptr req_poa,
					  CORBA::NVList_ptr args,
					  CORBA::NamedValue_ptr result) const
{
	const std::string & rep_id = this->type_id ();
	OCI_APT::ArgList *arg_list =  this->optable_->find (operation,
							    rep_id.c_str ());
	this->proxify_result  (req_poa, result, arg_list);

	/// proxify the OUT arguments
	this->proxify_params (req_poa, args, arg_list, CORBA::ARG_OUT);
	return true;
}


bool
Lorica::GenericEvaluator::evaluate_exception (const char *operation,
					      PortableServer::POA_ptr req_poa,
					      const char * ex_type,
					      TAO_InputCDR &incoming,
					      TAO_OutputCDR &encap) const
{
	if (Lorica_debug_level > 4)
		ACE_DEBUG ((LM_DEBUG,
			    "(%P|%t) Lorica::GenericEvaluator::evaluate_exception "
			    "invoked, operation = %s\n", operation));
	const std::string & rep_id = this->type_id ();
	OCI_APT::ArgList *arg_list =  this->optable_->find (operation,
							    rep_id.c_str ());
	CORBA::Any_var any;
	OCI_APT::Arg *excep = arg_list->find_exception(ex_type, any.out());
	if (excep == 0)
	{
		if (Lorica_debug_level > 0)
			ACE_ERROR ((LM_ERROR,
				    "(%P|%t) Lorica::GenericEvaluator::evaluate_exception "
				    "could not find exception type %s\n", ex_type));
		return false;
	}

	any->impl()->_tao_decode(incoming);
	this->proxify_exception (req_poa, any.inout (), excep);

	if (any->impl()->marshal_value(encap))
	{
		if (Lorica_debug_level > 4)
			ACE_DEBUG ((LM_DEBUG,
				    "(%P|%t) Lorica::GenericEvaluator::evaluate_exception "
				    "Marshal_value succeeded, buflen = %d\n",
				    encap.length()));
	}
	else
		if (Lorica_debug_level > 0)
			ACE_DEBUG ((LM_DEBUG,
				    "(%P|%t) Lorica::GenericEvaluator::evaluate_exception "
				    "Marshal_value failed\n"));
	return true;
}

void
Lorica::GenericEvaluator::proxify_params (PortableServer::POA_ptr req_poa,
					  CORBA::NVList_ptr args,
					  OCI_APT::ArgList *arg_list,
					  CORBA::Flags direction) const
{
	// Iterate through the NVList and proxify each OUT and INOUT
	// parameters

	if (arg_list->must_proxy (direction) == 0)
		return; // nothing to do!

	CORBA::ULong n_arg = 0;
	OCI_APT::Arg *arguments = arg_list->args();
	CORBA::Flags exclude =
		direction == CORBA::ARG_IN ? CORBA::ARG_OUT : CORBA::ARG_IN;

	for ( CORBA::ULong i=0; i < arg_list->num_args(); ++i)
	{
		// if the argument is the not the right direction
		if (arguments[i].mode_ == exclude)
			continue;

		// args[i] is in the right direction, get the nv
		CORBA::NamedValue_ptr nv = args->item (n_arg++);
		while (nv->flags() == exclude)
			nv = args->item (n_arg++);

		// we've examined this arg before and know it is ok.
		if (arguments[i].must_proxy_ == 0)
			continue;

		// Check whether it has object references and then proxify
		if (arguments[i].must_proxy_ == -1)
		{
			arguments[i].must_proxy_ = this->has_objref(arguments[i].tc_);
			if (arguments[i].must_proxy_ == 0)
				continue;
		}

		this->proxify_any (req_poa,*nv->value());
	}
}

void
Lorica::GenericEvaluator::proxify_exception (PortableServer::POA_ptr req_poa,
					     CORBA::Any &value,
					     OCI_APT::Arg *exception) const
{
	if (exception->must_proxy_ == 0)
		return;
	if (exception->must_proxy_ == -1)
	{
		exception->must_proxy_ =
			this->has_objref(exception->tc_);
		if (exception->must_proxy_ == 0)
			return;
	}
	this->proxify_any (req_poa,value);
}

// Checks for object references
// it is a recursive function
bool
Lorica::GenericEvaluator::has_objref(CORBA::TypeCode_ptr tc) const
{
	CORBA::Boolean result = 0;

	while (tc->kind() == CORBA::tk_alias)
		tc = tc->content_type();

	try
	{
		switch ( tc->kind () )
		{
		case CORBA::tk_objref:
			result = 1;
			break;
		case CORBA::tk_struct:
		case CORBA::tk_except:
		case CORBA::tk_union:
		{
			CORBA::ULong mem_cnt = tc->member_count();
			for(CORBA::ULong index=0L; index < mem_cnt; ++index)
			{
				CORBA::TypeCode_var member_tc = tc->member_type(index);
				if ((result = this->has_objref( member_tc.in() )) != 0)
					break;
			}
		}
		break;
		case CORBA::tk_array:
		case CORBA::tk_sequence:
		{
			CORBA::TypeCode_var elt_type = tc->content_type();
			result = this->has_objref(elt_type.in() );
		}
		break;
		case CORBA::tk_any:
		{
			result = 1;
		}
		break;
		default:
			break;
		}
	}
	catch ( CORBA::SystemException& sysEx)
	{
		sysEx._tao_print_system_exception ();
		ACE_ERROR ((LM_ERROR,
			    "[Lorica::GenericEvaluator::has_objref] MinorCode = %d \n",
			    sysEx . minor ()));
		throw;
	}
	catch (...)
	{
		ACE_ERROR ((LM_ERROR,
			    "[Lorica::GenericEvaluator::has_objref] Unknown Exception  \n"));
		throw;
	}

	return result;
}

void
Lorica::GenericEvaluator::proxify_result (PortableServer::POA_ptr req_poa,
					  CORBA::NamedValue_ptr nv,
					  OCI_APT::ArgList *arg_list) const
{
	if (arg_list->result().must_proxy_ == 0)
		return;

	if (arg_list->result().must_proxy_ == -1)
	{
		arg_list->result().must_proxy_ =
			this->has_objref(arg_list->result().tc_);
		if (arg_list->result().must_proxy_ == 0)
			return;
	}
	this->proxify_any (req_poa, *nv->value());

}

void
Lorica::GenericEvaluator::proxify_any (PortableServer::POA_ptr req_poa,
				       CORBA::Any &any) const
{
	DynamicAny::DynAny_var theDynAny =
		this->dynAnyFact_->create_dyn_any (any);

	if (this->proxify (req_poa, theDynAny.inout()))
	{
		CORBA::Any_var tmp = theDynAny->to_any ();
		any = tmp.in();
	}
	theDynAny->destroy();
}

bool
Lorica::GenericEvaluator::proxify (PortableServer::POA_ptr req_poa,
				   DynamicAny::DynAny_ptr &theDynAny) const
{
	bool result = false;
	try
	{
		CORBA::TypeCode_var tc ( theDynAny->type () );
		// Strip aliases
		while (tc->kind () == CORBA::tk_alias )
			tc = tc->content_type ();

		switch ( tc->kind () )
		{
		case CORBA::tk_objref:
		{
			CORBA::Object_var obj = theDynAny->get_reference ();
			if (!this->mapper_.mapped_for_native (obj.inout(), req_poa))
				return result;

			if (!CORBA::is_nil (obj.in()))
			{
				DynamicAny::DynAny_var dyn_any =
					this->dynAnyFact_->create_dyn_any_from_type_code (tc.in ());
				dyn_any->insert_reference (obj.in ());
				theDynAny->assign (dyn_any.in ());
				dyn_any->destroy();
				result = 1;
			}
			else
				result = 0;
		}

		break;
		case CORBA::tk_struct:
		case CORBA::tk_except:
		{
			DynamicAny::DynStruct_var ds =
				DynamicAny::DynStruct::_narrow (theDynAny);
			// I think there may be an easier way to do this, but this will
			// do for now.
			CORBA::ULong mem_cnt = tc->member_count();
			for(CORBA::ULong index=0L; index < mem_cnt; ++index)
			{
				CORBA::TypeCode_var member_tc = tc->member_type(index);

				if (this->has_objref (member_tc.in()))
				{
					ds->seek(index);
					DynamicAny::DynAny_var da_comp =
						ds->current_component();
					result |= this->proxify(req_poa, da_comp.inout());
				}
			}
		}
		break;
		case CORBA::tk_array:
		case CORBA::tk_sequence:
		{
			// This check is *probably* redundant, since we should only
			// get here if the sequence does contain objrefs, but it doesn't
			// hurt.
			CORBA::TypeCode_var elt_type = tc->content_type();
			if (this->has_objref(elt_type.in()))
			{
				DynamicAny::DynSequence_var ds =
					DynamicAny::DynSequence::_narrow ( theDynAny );
				DynamicAny::DynAnySeq_var theDynSeq =
					ds->get_elements_as_dyn_any ();
				for ( size_t i = 0; i < theDynSeq->length (); ++i )
				{
					result |= this->proxify (req_poa, theDynSeq[i].inout());
				}
			}
		}
		break;
		case CORBA::tk_union:
		{
			DynamicAny::DynUnion_var du =
				DynamicAny::DynUnion::_narrow (theDynAny);
			if (!du->has_no_active_member())
			{
				DynamicAny::DynAny_var dyn_any =  du->member ();
				CORBA::TypeCode_var elt_type = dyn_any->type();
				if (this->has_objref(elt_type.in()))
					result |= this->proxify (req_poa, dyn_any.inout());
			}
		}
		break;
		case CORBA::tk_any:
		{
			//  If that does not contain an objref, then don't proxify it.
			DynamicAny::DynAny_var dyn_any = theDynAny->get_dyn_any();
			if( this->has_objref(dyn_any->type()))
			{
				result |= this->proxify (req_poa, dyn_any.inout());
				theDynAny->insert_dyn_any(dyn_any.in());
			}
			dyn_any->destroy ();
		}
		break;
		default:
			break;
		} /* End of Switch Statement */
	catch (CORBA::Exception& Ex)
	{
		ACE_ERROR ((LM_ERROR,
			    "(%P|%t) Lorica::GenericEvaluator::proxify caught: %s\n", Ex._name ()));
		throw;
	}
	catch (...)
	{
		ACE_ERROR ((LM_ERROR,
			    "(%P|%t) Lorica::GenericEvaluator::proxify caught unknown exception\n"));
		throw;
	}

	return result;
}
