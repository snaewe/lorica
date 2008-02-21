# 
#  Copyright (C) 2007-2008 OMC Denmark ApS
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#  MA 02111-1307 USA

# serial 2

dnl AX_LORICA_CHECK_ACETAO()
dnl Checks for all programs, libraries and header files that are 
dnl needed for the build.
dnl
AC_DEFUN([AX_LORICA_CHECK_ACETAO],
[ 
  EXTRA_PATH="$1"
  LORICA_ACETAO_CHECKS_OUT="yes"

  dnl State all used ACE and TAO headers
  ACE_HEADERS="ace/ACE.h ace/Auto_Ptr.h ace/Dynamic_Service.h ace/Get_Opt.h ace/INET_Addr.h ace/Log_Msg.h ace/Min_Max.h ace/Mutex.h ace/OS_NS_errno.h ace/OS_NS_stdio.h ace/OS_NS_stdlib.h ace/OS_NS_string.h ace/OS_NS_strings.h ace/OS_NS_unistd.h ace/Service_Config.h ace/Service_Gestalt.h ace/Signal.h ace/SString.h ace/streams.h ace/Task.h ace/Thread.h ace/Time_Value.h TAO/orbsvcs/orbsvcs/IFRService/ComponentRepository_i.h TAO/orbsvcs/orbsvcs/IFRService/IFR_ComponentsS.h TAO/orbsvcs/orbsvcs/IFRService/IFR_Service_Utils.h TAO/orbsvcs/orbsvcs/IFRService/Options.h TAO/orbsvcs/orbsvcs/IOR_Multicast.h TAO/orbsvcs/orbsvcs/SecurityC.h TAO/orbsvcs/orbsvcs/SSLIOPC.h TAO/tao/AnyTypeCode/Any_Impl.h TAO/tao/AnyTypeCode/Any_Unknown_IDL_Type.h TAO/tao/AnyTypeCode/NVList.h TAO/tao/corbafwd.h TAO/tao/CORBA_String.h TAO/tao/debug.h TAO/tao/DynamicAny/DynamicAny.h TAO/tao/DynamicInterface/AMH_DSI_Response_Handler.h TAO/tao/DynamicInterface/Request.h TAO/tao/DynamicInterface/Server_Request.h TAO/tao/EndpointPolicy/EndpointPolicy.h TAO/tao/EndpointPolicy/IIOPEndpointValue_i.h TAO/tao/Exception.h TAO/tao/IFR_Client/IFR_BasicC.h TAO/tao/IORTable/IORTable.h TAO/tao/Messaging/AMH_Response_Handler.h TAO/tao/MProfile.h TAO/tao/Object.h TAO/tao/OctetSeqC.h TAO/tao/ORB_Constants.h TAO/tao/ORB_Core.h TAO/tao/ORBInitializer_Registry.h TAO/tao/PI/PI.h TAO/tao/PortableServer/POAManagerFactory.h TAO/tao/PortableServer/PortableServer.h TAO/tao/PortableServer/Servant_Base.h TAO/tao/Profile.h TAO/tao/Stub.h TAO/tao/SystemException.h TAO/tao/Tagged_Components.h TAO/tao/TAO_Server_Request.h TAO/tao/Thread_Lane_Resources.h"

  dnl The order is vitally important
  ACE_LIBS="ACE ACE_SSL TAO TAO_Codeset TAO_AnyTypeCode TAO_CodecFactory TAO_Valuetype TAO_DynamicAny TAO_PI TAO_PortableServer TAO_Messaging TAO_DynamicInterface TAO_EndpointPolicy TAO_IFR_Client TAO_IORTable TAO_Svc_Utils TAO_TypeCodeFactory TAO_IFRService TAO_PI_Server TAO_Security TAO_SSLIOP"

  dnl State all needed TAO programs
  TAO_EXECUTABLES="gperf tao_idl tao_ifr"

  dnl Check for needed TAO programs
  for exefile in $TAO_EXECUTABLES
  do
      if test "x$EXTRA_PATH" = "x"; then
      	 AC_CHECK_PROG(HAVE_TAO_EXE, [$exefile], yes, no)
      else
         AC_CHECK_PROG(HAVE_TAO_EXE, [$exefile], yes, no, [$EXTRA_PATH:$PATH])
      fi
      if test "x$HAVE_TAO_EXE" = "xno"; then
      	 LORICA_ACETAO_CHECKS_OUT="no"
      fi
   done

   dnl ACE and TAO libraries
   for libfile in $ACE_LIBS
   do
	AC_CHECK_LIB($libfile, [main], [], [LORICA_ACETAO_CHECKS_OUT="no"])
   done

   dnl ACE and TAO headers
   for headerfile in $ACE_HEADERS
   do
	AC_CHECK_HEADER([$headerfile], [], [LORICA_ACETAO_CHECKS_OUT="no"])
   done
])

