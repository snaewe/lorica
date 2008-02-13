#
#  Additional M4 autoconf macros for Lorica.
#
#  This file also contain M4 autoconf macros which has been copied 
#  and/or modified from other software packages and to which OMC
#  Denmark ApS do not hold any or only partial copyrights. 
#
#  These macros are hereby distributed under their original copyright 
#  and license terms. The part(s) of this file holding such macros 
#  are clearly marked as such. 
#
#  OMC Denmark ApS are the sole copyright owner of all other parts 
#  of this file which are subject to the license terms of the GNU 
#  General Public License as specified directly following this line.
# 
#  Copyright (C) 2007 OMC Denmark ApS
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

dnl AM_LORICA_CHECK_PLATFORM()
dnl Tries to determine the platform and defines the pretty LORICA_DIST_RELEASE
dnl descriptive string as well as these AM_CONDITIONALS:
dnl
dnl LORICA_FEDORA
dnl LORICA_GENTOO
dnl LORICA_DEBIAN
dnl LORICA_SUSE
dnl
dnl These all defaults to false unless we hapen to be on one of those 
dnl platforms .The not so pretty string lorica_target is also defined
dnl by this macro.
dnl
AC_DEFUN([AM_LORICA_CHECK_PLATFORM],
[ LORICA_DIST_RELEASE="UNKNOWN"
  AM_CONDITIONAL(LORICA_DARWIN,         false)
  AM_CONDITIONAL(LORICA_DARWIN_TIGER,   false)
  AM_CONDITIONAL(LORICA_DARWIN_LEOPARD, false)
  AM_CONDITIONAL(LORICA_LINUX,          false)
  AM_CONDITIONAL(LORICA_FEDORA,         false)
  AM_CONDITIONAL(LORICA_SUSE,           false)
  AM_CONDITIONAL(LORICA_GENTOO,         false)
  AM_CONDITIONAL(LORICA_DEBIAN,         false)
  AC_ARG_ENABLE(lorica-target,
		[AS_HELP_STRING([--enable-lorica-target[[[[=fedora/opensuse102/opensuse103/gentoo/ubuntu/darwin]]]]], [Distribution target [default=UNKNOWN]])],
	      	[],
	      	enable_lorica_target=UNKNOWN)

  dnl Determine OS
  system_name=`uname -s`
  case "x$system_name" in
        xDarwin)
		AM_CONDITIONAL(LORICA_DARWIN, true)
		AC_DEFINE([LORICA_DARWIN], [1], [Define if this is some kind of Darwin (MacOS)])
		enable_lorica_target=darwin
       		;;
	xLinux)
		AM_CONDITIONAL(LORICA_LINUX, true)
		AC_DEFINE([LORICA_LINUX], [1], [Define if this is some kind of Linux])
		;;
 	*)
		;;
  esac	

  dnl Determine system sub-type
  case "x$enable_lorica_target" in
	xfedora)
		lorica_target=Fedora
		;;
	xopensuse102)
		lorica_target=OpenSUSE_10_2
		;;
	xopensuse103)
		lorica_target=OpenSUSE_10_3
		;;
	xgentoo)
		lorica_target=Gentoo
		;;
	xubuntu)
		lorica_target=Ubuntu
		;;
	xdarwin)
		lorica_target=Darwin
		;;
	*)
		lorica_target="UNKNOWN"
		if test -f /etc/gentoo-release; then
			lorica_target=Gentoo
		fi
		if test -f /etc/redhat-release; then
			is_fedora="`grep Fedora /etc/redhat-release >/dev/null ; echo $?`"
			if test "x$is_fedora" = "x0"; then
			   	lorica_target=Fedora
			fi
		else
			AC_CHECK_PROG(HAVE_LSB_RELEASE, [lsb_release], yes, no)
			if test "x$HAVE_LSB_RELEASE" = "xyes"; then
				lsb_name=`lsb_release -si`
				lsb_version=`lsb_release -sr`
				case "x$lsb_name" in
					"xSUSE LINUX")
						if test "x$lsb_version" = "x10.2"; then
							lorica_target="OpenSUSE_10_2"
						fi
						if test "x$lsb_version" = "x10.3"; then
							lorica_target="OpenSUSE_10_3"
						fi
						;;
					xFedoraCore)
						lorica_target="Fedora"
						;;
					xUbuntu)
						lorica_target="Ubuntu"
						;;
				esac
			fi
		fi
		;;
  esac

  dnl Determine system sub-type variant
  case "x$lorica_target" in
        xDarwin)
		system_release=`uname -r`
		system_version=${system_release:0:1}
		case "x$system_version" in
		     x8)
			AM_CONDITIONAL(LORICA_DARWIN_TIGER, true)
			LORICA_DIST_RELEASE="Mac%20OS%20X%20Tiger"
			;;
		     x9)
			AM_CONDITIONAL(LORICA_DARWIN_LEOPARD, true)
			LORICA_DIST_RELEASE="Mac%20OS%20X%20Leopard"
			;;
		     *)
			AC_MSG_WARN([[Unable to determine Darwin release]])
			;;
		esac
		;;
	xFedora)
		AM_CONDITIONAL(LORICA_FEDORA, true)
		AC_DEFINE([LORICA_FEDORA], [1], [Define if this is a Fedora based distribution])
		if test -f /etc/redhat-release; then
			fedora_release="`grep Stentz /etc/redhat-release >/dev/null ; echo $?`"
			if test "x$fedora_release" = "x0"; then
				LORICA_DIST_RELEASE="Fedora%20Core%204"
			fi
			fedora_release="`grep Bordeaux /etc/redhat-release >/dev/null ; echo $?`"
			if test "x$fedora_release" = "x0"; then
				LORICA_DIST_RELEASE="Fedora%20Core%205"
			fi
			fedora_release="`grep Zod /etc/redhat-release >/dev/null ; echo $?`"
			if test "x$fedora_release" = "x0"; then
				LORICA_DIST_RELEASE="Fedora%20Core%206"
			fi
			fedora_release="`grep Moonshine /etc/redhat-release >/dev/null ; echo $?`"
			if test "x$fedora_release" = "x0"; then
				LORICA_DIST_RELEASE="Fedora%207"
			fi
			fedora_release="`grep Werewolf /etc/redhat-release >/dev/null ; echo $?`"
			if test "x$fedora_release" = "x0"; then
				LORICA_DIST_RELEASE="Fedora%208"
			fi
			fedora_release="`grep Rawhide /etc/redhat-release >/dev/null ; echo $?`"
			if test "x$fedora_release" = "x0"; then
				LORICA_DIST_RELEASE="Rawhide"
			fi
			if test "x$LORICA_DIST_RELEASE" = "x"; then
				AC_MSG_WARN([[Unable to determine Fedora release number]])
			fi
		fi
		;;
	xOpenSUSE_10_2)
		AM_CONDITIONAL(LORICA_SUSE, true)
		AC_DEFINE([LORICA_SUSE], [1], [Define if this is an SUSE based distribution])
		LORICA_DIST_RELEASE="OpenSUSE%2010.2"
		;;
	xOpenSUSE_10_3)
		AM_CONDITIONAL(LORICA_SUSE, true)
		AC_DEFINE([LORICA_SUSE], [1], [Define if this is an SUSE based distribution])
		LORICA_DIST_RELEASE="OpenSUSE%2010.3"
		;;
	xGentoo)
		AM_CONDITIONAL(LORICA_GENTOO, true)
		AC_DEFINE([LORICA_GENTOO], [1], [Define if this is a Gentoo based distribution])
		LORICA_DIST_RELEASE="Gentoo"
		;;
	xUbuntu)
		AM_CONDITIONAL(LORICA_DEBIAN, true)
		AC_DEFINE([LORICA_DEBIAN], [1], [Define if this is a Debian based distribution])
		LORICA_DIST_RELEASE="Ubuntu"
		;;
	xUNKNOWN)
		LORICA_DIST_RELEASE="UNKNOWN"
		;;
	*)
		AC_MSG_ERROR([[Defective distribution detection]])
		;;
  esac
  AC_SUBST(LORICA_DIST_RELEASE)
])

