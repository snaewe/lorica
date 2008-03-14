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

# serial 1

dnl AX_LORICA_CHECK_PLATFORM()
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
AC_DEFUN([AX_LORICA_CHECK_PLATFORM],
[ LORICA_DIST_RELEASE="UNKNOWN"
  AM_CONDITIONAL(LORICA_DARWIN,         false)
  AM_CONDITIONAL(LORICA_DARWIN_TIGER,   false)
  AM_CONDITIONAL(LORICA_DARWIN_LEOPARD, false)
  AM_CONDITIONAL(LORICA_LINUX,          false)
  AM_CONDITIONAL(LORICA_FEDORA,         false)
  AM_CONDITIONAL(LORICA_RHEL,           false)
  AM_CONDITIONAL(LORICA_SUSE,           false)
  AM_CONDITIONAL(LORICA_GENTOO,         false)
  AM_CONDITIONAL(LORICA_DEBIAN,         false)
  AC_ARG_ENABLE(lorica-target,
		[AS_HELP_STRING([--enable-lorica-target[[[[=rhel/fedora/opensuse102/opensuse103/gentoo/ubuntu/darwin]]]]], [Distribution target [default=UNKNOWN]])],
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
	xrhel)
		lorica_target="RHEL"
		;;
	xfedora)
		lorica_target="Fedora"
		;;
	xopensuse102)
		lorica_target="OpenSUSE_10_2"
		;;
	xopensuse103)
		lorica_target="OpenSUSE_10_3"
		;;
	xgentoo)
		lorica_target="Gentoo"
		;;
	xubuntu)
		lorica_target="Ubuntu"
		;;
	xdarwin)
		lorica_target="Darwin"
		;;
	*)
		lorica_target="UNKNOWN"
		if test -f /etc/gentoo-release; then
			lorica_target=Gentoo
		fi
		if test -f /etc/redhat-release; then
			is_fedora="`grep Fedora /etc/redhat-release >/dev/null ; echo $?`"
			if test "x$is_fedora" = "x0"; then
			   	lorica_target="Fedora"
			fi
			is_rhel="`grep 'Red Hat Enterprise Linux' /etc/redhat-release >/dev/null ; echo $?`"
			if test "x$is_rhel" = "x0"; then
			   	lorica_target="RHEL"
			fi
		else
			AC_CHECK_PROG(HAVE_LSB_RELEASE, [lsb_release], yes, no)
			if test "x$HAVE_LSB_RELEASE" = "xyes"; then
				lsb_name=`lsb_release -si`
				lsb_version=`lsb_release -sr`
				case "x$lsb_name" in
					xRedHatEnterprise*)
						lorica_target="RHEL"
						;;
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
        xRHEL)
		AM_CONDITIONAL(LORICA_RHEL, true)
		AC_DEFINE([LORICA_RHEL], [1], [Define if this is a Red Hat Enterprise Linux based distribution])
		if test -f /etc/redhat-release; then
			rhel_release="`grep Nahant /etc/redhat-release >/dev/null ; echo $?`"
			if test "x$rhel_release" = "x0"; then
				LORICA_DIST_RELEASE="Red%20Hat%20Enterprise%20Linux%204"
			fi
			if test "x$LORICA_DIST_RELEASE" = "x"; then
				AC_MSG_ERROR([[Unable to determine Red Hat Enterprise Linux release number]])
			fi
		fi
		;;
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
			AC_MSG_ERROR([[Unable to determine Darwin release]])
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
				AC_MSG_ERROR([[Unable to determine Fedora release number]])
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
		codename=`lsb_release -sc`
		case "$codename" in
		     "edgy")
			LORICA_DIST_RELEASE="Ubuntu%20Edgy"
			;;
	    	     "feisty")
			LORICA_DIST_RELEASE="Ubuntu%20Feisty"
			;;
		     "gutsy")
			LORICA_DIST_RELEASE="Ubuntu%20Gutsy"
			;;
	    	     *)
			AC_MSG_ERROR([[Unable to determine Ubuntu release]])
			;;
 		esac
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

dnl AX_LORICA_PLATFORM_ADAPT()
dnl Tries to adapt the build variables to the platform at hand.
dnl
AC_DEFUN([AX_LORICA_PLATFORM_ADAPT],
[ if test "x$LORICA_DIST_RELEASE" = "xUNKNOWN"; then
     AX_LORICA_CHECK_PLATFORM()
  fi
  case "$lorica_target" in
	RHEL)
		RPM_SPEC_DEBUGINFO=''
		RPM_SPEC_CONFIGURE='%configure --enable-lorica-target=rhel --enable-tao-build --enable-lorica-dist=yes --enable-lorica-debug=yes --enable-lorica-devel=yes'
		;;
	Fedora)
		RPM_SPEC_DEBUGINFO=''
		RPM_SPEC_CONFIGURE='%configure --enable-lorica-target=fedora --enable-tao-build --enable-lorica-dist=yes --enable-lorica-debug=yes --enable-lorica-devel=yes'
		;;
	*)
		RPM_SPEC_DEBUGINFO='%debug_package'
		RPM_SPEC_CONFIGURE=''
		;;
  esac
  AC_SUBST([RPM_SPEC_DEBUGINFO])
  AC_SUBST([RPM_SPEC_CONFIGURE])

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