dnl AX_LORICA_ACETAO_ADAPT()
dnl Tries to adapt the ACE+TAO variables to the platform at hand.
dnl
dnl CONF_ACE_ROOT, ACETAO_CPPFLAGS and ACETAO_LDFLAGS will have
dnl been set upon exit. The flags must be prefixed to CPPFLAGS 
dnl and LDFLAGS respectively.
dnl
dnl AM_CONDITIONAL(HAVE_ACE_ROOT) will be set to true if the
dnl environment variable ACE_ROOT is set or if enable_tao_build 
dnl is set.
dnl 
dnl AM_CONDITIONAL(HAVE_CONF_PATH) will be set to true if the
dnl CONF_ACE_ROOT is non-nil. CONF_PATH will be set accordingly.
dnl
dnl The big while loop below implements this logic:
dnl
dnl Level 0: Do the $ACE_ROOT environment variable exist
dnl Level 1: Check if ACE_ROOT is suitable for building Lorica
dnl Level 2: May we build ACE and TAO
dnl Level 3: Check if ACE and TAO is installed in the system
dnl Level 4: Build ACE and TAO
dnl Level 5: Build Lorica
dnl
dnl TRUE   Level   FALSE
dnl  1 <==   0   ==> 2
dnl  5 <==   1   ==> 2
dnl  4 <==   2   ==> 3
dnl  5 <==   3   ==> ERROR
dnl  5 <==   4   ==> ERROR
dnl OK <==   5   ==> ERROR
dnl 
dnl In this way it is guarenteed that the outcome of any 
dnl configuration is OK or a well defined terminating error.
dnl 
AC_DEFUN([AX_LORICA_ACETAO_ADAPT],
[ 
  dnl ACE and TAO specific flags
  ACETAO_CPPFLAGS=""
  ACETAO_LDFLAGS=""
 
  CONF_ACE_ROOT=""
  CONF_LD_PATH=""

  dnl Must we install ACE and TAO?
  AM_CONDITIONAL(LORICA_MUST_INSTALL_ACETAO, true)

  dnl Must we build ACE and TAO?
  AM_CONDITIONAL(LORICA_MUST_BUILD_ACETAO, false)

  AM_CONDITIONAL(HAVE_ACE_ROOT, false)
  AM_CONDITIONAL(HAVE_CONF_PATH, false)
  AM_CONDITIONAL(HAVE_CONF_LD_PATH, false)

  dnl Take care of CPPFLAGS
  oldCPPFLAGS="$CPPFLAGS"
  CPPFLAGS=""
  
  dnl Take care of LDFLAGS
  oldLDFLAGS="$LDFLAGS"
  LDFLAGS=""

  while [ true ]; do # level 5
  	while [ true ]; do # level 4
  	      while [ true ]; do # level 3
  	      	    while [ true ]; do # level 2
  	      	    	  while [ true ]; do # level 1                                
                                while [ true ]; do # level 0 - do $ACE_ROOT exists?
                                      if test "x$ACE_ROOT" != "x"; then
                                         AM_CONDITIONAL(HAVE_ACE_ROOT, true)
                                         CONF_ACE_ROOT="$ACE_ROOT"
                                         break 1 # check ACE_ROOT
                                      else
                                         break 2 # check sys
                                      fi
                                done
                                # This is level 1 - check ACE_ROOT
                                ACETAO_CPPFLAGS="-I$CONF_ACE_ROOT -I$CONF_ACE_ROOT/TAO -I$CONF_ACE_ROOT/TAO/orbsvcs"
                                CPPFLAGS="$ACETAO_CPPFLAGS"

                                ACETAO_LDFLAGS="-L$CONF_ACE_ROOT/lib"
                                LDFLAGS="$oldLDFLAGS $ACETAO_LDFLAGS"

                                AX_LORICA_CHECK_ACETAO($ACE_ROOT/bin)
                                if test "x$LORICA_ACETAO_CHECKS_OUT" = "xyes"; then
                                    AM_CONDITIONAL(HAVE_CONF_LD_PATH, true)
                                    break 4 # goto level 5
                                else
                                    ACETAO_CPPFLAGS=""
                                    ACETAO_LDFLAGS=""
                                    break 1 # goto level 2
                                fi
                          done
                          # This is level 2
                          if test "x$enable_tao_build" = "xyes"; then
                             AM_CONDITIONAL(HAVE_ACE_ROOT, true)
                             AM_CONDITIONAL(HAVE_CONF_LD_PATH, true)
                             if test "x$ACE_ROOT" != "x"; then
                                CONF_ACE_ROOT="$ACE_ROOT"
                             else
                                CONF_ACE_ROOT="`pwd`/ACE_wrappers"
                             fi
                             ACETAO_CPPFLAGS="-I$CONF_ACE_ROOT -I$CONF_ACE_ROOT/TAO -I$CONF_ACE_ROOT/TAO/orbsvcs"
                             ACETAO_LDFLAGS="-L$CONF_ACE_ROOT/lib"
                             break 2 # goto level 4
                          else
                             ACETAO_CPPFLAGS=""
                             ACETAO_LDFLAGS=""
                             break 1 # goto level 3
                          fi
                    done
                    # This is level 3 - check sys
                    ACETAO_CPPFLAGS="-I/usr/include/TAO -I/usr/include/TAO/orbsvcs"
                    CPPFLAGS="$ACETAO_CPPFLAGS"

                    ACETAO_LDFLAGS=""
                    LDFLAGS="$oldLDFLAGS $ACETAO_LDFLAGS"

                    AX_LORICA_CHECK_ACETAO()
                    if test "x$LORICA_ACETAO_CHECKS_OUT" = "xyes"; then
                       AM_CONDITIONAL(LORICA_MUST_INSTALL_ACETAO, false)
                       break 2 # goto level 5
                    else
                       ACETAO_CPPFLAGS=""
                       ACETAO_LDFLAGS=""
                       AC_MSG_ERROR([[You need ACE and TAO to build and run Lorica - consider enabling tao-build]]) # Error
                    fi
                    break # not really needed... 
              done
              # This is level 4
              AM_CONDITIONAL(LORICA_MUST_BUILD_ACETAO, true)
              break
        done
        # This is level 5 superflous?
        break
  done

  if test "x$CONF_ACE_ROOT" != "x"; then
     AM_CONDITIONAL(HAVE_CONF_PATH, true)
     CONF_PATH="$CONF_ACE_ROOT/bin"
     CONF_LD_PATH="$CONF_ACE_ROOT/lib"
  fi

  # Effective ACE_ROOT
  AC_SUBST(CONF_PATH)
  AC_SUBST(CONF_ACE_ROOT)
  AC_SUBST(CONF_LD_PATH)

  dnl ACE+TAO flags
  AC_SUBST(ACETAO_CPPFLAGS)
  AC_SUBST(ACETAO_LDFLAGS)

  dnl Restore old CPPFLAGS
  CPPFLAGS="$oldCPPFLAGS"
  LDFLAGS="$oldLDFLAGS"
])