dnl AM_LORICA_PLATFORM_ADAPT()
dnl Tries to adapt the build variables to the platform at hand.
dnl
AC_DEFUN([AM_LORICA_PLATFORM_ADAPT],
[ if test "x$LORICA_DIST_RELEASE" = "xUNKNOWN"; then
     AM_LORICA_CHECK_PLATFORM()
  fi
  case "$lorica_target" in
	OpenSUSE_10_2)
		RPM_SPEC_SUSE_PREFIX='%define _prefix /opt/gnome'
		RPM_SPEC_SUSE_DOCDIR='%define _docdir %{_prefix}/share/doc/packages'
		RPM_SPEC_LIBIDL='libidl'
		RPM_SPEC_ORBIT2='orbit2'
		RPM_SPEC_CONFIGURE='%configure --enable-lorica-target=opensuse102 --enable-lorica-dist=yes --enable-lorica-debug=yes --enable-lorica-devel=yes --prefix=/opt/gnome --libdir=/opt/gnome/%{_lib} --libexecdir=%{_libexecdir}'
		RPM_SPEC_BUILDREQUIRES_1='BuildRequires:   mDNSResponder-lib'
		RPM_SPEC_BUILDREQUIRES_2='BuildRequires:   gnome-common'
		RPM_SPEC_BUILDREQUIRES_3="BuildRequires:   $RPM_SPEC_LIBIDL-devel"
		RPM_SPEC_DEBUGINFO='%debug_package'
		;;
	OpenSUSE_10_3)
		RPM_SPEC_SUSE_PREFIX=''
		RPM_SPEC_SUSE_DOCDIR=''
		RPM_SPEC_LIBIDL='libidl'
		RPM_SPEC_ORBIT2='orbit2'
		RPM_SPEC_CONFIGURE='%configure --enable-lorica-target=opensuse103 --enable-lorica-dist=yes --enable-lorica-debug=yes --enable-lorica-devel=yes --prefix=/opt/gnome --libdir=/opt/gnome/%{_lib} --libexecdir=%{_libexecdir}'
		RPM_SPEC_BUILDREQUIRES_1='BuildRequires:   avahi'
		RPM_SPEC_BUILDREQUIRES_2='BuildRequires:   gnome-common'
		RPM_SPEC_BUILDREQUIRES_3="BuildRequires:   $RPM_SPEC_LIBIDL-devel"
		RPM_SPEC_DEBUGINFO='%debug_package'
		;;
	Fedora)
		RPM_SPEC_SUSE_PREFIX=''
		RPM_SPEC_SUSE_DOCDIR=''
		RPM_SPEC_LIBIDL='libIDL'
		RPM_SPEC_ORBIT2='ORBit2'
		RPM_SPEC_CONFIGURE='%configure --enable-lorica-target=fedora --enable-lorica-dist=yes --enable-lorica-debug=yes --enable-lorica-devel=yes'
		RPM_SPEC_BUILDREQUIRES_1=''
		RPM_SPEC_BUILDREQUIRES_2=''
		RPM_SPEC_BUILDREQUIRES_3=''
		RPM_SPEC_DEBUGINFO=''
		;;
	*)
		RPM_SPEC_SUSE_PREFIX=''
		RPM_SPEC_SUSE_DOCDIR=''
		RPM_SPEC_LIBIDL='libIDL'
		RPM_SPEC_ORBIT2='ORBit2'
		RPM_SPEC_CONFIGURE='%configure --enable-lorica-target=fedora --enable-lorica-dist=yes --enable-lorica-debug=yes --enable-lorica-devel=yes'
		RPM_SPEC_BUILDREQUIRES_1=''
		RPM_SPEC_BUILDREQUIRES_2=''
		RPM_SPEC_BUILDREQUIRES_3=''
		RPM_SPEC_DEBUGINFO=''
		;;
  esac
  dnl This workaround can be used in automake 1.10 and above:
  dnl _AM_SUBST_NOTMAKE([RPM_SPEC_BUILDREQUIRES])
  AC_SUBST([RPM_SPEC_BUILDREQUIRES_1])
  AC_SUBST([RPM_SPEC_BUILDREQUIRES_2])
  AC_SUBST([RPM_SPEC_BUILDREQUIRES_3])
  AC_SUBST([RPM_SPEC_BUILDREQUIRES_4])
  AC_SUBST([RPM_SPEC_SUSE_PREFIX])
  AC_SUBST([RPM_SPEC_SUSE_DOCDIR])
  AC_SUBST([RPM_SPEC_LIBIDL])
  AC_SUBST([RPM_SPEC_ORBIT2])
  AC_SUBST([RPM_SPEC_CONFIGURE])
  AC_SUBST([RPM_SPEC_DEBUGINFO])

  dnl For the dist-deb targets
  AC_CHECK_PROG(HAVE_DATE, [date], yes, no)
  if test "$lorica_target" = "Ubuntu"; then
	  if test "x$HAVE_DATE" = "xyes"; then
		  RFC_822_DATE=`date -R`
		  RFC_822_YEAR=`date +%Y`
	  else
		  AC_MSG_ERROR([You need the date(1) program to build Lorica on $lorica_target])
	  fi
  fi
  AC_SUBST(RFC_822_DATE)
  AC_SUBST(RFC_822_YEAR)

  dnl different pkgconfig configuration directory in OpenSUSE 10.2
  if test "$lorica_target" = "OpenSUSE_10_2"; then
	  PKGCONFIG_DIR='/opt/gnome/lib/pkgconfig/'
  else
	  if test "$lorica_target" = "OpenSUSE_10_3"; then
		  PKGCONFIG_DIR='/opt/gnome/lib/pkgconfig/'
	  else
		  PKGCONFIG_DIR='/usr/lib*/pkgconfig/'
	  fi
  fi
])


