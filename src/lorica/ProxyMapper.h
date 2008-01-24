/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 *    Lorica header file.
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

#ifndef PROXY_MAPPER_H
#define PROXY_MAPPER_H

#include <tao/PortableServer/PS_CurrentC.h>
#include <tao/PortableServer/POAC.h>
#include <tao/PortableServer/POAManagerC.h>
#include <ace/RB_Tree.h>
#include <ace/Mutex.h>

#include <string>
#include "Lorica_EvaluatorBase_Export.h"
#include "ReferenceMapValue.h"
#include "RMVByMapped.h"
//#include "RMVByNative.h"

class Lorica_MapperRegistry;

namespace Lorica
{
  class EvaluatorBase;
  class ProxyServant;

  struct MappedObjectId
  {
    char magic[4];
    char version;
    char in_out;
    ACE_UINT16 pid;
    ACE_UINT32 index;
    ACE_UINT32 hostid;
  };

  class Lorica_EvaluatorBase_Export ProxyMapper
  {
  public:
    ProxyMapper (Lorica_MapperRegistry &, const std::string &id);
    virtual ~ProxyMapper (void);

    /// Add an evalutor to the mapper. The mapper takes ownership of the
    /// evaluator reference.
    void add_evaluator (Lorica::EvaluatorBase *ev);

    /// Proxy Mappers are changed in a linked list, they are also reference
    /// counted. Take_mapper assumes ownership of the supplied proxy mapper.
    /// This method does not increase the reference count, but the instance
    /// supplied will have its count decremented when the list is destroyed.
    void take_mapper (ProxyMapper *p);

    /// Destroys the entire chain of proxy mappers. This works recursively,
    /// destroying the next element in the chain before destroying itself.
    /// Destruction is done by decrementing the reference count on itself.
    void destroy_chain (void);

    /// Returns a duplicate of the next reference in the chain. This means
    /// the receiver must ensure that it decrements the reference count
    /// when it is done with the value.
    ProxyMapper *next (void);

    /// Removes an arbitrary link from the chain of references.
    void remove_mapper (ProxyMapper *p);

    /// Initializer specific to proxy mappers. Called by the main during
    /// program initialization.
    virtual int proxy_mapper_init (PortableServer::POAManager_ptr outward,
                 PortableServer::POAManager_ptr inward,
                 CORBA::ORB_ptr orb);

    /// Add a new element to the table. Returns a mapped reference if the
    /// original reference's type is supported. If the native reference is
    /// already in the list, it's mapped value is returned. If the type is
    /// not supported, a nil reference is returned. This method is recursive
    /// to support the mapper registry. If the typeId is unknown, the call
    /// is forwarded to the next mapper in the chain.
    ReferenceMapValue * add_native (CORBA::Object_ptr native_ref,
            const std::string &typeId,
            bool out_facing,
            bool require_secure);

    /// Remove a previously mapped reference. This returns the value for
    /// the reference that was removed from the table.
    ReferenceMapValue * remove_mapped (CORBA::Object_ptr mapped_ref,
               const std::string &typeId,
               bool out_facing);

    /// returns true, and replaces the supplied mapped ref with a native
    /// ref if the mapped ref is known. Otherwise it returns false.
    bool native_for_mapped (CORBA::Object_ptr &ref,
          PortableServer::POA_ptr poa);

    CORBA::Object_ptr native_for_mapped_i (const CORBA::OctetSeq &idstr,
                   Lorica::ServerAgent_ptr &sa);

    /// returns the native reference corresponding to the mapped object
    /// based on the Object ID and POA of the current upcall. Returns
    /// a nil reference if not in an upcall.
    CORBA::Object_ptr current_native (Lorica::ServerAgent_ptr &agent);

    /// returns true, and replaces the supplied native ref with a mapped
    /// ref. This may involve adding a reference to this map, or another
    /// as required by the type. If no known evaluator supports the type
    /// the function returns false.
    bool mapped_for_native (CORBA::Object_ptr &ref,
          PortableServer::POA_ptr poa);

    /// Returns the evaluator associated with a given type ID. Returns null
    /// if the type is unknown.
    virtual EvaluatorBase * evaluator_for (const std::string &typeId);

    /// Returns the evaluator appropriate for the object supplied. The
    /// default behavior is to get the type ID from the argument's stub,
    /// and call evaluator_for(string). Specialized proxy mappers may
    /// directly return a specific evaluator without first extracting
    /// the type if the type info is not needed to choose the evaluator.
    virtual EvaluatorBase * evaluator_for (CORBA::Object_ptr native);

    /// Returns the POA which is not the supplied POA. When passed the
    /// in-facing POA, this returns the out-facing and vice-versa.
    PortableServer::POA_ptr other_POA (PortableServer::POA_ptr p);

    void incr_refcount(void);
    long decr_refcount(void);

    CORBA::ORB_ptr orb(void) const;

    /// Returns the POA and Object id values for an outward facing object
    /// reference. This is used by the reference mapper to allow insecure
    /// access to objects when security enforcement is enabled.
    bool poa_and_object_id (CORBA::Object_ptr mapped_ref,
          CORBA::OctetSeq *& poaid,
          PortableServer::ObjectId *& objid);


  protected:
    static MappedObjectId mapped_object_id_;
    virtual PortableServer::ObjectId * get_mapped_objectId(bool out_facing,
                       ACE_UINT32 &index);

    enum MappedStatus
    {
      ALREADY_MAPPED,
      REVERSE_MAPPED,
      NOT_MAPPED,
      MAPPER_ERROR
    };

    virtual MappedStatus already_mapped (CORBA::Object_ptr native,
                 bool out_facing,
                 ACE_UINT32 &index);

    virtual ProxyServant *make_default_servant(void) = 0;

    bool supports_secure (CORBA::Object_ptr native);

    ReferenceMapValue * add_native_unchecked (CORBA::Object_ptr native_ref,
                const std::string &typeId,
                bool out_facing,
                bool require_secure);

    /// a linked list of evaluators known by this mapper
    EvaluatorBase * evaluator_head_;

    CORBA::ORB_var orb_;
    PortableServer::Current_var poa_current_;

  private:
    /// The POA used to map outward facing object references
    PortableServer::POA_var out_facing_poa_;
    /// The POA used to map inward facing object references
    PortableServer::POA_var in_facing_poa_;

    /// a collection of ReferenceMapValues keyed by the mapped obj's ID.
    RMVByMapped *mapped_values_;
    /// The same mapped values keyed by the stringified native OBJref
//     RMVByNative *native_values_;

    /// the collection of peer mappers
    Lorica_MapperRegistry &registry_;

    /// a unique name that is used to identify the inside and outside
    /// poas.
    std::string id_;

    /// the reference count
    long ref_count_;

    /// the next member of the linked list
    ProxyMapper *next_;
  };
}

#endif // PROXY_MAPPER_H
