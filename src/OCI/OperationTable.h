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

#ifndef OPERATION_TABLE_H
#define OPERATION_TABLE_H

#include "ace/Synch.h"
#include "tao/AnyTypeCode/NVList.h"
#include "tao/IFR_Client/IFR_BasicC.h"

#include "OCI/AsynchProxyTools_Export.h"

namespace OCI_APT
{
	//***************************************************************************
	//
	// Arg encapuslates a single argument's details.

	class OCI_AsynchProxyTools_Export Arg
	{
	public:
		Arg ();
		~Arg (void);

		void init (CORBA::TypeCode_ptr tc, CORBA::Flags m);

		// tc_ is the type code for the arugment. It may be arbirarily complex.
		// In the future, it might be resonable to further pre-evaluate complex
		// type codes to tag individual elements as having an object reference
		// or not, but for now we just maintain that for the whole argument.
		CORBA::TypeCode_ptr tc_;

		// mode_ is the data-direction flag. It is set for IN, INOUT, OUT args.
		// It is not set for return values. Mode_ is used along with tc_ to
		// populate arg NVLists to handle requests.
		CORBA::Flags mode_;

		// This flag is set by the ObjectMap class. If negative, then the
		// status is unknown and must be set. If positive, then the argument
		// must be processed to proxify a contained object reference. Complex
		// types (structs, unions, etc.) must be traversed at parse time to
		// locate the object references within.
		int must_proxy_;
	};

	//***************************************************************************
	//
	// ArgList is a reference counted object, it may be associated with more
	// than one interface/operation. Such a multiplicity will happen whenever
	// an interface is a known base of other interfaces, in which case two or
	// more interfaces will have the same operation with the same arguments.

	class OCI_AsynchProxyTools_Export ArgList
	{
	public:
		ArgList ();
		ArgList (CORBA::ULong argc,
			 CORBA::ULong excepc,
			 int oneway);
		~ArgList (void);

		void init (CORBA::ULong argc,
			   CORBA::ULong excepc,
			   int oneway);

		void prepare_request(CORBA::NVList_ptr args,
				     CORBA::TypeCode_ptr &result_tc,
				     bool &is_oneway);

		Arg * find_exception (const char * id,
				      CORBA::Any *& value);

		// Reference count manipulators. Object will destroy self when count
		// goes to zero
		int add_ref (void);
		int remove_ref (void);

		// The must_proxy flag is a short-hand way to determine if *any* arguments
		// need to be proxified. This isn't known until the first time the operation
		// is actually traversed by the ObjectMap. Since all operations on an
		// interface are added to this collection whenever the interface repository
		// is queried, it is possible that any particular operation is never
		// invoked.
		int must_proxy(CORBA::Flags) const;
		void must_proxy (CORBA::Flags,int );

		// Returns true if the associated operation is a one-way
		int is_oneway (void) const;

		// The details for the return type, which may be null.
		void result (CORBA::TypeCode_ptr tc);
		Arg& result ();

		// Access to the list of arguments for this operation
		CORBA::ULong num_args(void) const;
		void set_arg (CORBA::ULong ndx, CORBA::TypeCode_ptr, CORBA::Flags);
		Arg *args (void);

		// Access to the list of user exceptions for this operation
		CORBA::ULong num_exceps(void) const;
		void set_excep (CORBA::ULong ndx, CORBA::TypeCode_ptr);
		Arg *exceps (void);

	private:
		int must_proxy_[2];
		Arg result_;
		CORBA::ULong num_args_;
		Arg *args_;
		CORBA::ULong num_exceptions_;
		Arg *exceptions_;

		int is_oneway_;

		int refcount_;
	};

	//***************************************************************************
	//
	// OpArgs is keyed by interface repository name, and includes the
	// particulars of an operation in an interface, including the oneway flag,
	// result value typecode, arg mode & type. Also, the OpArgs contains the
	// proxification requirements for the arguments.
	//

	class OCI_AsynchProxyTools_Export OpArgs
	{
	public:
		OpArgs (const char *ref_id, ArgList *args = 0);
		~OpArgs (void);

		ArgList *args (void);

		const char * id (void) const;

		OpArgs *next (void);
		void next (OpArgs *);

	private:
		OpArgs *next_;
		ArgList *args_;
		char * id_;

	};

	//***************************************************************************
	//
	// Operation has a name, and participates in a balanced tree. The
	// operation also has a collection interfaces that have an operation by
	// this name. This includes base interfaces and happenstance names
	//

	class OCI_AsynchProxyTools_Export Operation
	{
	public:
		Operation (const char *name);
		~Operation (void);

		ArgList *arg_list (const char * rep_id);

		const char * name(void) const;
		Operation *left(void);
		Operation *right(void);

		void left (Operation *o);
		void right (Operation *o);

		OpArgs *args (const char *rep_id);
		void add_set (OpArgs *arglist);

	private:
		char *name_;
		Operation *left_;
		Operation *right_;
		OpArgs *arg_set_;
	};

	//***************************************************************************
	//
	// OperationTable is a collection Operation objects, sorted by opname
	// with a few helper methods.

	class OCI_AsynchProxyTools_Export OperationTable
	{
	public:
		OperationTable ();
		~OperationTable ();

		ArgList * find (const char * opname,
				const char * rep_id);

		ArgList * add_interface (CORBA::InterfaceDef_ptr intDef,
					 const char * opname = 0);

	private:
		ArgList * add_is_a (const char *rep_id);
		ArgList * add_non_existent (const char *rep_id);

		Operation * find_or_add (const char *opname);

		Operation *head_;
		ACE_RW_Thread_Mutex lock_;
	};
}

#endif