#####################################################################

dnl AM_LORICA_CHECK_ACETAO()
dnl Checks for programs, libraries and headers
dnl
AC_DEFUN([AM_LORICA_CHECK_ACETAO],
[ 
  EXTRA_PATH="$1"
  LORICA_ACETAO_CHECKS_OUT="yes"

  dnl State all used ACE and TAO headers
  ACE_HEADERS="ace/ACE.h ace/Auto_Ptr.h ace/Dynamic_Service.h ace/Get_Opt.h ace/INET_Addr.h ace/Log_Msg.h ace/Min_Max.h ace/Mutex.h ace/OS_NS_errno.h ace/OS_NS_stdio.h ace/OS_NS_stdlib.h ace/OS_NS_string.h ace/OS_NS_strings.h ace/OS_NS_unistd.h ace/Service_Config.h ace/Service_Gestalt.h ace/Signal.h ace/SString.h ace/streams.h ace/Task.h ace/Thread.h ace/Time_Value.h TAO/orbsvcs/orbsvcs/IFRService/ComponentRepository_i.h TAO/orbsvcs/orbsvcs/IFRService/IFR_ComponentsS.h TAO/orbsvcs/orbsvcs/IFRService/IFR_Service_Utils.h TAO/orbsvcs/orbsvcs/IFRService/Options.h TAO/orbsvcs/orbsvcs/IOR_Multicast.h TAO/orbsvcs/orbsvcs/SecurityC.h TAO/orbsvcs/orbsvcs/SSLIOPC.h TAO/tao/AnyTypeCode/Any_Impl.h TAO/tao/AnyTypeCode/Any_Unknown_IDL_Type.h TAO/tao/AnyTypeCode/NVList.h TAO/tao/corbafwd.h TAO/tao/CORBA_String.h TAO/tao/debug.h TAO/tao/DynamicAny/DynamicAny.h TAO/tao/DynamicInterface/AMH_DSI_Response_Handler.h TAO/tao/DynamicInterface/Request.h TAO/tao/DynamicInterface/Server_Request.h TAO/tao/EndpointPolicy/EndpointPolicy.h TAO/tao/EndpointPolicy/IIOPEndpointValue_i.h TAO/tao/Exception.h TAO/tao/IFR_Client/IFR_BasicC.h TAO/tao/IORTable/IORTable.h TAO/tao/Messaging/AMH_Response_Handler.h TAO/tao/MProfile.h TAO/tao/Object.h TAO/tao/OctetSeqC.h TAO/tao/ORB_Constants.h TAO/tao/ORB_Core.h TAO/tao/ORBInitializer_Registry.h TAO/tao/PI/PI.h TAO/tao/PortableServer/POAManagerFactory.h TAO/tao/PortableServer/PortableServer.h TAO/tao/PortableServer/Servant_Base.h TAO/tao/Profile.h TAO/tao/Stub.h TAO/tao/SystemException.h TAO/tao/Tagged_Components.h TAO/tao/TAO_Server_Request.h TAO/tao/Thread_Lane_Resources.h"

  dnl The order is vitally important
  ACE_LIBS="ACE ACE_SSL TAO TAO_Codeset TAO_AnyTypeCode TAO_CodecFactory TAO_Valuetype TAO_DynamicAny TAO_PI TAO_PortableServer TAO_Messaging TAO_DynamicInterface TAO_EndpointPolicy TAO_IFR_Client TAO_IORTable TAO_Svc_Utils TAO_TypeCodeFactory TAO_IFRService TAO_PI_Server TAO_Security TAO_SSLIOP"

  dnl State all needed TAO programs
  TAO_EXECUTABLES="tao_gperf tao_idl tao_ifr"

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

dnl AM_LORICA_ACETAO_ADAPT()
dnl Tries to adapt the ACE+TAO variables to the platform at hand.
dnl
dnl CONF_ACE_ROOT, ACETAO_CPPFLAGS and ACETAO_LDFLAGS will have
dnl been set upon exit. The flags must be prefixed to CXXFLAGS 
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
AC_DEFUN([AM_LORICA_ACETAO_ADAPT],
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

  dnl Take care of CXXFLAGS
  oldCXXFLAGS="$CXXFLAGS"
  CXXFLAGS=""
  
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
                                CXXFLAGS="$ACETAO_CPPFLAGS"

                                ACETAO_LDFLAGS="-L$CONF_ACE_ROOT/lib"
                                LDFLAGS="$oldLDFLAGS $ACETAO_LDFLAGS"

                                AM_LORICA_CHECK_ACETAO($ACE_ROOT/bin)
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
                             CONF_ACE_ROOT="`pwd`/ACE_wrappers"
                             AM_CONDITIONAL(HAVE_CONF_LD_PATH, true)
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
                    CXXFLAGS="$ACETAO_CPPFLAGS"

                    ACETAO_LDFLAGS=""
                    LDFLAGS="$oldLDFLAGS $ACETAO_LDFLAGS"

                    AM_LORICA_CHECK_ACETAO()
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

  dnl Restore old CXXFLAGS
  CXXFLAGS="$oldCXXFLAGS"
  LDFLAGS="$oldLDFLAGS"
])

#####################################################################
#                                                                   #
#                Start of section ACX_PTHREAD                       #
#                                                                   #
#                                                                   #
# This part of the file has been taken from:                        #
#                                                                   #
#    http://autoconf-archive.cryp.to/acx_pthread.html               #
#                                                                   #
#####################################################################

# License
# Copyright © 2007 Steven G. Johnson <stevenj@alum.mit.edu>
#
# This program is free software: you can redistribute it and/or 
# modify it under the terms of the GNU General Public License as 
# published by the Free Software Foundation, either version 3 of 
# the License, or (at your option) any later version. 
# 
# This program is distributed in the hope that it will be useful, 
# but WITHOUT ANY WARRANTY; without even the implied warranty of 
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License 
# along with this program. If not, see <http://www.gnu.org/licenses/>.
# 
# As a special exception, the respective Autoconf Macro's copyright 
# owner gives unlimited permission to copy, distribute and modify 
# the configure scripts that are the output of Autoconf when 
# processing the Macro. You need not follow the terms of the GNU 
# General Public License when using or distributing such scripts, 
# even though portions of the text of the Macro appear in them. 
# The GNU General Public License (GPL) does govern all other use of 
# the material that constitutes the Autoconf Macro.
# 
# This special exception to the GPL applies to versions of the 
# Autoconf Macro released by the Autoconf Macro Archive. When you 
# make and distribute a modified version of the Autoconf Macro, 
# you may extend this special exception to the GPL to apply to 
# your modified version as well.

AC_DEFUN([ACX_PTHREAD], [
AC_REQUIRE([AC_CANONICAL_HOST])
AC_LANG_SAVE
AC_LANG_C
acx_pthread_ok=no

# We used to check for pthread.h first, but this fails if pthread.h
# requires special compiler flags (e.g. on True64 or Sequent).
# It gets checked for in the link test anyway.

# First of all, check if the user has set any of the PTHREAD_LIBS,
# etcetera environment variables, and if threads linking works using
# them:
if test x"$PTHREAD_LIBS$PTHREAD_CFLAGS" != x; then
        save_CFLAGS="$CFLAGS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
        save_LIBS="$LIBS"
        LIBS="$PTHREAD_LIBS $LIBS"
        AC_MSG_CHECKING([for pthread_join in LIBS=$PTHREAD_LIBS with CFLAGS=$PTHREAD_CFLAGS])
        AC_TRY_LINK_FUNC(pthread_join, acx_pthread_ok=yes)
        AC_MSG_RESULT($acx_pthread_ok)
        if test x"$acx_pthread_ok" = xno; then
                PTHREAD_LIBS=""
                PTHREAD_CFLAGS=""
        fi
        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"
fi

# We must check for the threads library under a number of different
# names; the ordering is very important because some systems
# (e.g. DEC) have both -lpthread and -lpthreads, where one of the
# libraries is broken (non-POSIX).

# Create a list of thread flags to try.  Items starting with a "-" are
# C compiler flags, and other items are library names, except for "none"
# which indicates that we try without any flags at all, and "pthread-config"
# which is a program returning the flags for the Pth emulation library.

acx_pthread_flags="pthreads none -Kthread -kthread lthread -pthread -pthreads -mthreads pthread --thread-safe -mt pthread-config"

# The ordering *is* (sometimes) important.  Some notes on the
# individual items follow:

# pthreads: AIX (must check this before -lpthread)
# none: in case threads are in libc; should be tried before -Kthread and
#       other compiler flags to prevent continual compiler warnings
# -Kthread: Sequent (threads in libc, but -Kthread needed for pthread.h)
# -kthread: FreeBSD kernel threads (preferred to -pthread since SMP-able)
# lthread: LinuxThreads port on FreeBSD (also preferred to -pthread)
# -pthread: Linux/gcc (kernel threads), BSD/gcc (userland threads)
# -pthreads: Solaris/gcc
# -mthreads: Mingw32/gcc, Lynx/gcc
# -mt: Sun Workshop C (may only link SunOS threads [-lthread], but it
#      doesn't hurt to check since this sometimes defines pthreads too;
#      also defines -D_REENTRANT)
#      ... -mt is also the pthreads flag for HP/aCC
# pthread: Linux, etcetera
# --thread-safe: KAI C++
# pthread-config: use pthread-config program (for GNU Pth library)

case "${host_cpu}-${host_os}" in
        *solaris*)

        # On Solaris (at least, for some versions), libc contains stubbed
        # (non-functional) versions of the pthreads routines, so link-based
        # tests will erroneously succeed.  (We need to link with -pthreads/-mt/
        # -lpthread.)  (The stubs are missing pthread_cleanup_push, or rather
        # a function called by this macro, so we could check for that, but
        # who knows whether they'll stub that too in a future libc.)  So,
        # we'll just look for -pthreads and -lpthread first:

        acx_pthread_flags="-pthreads pthread -mt -pthread $acx_pthread_flags"
        ;;
esac

if test x"$acx_pthread_ok" = xno; then
for flag in $acx_pthread_flags; do

        case $flag in
                none)
                AC_MSG_CHECKING([whether pthreads work without any flags])
                ;;

                -*)
                AC_MSG_CHECKING([whether pthreads work with $flag])
                PTHREAD_CFLAGS="$flag"
                ;;

                pthread-config)
                AC_CHECK_PROG(acx_pthread_config, pthread-config, yes, no)
                if test x"$acx_pthread_config" = xno; then continue; fi
                PTHREAD_CFLAGS="`pthread-config --cflags`"
                PTHREAD_LIBS="`pthread-config --ldflags` `pthread-config --libs`"
                ;;

                *)
                AC_MSG_CHECKING([for the pthreads library -l$flag])
                PTHREAD_LIBS="-l$flag"
                ;;
        esac

        save_LIBS="$LIBS"
        save_CFLAGS="$CFLAGS"
        LIBS="$PTHREAD_LIBS $LIBS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"

        # Check for various functions.  We must include pthread.h,
        # since some functions may be macros.  (On the Sequent, we
        # need a special flag -Kthread to make this header compile.)
        # We check for pthread_join because it is in -lpthread on IRIX
        # while pthread_create is in libc.  We check for pthread_attr_init
        # due to DEC craziness with -lpthreads.  We check for
        # pthread_cleanup_push because it is one of the few pthread
        # functions on Solaris that doesn't have a non-functional libc stub.
        # We try pthread_create on general principles.
        AC_TRY_LINK([#include <pthread.h>],
                    [pthread_t th; pthread_join(th, 0);
                     pthread_attr_init(0); pthread_cleanup_push(0, 0);
                     pthread_create(0,0,0,0); pthread_cleanup_pop(0); ],
                    [acx_pthread_ok=yes])

        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"

        AC_MSG_RESULT($acx_pthread_ok)
        if test "x$acx_pthread_ok" = xyes; then
                break;
        fi

        PTHREAD_LIBS=""
        PTHREAD_CFLAGS=""
done
fi

# Various other checks:
if test "x$acx_pthread_ok" = xyes; then
        save_LIBS="$LIBS"
        LIBS="$PTHREAD_LIBS $LIBS"
        save_CFLAGS="$CFLAGS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"

        # Detect AIX lossage: JOINABLE attribute is called UNDETACHED.
        AC_MSG_CHECKING([for joinable pthread attribute])
        attr_name=unknown
        for attr in PTHREAD_CREATE_JOINABLE PTHREAD_CREATE_UNDETACHED; do
            AC_TRY_LINK([#include <pthread.h>], [int attr=$attr; return attr;],
                        [attr_name=$attr; break])
        done
        AC_MSG_RESULT($attr_name)
        if test "$attr_name" != PTHREAD_CREATE_JOINABLE; then
            AC_DEFINE_UNQUOTED(PTHREAD_CREATE_JOINABLE, $attr_name,
                               [Define to necessary symbol if this constant
                                uses a non-standard name on your system.])
        fi

        AC_MSG_CHECKING([if more special flags are required for pthreads])
        flag=no
        case "${host_cpu}-${host_os}" in
            *-aix* | *-freebsd* | *-darwin*) flag="-D_THREAD_SAFE";;
            *solaris* | *-osf* | *-hpux*) flag="-D_REENTRANT";;
        esac
        AC_MSG_RESULT(${flag})
        if test "x$flag" != xno; then
            PTHREAD_CFLAGS="$flag $PTHREAD_CFLAGS"
        fi

        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"

        # More AIX lossage: must compile with xlc_r or cc_r
        if test x"$GCC" != xyes; then
          AC_CHECK_PROGS(PTHREAD_CC, xlc_r cc_r, ${CC})
        else
          PTHREAD_CC=$CC
        fi
else
        PTHREAD_CC="$CC"
fi

AC_SUBST(PTHREAD_LIBS)
AC_SUBST(PTHREAD_CFLAGS)
AC_SUBST(PTHREAD_CC)

# Finally, execute ACTION-IF-FOUND/ACTION-IF-NOT-FOUND:
if test x"$acx_pthread_ok" = xyes; then
        ifelse([$1],,AC_DEFINE(HAVE_PTHREAD,1,[Define if you have POSIX threads libraries and header files.]),[$1])
        :
else
        acx_pthread_ok=no
        $2
fi
AC_LANG_RESTORE
])dnl ACX_PTHREAD

#####################################################################
#                                                                   #
#                  END of section ACX_PTHREAD                       #
#                                                                   #
#####################################################################

