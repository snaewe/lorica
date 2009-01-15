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

#include "Callback_i.h"
#include <ace/Get_Opt.h>
#include <ace/OS.h>
#include <ace/Task.h>
#include "lorica/ReferenceMapperC.h"

const char *lorica_corbaloc = "corbaloc::localhost:9951/LORICA_REFERENCE_MAPPER";
char server_corbaloc[512] = { '\0' };

int
parse_args (int argc, char *argv[])
{
        ACE_Get_Opt get_opts (argc, argv, "k:");
	const char *server_ip_addr = NULL;
        int c;

        while ((c = get_opts ()) != -1) {
                switch (c)
                {
                case 'k':
                        server_ip_addr = get_opts.optarg;
                        break;
                case '?':
                default:
                        ACE_ERROR_RETURN ((LM_ERROR,
                                           "usage:  %s "
                                           "-k <Server IP Address>"
                                           "\n",
                                           argv [0]),
                                          -1);
                        break;
                }
        }
        if (!server_ip_addr) {
                ACE_ERROR_RETURN ((LM_ERROR,
                                   "usage:  %s "
                                   "-k <Server IP Address>"
                                   "\n",
                                   argv [0]),
                                  -1);
        }
        sprintf(server_corbaloc, "corbaloc::%s:20951/Server", server_ip_addr);

        // Indicates sucessful parsing of the command line
        return 0;
}

static ::Lorica::ReferenceMapper_ptr
get_lorica(CORBA::ORB_ptr orb)
{
        CORBA::Object_var obj = CORBA::Object::_nil();
        ::Lorica::ReferenceMapper_var mapper = Lorica::ReferenceMapper::_nil();

        try {
                obj = orb->string_to_object(lorica_corbaloc);
        }
        catch (...) {
                goto no_lorica;
        }
        try {
                mapper = ::Lorica::ReferenceMapper::_narrow(obj.in());
        }
        catch (const CORBA::Exception &e) {
                e._tao_print_exception("could not narrow to ::BRUTUS::BrutusLogOn\n");
                goto no_lorica;
        }
        catch (...) {
                printf("ERROR - Unknown exception caught in _narrow()\n");
                goto no_lorica;
        }
        if (CORBA::is_nil(mapper.in())) 
                goto no_lorica;

        try {
                if (mapper->_non_existent())
                        goto no_lorica;
        }
        catch (...) {
                goto no_lorica;
        }

        goto lorica_ok;
        
no_lorica:
        return ::Lorica::ReferenceMapper::_nil();
        
lorica_ok:
        return mapper._retn();
}

// thread entry class
class ORB_Thread_Base : public ACE_Task_Base {
public:
        ORB_Thread_Base(void)
                {
                };

        void set_orb(CORBA::ORB_ptr Broker)
                {
                        orb_ = CORBA::ORB::_duplicate(Broker);
                };

        virtual int svc(void)
                {
                        if (CORBA::is_nil(orb_))
                                throw 1;

                        orb_->run();

                        return 0;
                };

private:
        CORBA::ORB_var orb_;
};

int
main (int argc, char *argv[])
{
        ORB_Thread_Base orb_thread;

        try {
                CORBA::ORB_var orb = CORBA::ORB_init(argc, argv, "");

                CORBA::Object_var poa_object = orb->resolve_initial_references("RootPOA");

                if (CORBA::is_nil(poa_object.in())) {
                        ACE_ERROR_RETURN ((LM_ERROR,
                                           " (%P|%t) Unable to initialize the POA.\n"),
                                          1);
                }

                PortableServer::POA_var root_poa = PortableServer::POA::_narrow (poa_object.in ());

                PortableServer::POAManager_var poa_manager = root_poa->the_POAManager ();

                if (parse_args (argc, argv) != 0) {
                        ACE_ERROR_RETURN ((LM_ERROR,
                                           " (%P|%t) Unable to parse arguments.\n"),
                                          1);
                }

                // Get the server reference...
                CORBA::Object_var obj = CORBA::Object::_nil();
                Test::Server_var server_mapped = Test::Server::_nil();
                ::Lorica::ReferenceMapper_var lorica = get_lorica(orb.in());
                if (CORBA::is_nil(lorica)) {
                        ACE_ERROR_RETURN ((LM_ERROR,
                                           " (%P|%t) Unable to acquire Lorica reference.\n"),
                                          1);
                }

                try {
                        obj = lorica->as_client_with_corbaloc(server_corbaloc, 
                                                              "IDL:test.lorica/Test/Server:1.0");
                }
                catch (const CORBA::Exception &e) {
                        e._tao_print_exception("Exception from as_client_with_corbaloc()\n");
                        ACE_ERROR_RETURN ((LM_ERROR,
                                           " (%P|%t) Unable to map_as_client_with_corbaloc()\n"),
                                          1);
                }
                catch (...) {
                        ACE_ERROR_RETURN ((LM_ERROR,
                                           " (%P|%t) Unable to map_as_client_with_corbaloc()\n"),
                                          1);
                }

                try {
                        server_mapped = Test::Server::_narrow(obj.in());
                }
                catch (const CORBA::Exception &e) {
                        e._tao_print_exception("Exception from narrow()\n");
                        ACE_ERROR_RETURN ((LM_ERROR,
                                           " (%P|%t) Unable to narrow\n"),
                                          1);
                }
                catch (...) {
                        ACE_ERROR_RETURN ((LM_ERROR,
                                           " (%P|%t) Unable to narrow - unkown exception\n"),
                                          1);
                }

                if (CORBA::is_nil(server_mapped)) {
                        ACE_ERROR_RETURN ((LM_ERROR,
                                           " (%P|%t) Could not map in lorica\n"),
                                          1);
                }

		Test_CallBack_i *callback_impl = new Test_CallBack_i;
		::Test::CallBack_var callback = ::Test::CallBack::_nil();
                {
                        PortableServer::ObjectId_var oid = root_poa->activate_object(callback_impl);
                        CORBA::Object_var obj = root_poa->id_to_reference(oid);
                        callback = Test::CallBack::_narrow(obj);
                }

                // Activate poa manager
                poa_manager->activate ();

		orb_thread.set_orb(orb.in());
		orb_thread.activate();

		if (!server_mapped->receive_call(callback.in())) {
                        ACE_ERROR_RETURN ((LM_ERROR,
                                           " (%P|%t) Callback failed\n"),
                                          1);
                }

                orb->destroy ();
		orb_thread.wait();
        }
        catch (const CORBA::Exception& ex)
        {
                ex._tao_print_exception ("Exception caught:");
                return 1;
        }

        return 0;
};
