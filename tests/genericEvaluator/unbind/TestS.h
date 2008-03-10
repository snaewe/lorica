// -*- C++ -*-
//
// $Id$

// ****  Code generated by the The ACE ORB (TAO) IDL Compiler ****
// TAO and the TAO IDL Compiler have been developed by:
//       Center for Distributed Object Computing
//       Washington University
//       St. Louis, MO
//       USA
//       http://www.cs.wustl.edu/~schmidt/doc-center.html
// and
//       Distributed Object Computing Laboratory
//       University of California at Irvine
//       Irvine, CA
//       USA
//       http://doc.ece.uci.edu/
// and
//       Institute for Software Integrated Systems
//       Vanderbilt University
//       Nashville, TN
//       USA
//       http://www.isis.vanderbilt.edu/
//
// Information about TAO is available at:
//     http://www.cs.wustl.edu/~schmidt/TAO.html

// TAO_IDL - Generated from
// be/be_codegen.cpp:487

#ifndef _TAO_IDL_TESTS_H_
#define _TAO_IDL_TESTS_H_


#include "TestC.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "tao/Collocation_Proxy_Broker.h"
#include "tao/PortableServer/PortableServer.h"
#include "tao/PortableServer/Servant_Base.h"

// TAO_IDL - Generated from
// be/be_visitor_module/module_sh.cpp:49

namespace POA_Test
{
  
  
  // TAO_IDL - Generated from
  // be/be_visitor_interface/interface_sh.cpp:87
  
  class First;
  typedef First *First_ptr;
  
  class  First
    : public virtual PortableServer::ServantBase
  {
  protected:
    First (void);
  
  public:
    // Useful for template programming.
    typedef ::Test::First _stub_type;
    typedef ::Test::First_ptr _stub_ptr_type;
    typedef ::Test::First_var _stub_var_type;
    
    First (const First& rhs);
    virtual ~First (void);
    
    virtual ::CORBA::Boolean _is_a (const char* logical_type_id);
    
    static void _is_a_skel (
        TAO_ServerRequest & req,
        void * servant_upcall,
        void * servant
      );
    
    static void _non_existent_skel (
        TAO_ServerRequest & req,
        void * servant_upcall,
        void * servant
      );
    
    static void _interface_skel (
        TAO_ServerRequest & req,
        void * servant_upcall,
        void * servant
      );
    
    static void _component_skel (
        TAO_ServerRequest & req,
        void * servant_upcall,
        void * servant
      );
    
    static void _repository_id_skel (
        TAO_ServerRequest & req,
        void * servant_upcall,
        void * servant);
    
    virtual void _dispatch (
        TAO_ServerRequest & req,
        void * servant_upcall);
    
    ::Test::First *_this (void);
    
    virtual const char* _interface_repository_id (void) const;
    
    // TAO_IDL - Generated from
    // be/be_visitor_operation/operation_sh.cpp:45
    
    virtual void method (
        void) = 0;
    
    static void method_skel (
        TAO_ServerRequest & server_request,
        void * servant_upcall,
        void * servant
      );
  };
  
  // TAO_IDL - Generated from
  // be/be_visitor_interface/interface_sh.cpp:87
  
  class Second;
  typedef Second *Second_ptr;
  
  class  Second
    : public virtual PortableServer::ServantBase
  {
  protected:
    Second (void);
  
  public:
    // Useful for template programming.
    typedef ::Test::Second _stub_type;
    typedef ::Test::Second_ptr _stub_ptr_type;
    typedef ::Test::Second_var _stub_var_type;
    
    Second (const Second& rhs);
    virtual ~Second (void);
    
    virtual ::CORBA::Boolean _is_a (const char* logical_type_id);
    
    static void _is_a_skel (
        TAO_ServerRequest & req,
        void * servant_upcall,
        void * servant
      );
    
    static void _non_existent_skel (
        TAO_ServerRequest & req,
        void * servant_upcall,
        void * servant
      );
    
    static void _interface_skel (
        TAO_ServerRequest & req,
        void * servant_upcall,
        void * servant
      );
    
    static void _component_skel (
        TAO_ServerRequest & req,
        void * servant_upcall,
        void * servant
      );
    
    static void _repository_id_skel (
        TAO_ServerRequest & req,
        void * servant_upcall,
        void * servant);
    
    virtual void _dispatch (
        TAO_ServerRequest & req,
        void * servant_upcall);
    
    ::Test::Second *_this (void);
    
    virtual const char* _interface_repository_id (void) const;
    
    // TAO_IDL - Generated from
    // be/be_visitor_operation/operation_sh.cpp:45
    
    virtual void method (
        void) = 0;
    
    static void method_skel (
        TAO_ServerRequest & server_request,
        void * servant_upcall,
        void * servant
      );
  };
  
  // TAO_IDL - Generated from
  // be/be_visitor_interface/interface_sh.cpp:87
  
  class Third;
  typedef Third *Third_ptr;
  
  class  Third
    : public virtual PortableServer::ServantBase
  {
  protected:
    Third (void);
  
  public:
    // Useful for template programming.
    typedef ::Test::Third _stub_type;
    typedef ::Test::Third_ptr _stub_ptr_type;
    typedef ::Test::Third_var _stub_var_type;
    
    Third (const Third& rhs);
    virtual ~Third (void);
    
    virtual ::CORBA::Boolean _is_a (const char* logical_type_id);
    
    static void _is_a_skel (
        TAO_ServerRequest & req,
        void * servant_upcall,
        void * servant
      );
    
    static void _non_existent_skel (
        TAO_ServerRequest & req,
        void * servant_upcall,
        void * servant
      );
    
    static void _interface_skel (
        TAO_ServerRequest & req,
        void * servant_upcall,
        void * servant
      );
    
    static void _component_skel (
        TAO_ServerRequest & req,
        void * servant_upcall,
        void * servant
      );
    
    static void _repository_id_skel (
        TAO_ServerRequest & req,
        void * servant_upcall,
        void * servant);
    
    virtual void _dispatch (
        TAO_ServerRequest & req,
        void * servant_upcall);
    
    ::Test::Third *_this (void);
    
    virtual const char* _interface_repository_id (void) const;
    
    // TAO_IDL - Generated from
    // be/be_visitor_operation/operation_sh.cpp:45
    
    virtual void method (
        void) = 0;
    
    static void method_skel (
        TAO_ServerRequest & server_request,
        void * servant_upcall,
        void * servant
      );
  };

// TAO_IDL - Generated from
// be/be_visitor_module/module_sh.cpp:80

} // module Test

// TAO_IDL - Generated from 
// be/be_codegen.cpp:1288


#if defined (__ACE_INLINE__)
#include "TestS.inl"
#endif /* defined INLINE */

#endif /* ifndef */

