#
#  Top-level configure.in file for Lorica.
#  Copyright (C) 2008 OMC Denmark ApS
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

AC_PREREQ(2.59)
AC_INIT([lorica],[1.0.0],[bugs@42tools.com])

AC_COPYRIGHT([Copyright (C) 2008 OMC Denmark ApS])

AC_CONFIG_SRCDIR([lorica.spec.in])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)

dnl Required by autoconf 2.60
AC_SUBST(datarootdir)

AC_GNU_SOURCE

dnl Standard checks
AC_LANG([C++])
AC_PROG_CXX
AC_REQUIRE_CPP

AC_HEADER_STDC
AC_HEADER_MAJOR

AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
#AC_PROG_MKDIR_P #- RHEL4 doesn't seem to support this macro

AC_TYPE_PID_T
AC_TYPE_SIZE_T
#AC_TYPE_SSIZE_T #- RHEL4 doesn't seem to support this macro

AC_FUNC_MALLOC
AC_CHECK_FUNCS([dup2 ftruncate memset mkdir strcasecmp strdup strtol])

dnl Initialize libtool
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL

dnl
dnl ACE and TAO versions
dnl
AC_ARG_WITH(acetao-major, AS_HELP_STRING([--with-acetao-major], [ACE+TAO major version number]), 
			  [],
			  [withval=none])
if test "x$withval" = "xnone"; then
   	LIBACE_CURRENT=5
   	LIBTAO_CURRENT=1
else
   	let LIBACE_CURRENT=$withval+4
   	LIBTAO_CURRENT=$withval
fi
AC_SUBST(LIBACE_CURRENT)
AC_SUBST(LIBTAO_CURRENT)

AC_ARG_WITH(acetao-minor, AS_HELP_STRING([--with-acetao-minor], [ACE+TAO minor version number]), 
			  [LIBACETAO_REVISION=$withval],
          		  [LIBACETAO_REVISION=6])
AC_SUBST(LIBACETAO_REVISION)

AC_ARG_WITH(acetao-age, AS_HELP_STRING([--with-acetao-age], [ACE+TAO least significant version number]), 
 			[LIBACETAO_AGE=$withval],
 			[LIBACETAO_AGE=3])
AC_SUBST(LIBACETAO_AGE)

ACETAO="http://download.dre.vanderbilt.edu/previous_versions/ACE+TAO-$LIBACE_CURRENT.$LIBACETAO_REVISION.$LIBACETAO_AGE.tar.bz2"
AC_SUBST(ACETAO)

dnl
dnl Lorica versions and names
dnl
LORICA_NAME=AC_PACKAGE_NAME
AC_SUBST(LORICA_NAME)

LORICA_VERSION=AC_PACKAGE_VERSION
AC_SUBST(LORICA_VERSION)

dnl Lorica IANA registered ports
AC_DEFINE([LORICA_DEFAULT_INSIDE_FACING_PORT], [4080], [Default Lorica inside facing port])
AC_DEFINE([LORICA_DEFAULT_INSIDE_FACING_PORT_STR], ["4080"], [Default Lorica inside facing port (stringified)])

AC_DEFINE([LORICA_DEFAULT_INSIDE_FACING_PORT_SEC], [4081], [Default Lorica inside facing secure port])
AC_DEFINE([LORICA_DEFAULT_INSIDE_FACING_PORT_SEC_STR], ["4081"], [Default Lorica inside facing secure port (stringified)])

AC_DEFINE([LORICA_DEFAULT_OUTSIDE_FACING_PORT], [4082], [Default Lorica outside facing port])
AC_DEFINE([LORICA_DEFAULT_OUTSIDE_FACING_PORT_STR], ["4082"], [Default Lorica outside facing port (stringified)])

AC_DEFINE([LORICA_DEFAULT_OUTSIDE_FACING_PORT_SEC], [4083], [Default Lorica outside facing secure port])
AC_DEFINE([LORICA_DEFAULT_OUTSIDE_FACING_PORT_SEC_STR], ["4083"], [Default Lorica outside facing secure port (stringified)])

dnl
dnl Configure options
dnl

dnl Enable build of TAO if need be
AC_ARG_ENABLE(tao-build,
	      [AS_HELP_STRING([--enable-tao-build[[[[=no/yes]]]]], [Enable download and local rebuild of ACE and TAO [default=no]])],
 	      [],						   
 	      enable_tao_build=no)
if test "x$enable_tao_build" = "xyes"; then
	msg_tao_build=yes
else
	msg_tao_build=no
fi

dnl Link binaries statically
AC_ARG_ENABLE(static-lorica,
 	      [AS_HELP_STRING([--enable-static-lorica[[[[=no/yes]]]]], [Enable statically linked binaries if possible [default=no]])],
  	      [],
 	      [enable_static_lorica=no])
if test "x$enable_static_lorica" = "xyes"; then
	AC_DEFINE([LORICA_STATIC], [1], [Define if you want build and link statically with ACE and TAO])
	msg_static=yes
else
	msg_static=no
fi
AM_CONDITIONAL(LORICA_STATIC, test x$enable_static_lorica = xyes)

dnl Install development headers
AC_ARG_ENABLE(lorica-devel,
	      [AS_HELP_STRING([--enable-lorica-devel[[[[=no/yes]]]]], [Install development files [default=no]])],
	      [],
	      enable_lorica_devel=no)
if test "x$enable_lorica_devel" = "xyes"; then
	AC_DEFINE([LORICA_DEVEL], [1], [Define if you want to include header and IDL files when installing])
	msg_lorica_devel=yes
else
	msg_lorica_devel=no
fi
AM_CONDITIONAL(LORICA_DEVEL, test x$enable_lorica_devel = xyes)

dnl Do various things that helps debugging problems
AC_ARG_ENABLE(lorica-debug,
	      [AS_HELP_STRING([--enable-lorica-debug[[[[=no/yes]]]]], [Enable Lorica debugging [default=no]])],
	      [],
	      enable_lorica_debug=no)
if test "x$enable_lorica_debug" = "xyes"; then
	AC_DEFINE([LORICA_DEBUG], [1], [Define if you want to make a debug enabled build])
	msg_lorica_debug=yes
else
	msg_lorica_debug=no
fi
AM_CONDITIONAL(LORICA_DEBUG, test x$enable_lorica_debug = xyes)

dnl For the maintainer...
AC_ARG_ENABLE(lorica-dist,
	      [AS_HELP_STRING([--enable-lorica-dist[[[[=no/yes]]]]], [Is this a "make dist-<PACKAGE MANAGER>" attempt? [default=no]])],
	      [],
	      ,enable_lorica_dist=no)

dnl
dnl Program availability checks
dnl
AC_CHECK_PROG(HAVE_WGET, [wget], yes, no)
if test "x$HAVE_WGET" = "xno"; then
   AC_CHECK_PROG(HAVE_CURL, [curl], yes, no)
   if test "x$HAVE_CURL" = "xno"; then
      AC_MSG_ERROR([[You need wget(1) or curl(1) program to build Lorica]])
   else
      AM_CONDITIONAL(LORICA_WGET, false)
      AM_CONDITIONAL(LORICA_CURL, true)
   fi
else
   AM_CONDITIONAL(LORICA_WGET, true)
   AM_CONDITIONAL(LORICA_CURL, false)
fi

AC_CHECK_PROG(HAVE_SED, [sed], yes, no)
if test "x$HAVE_SED" = "xno"; then
   AC_MSG_ERROR([[You need the sed(1) program to build Lorica]])
fi

AC_CHECK_PROG(HAVE_BASENAME, [basename], yes, no)
if test "x$HAVE_BASENAME" = "xno"; then
   AC_MSG_ERROR([[You need the basename(1) program to build Lorica]])
fi

AC_CHECK_PROG(HAVE_DIRNAME, [dirname], yes, no)
if test "x$HAVE_DIRNAME" = "xno"; then
   AC_MSG_ERROR([[You need the dirname(1) program to build Lorica]])
fi

AC_CHECK_PROG(HAVE_FIND, [find], yes, no)
if test "x$HAVE_FIND" = "xno"; then
   AC_MSG_ERROR([[You need the find(1) program to build Lorica]])
fi

AC_CHECK_PROG(HAVE_RPMBUILD, [rpmbuild], yes, no)
if test "x$HAVE_RPMBUILD" = "xno"; then
   AM_CONDITIONAL(LORICA_RPMBUILD, false)
   AC_MSG_WARN([[You will need the rpmbuild(8) program to build Lorica RPMs]])
else
   AM_CONDITIONAL(LORICA_RPMBUILD, true)
fi

AC_CHECK_PROG(HAVE_RPMDEV_SETUPTREE, [rpmdev-setuptree], yes, no)
if test "x$HAVE_RPMDEV_SETUPTREE" = "xno"; then
   AM_CONDITIONAL(LORICA_RPMDEV_SETUPTREE, false)
   AC_MSG_WARN([[You should have the rpmdev-setuptree program if building Lorica RPMs]])
else
   AM_CONDITIONAL(LORICA_RPMDEV_SETUPTREE, true)
fi

AC_CHECK_PROG(HAVE_RPMDEV_WIPETREE, [rpmdev-wipetree], yes, no)
if test "x$HAVE_RPMDEV_WIPETREE" = "xno"; then
   AM_CONDITIONAL(LORICA_RPMDEV_WIPETREE, false)
   AC_MSG_WARN([[You should have the rpmdev-wipetree program if building Lorica RPMs]])
else
   AM_CONDITIONAL(LORICA_RPMDEV_WIPETREE, true)
fi

AC_CHECK_PROG(HAVE_BUILD, [build], yes, no)
if test "x$HAVE_BUILD" = "xno"; then
   AC_MSG_WARN([[You should have the build program if building Lorica RPMs on SUSE]])
fi

AC_CHECK_PROG(HAVE_FAKEROOT, [fakeroot], yes, no)
if test "x$HAVE_FAKEROOT" = "xno"; then
   AC_MSG_WARN([[You will need the fakeroot(1) program to build Lorica DEBs]])
fi

AC_CHECK_PROG(HAVE_DPKG_BUILDPACKAGE, [dpkg-buildpackage], yes, no)
if test "x$HAVE_DPKG_BUILDPACKAGE" = "xno"; then
   AM_CONDITIONAL(LORICA_DPKG_BUILDPACKAGE, false)
   AC_MSG_WARN([[You will need the dpkg-buildpackage(8) program to build Lorica DEBs]])
else
   AM_CONDITIONAL(LORICA_DPKG_BUILDPACKAGE, true)
fi

dnl Subversion revision number
AC_CHECK_PROG(HAVE_AWK, [awk], yes, no)
if test "x$HAVE_AWK" = "xno"; then
   AM_CONDITIONAL(LORICA_SVNREV, false)
   AC_MSG_WARN([[You will need the awk(1) program to use the dist-svnrev make target]])
else
   AM_CONDITIONAL(LORICA_SVNREV, true)
   SVN_REV=`svn info | grep Revision | awk '{ print $2 }'`
   AC_SUBST(SVN_REV)
fi


dnl
dnl Which Linux distribution or UNIX platform?
dnl
AX_LORICA_CHECK_PLATFORM()

dnl
dnl Adapt to different Linux distributions and UNIX platforms
dnl
AX_LORICA_PLATFORM_ADAPT()

dnl
dnl Adapt to the ACE+TAO configuration
dnl
AX_LORICA_ACETAO_ADAPT()

dnl
dnl CPPFLAGS, CXXFLAGS, LIBS and stuff
dnl

dnl Lorica install directories
LORICA_idl_install_subdir="idl/lorica"
AC_SUBST(LORICA_idl_install_subdir)

dnl Lorica top-level source directory
LORICA_top_dir="`pwd`"
AC_SUBST(LORICA_top_dir)

dnl Check if the compiler supports -iquote DIR (from ltrace)
ac_cv_have_iquote=no
AC_MSG_CHECKING(whether compiler accepts -iquote dir)
save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS -iquote ."
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]], [[]])],[ac_cv_have_iquote=yes],[])
CPPFLAGS="$save_CPPFLAGS"
AC_MSG_RESULT($ac_cv_have_iquote)
if test $ac_cv_have_iquote = yes; then
  iquote="-iquote "
else
  iquote="-I"
fi
AC_SUBST(iquote)

dnl Should only be searchable in "" include statements
LORICA_INCLUDES="$iquote$LORICA_top_dir $iquote$LORICA_top_dir/src $iquote$LORICA_top_dir/defines"

dnl From AM_LORICA_ACETAO_ADAPT
LORICA_INCLUDES="$LORICA_INCLUDES $ACETAO_CPPFLAGS"

dnl Now set the preprocessor arguments
LORICA_CPPFLAGS="$LORICA_INCLUDES"
if test "x$enable_static" = "xyes"; then
   LORICA_CPPFLAGS="$LORICA_CPPFLAGS -DACE_AS_STATIC_LIBS"
fi
AC_SUBST(LORICA_CPPFLAGS)

dnl CXXFLAGS
LORICA_CXXFLAGS=""

dnl Workaround for https://bugzilla.redhat.com/show_bug.cgi?id=218377
CXX_VERSION=`$CXX -dumpversion`
if test "x$CXX_VERSION" = "x3.4.6"; then
   LORICA_FIX_GXX_BUG_FLAG="-fno-var-tracking"
else
   LORICA_FIX_GXX_BUG_FLAG=""
fi
AC_SUBST(LORICA_FIX_GXX_BUG_FLAG)
LORICA_CXXFLAGS="$LORICA_CXXFLAGS $LORICA_FIX_GXX_BUG_FLAG"

if test "x$lorica_target" = "xUbuntu"; then
   cs_prog_cxx_visibility_inlines_hidden=""
else
   CS_VISIBILITY_FLAG_INLINES_HIDDEN()
fi
CS_COMPILER_WARNINGS([C++])
CS_COMPILER_ERRORS([C++])
LORICA_CXXFLAGS="$LORICA_CXXFLAGS $cs_cv_prog_cxx_enable_errors $cs_cv_prog_cxx_enable_warnings $cs_prog_cxx_visibility_inlines_hidden"

AX_CXXFLAGS_GCC_OPTION(-fvisibility=hidden, LORICA_CXXFLAGS)
AX_CXXFLAGS_GCC_OPTION(-Wextra, LORICA_CXXFLAGS)
AX_CXXFLAGS_GCC_OPTION(-Wundef, LORICA_CXXFLAGS)
AX_CXXFLAGS_GCC_OPTION(-Wpointer-arith, LORICA_CXXFLAGS)
AX_CXXFLAGS_GCC_OPTION(-pipe, LORICA_CXXFLAGS)

if test "x$lorica_target" = "xDarwin"; then
   CS_COMPILER_IGNORE_LONG_DOUBLE([C++])
   LORICA_CXXFLAGS="$LORICA_CXXFLAGS $cs_cv_prog_cxx_ignore_long_double"
fi
if test "x$enable_lorica_dist" = "xyes"; then
	msg_lorica_dist=yes
else
	msg_lorica_dist=no
	if test "x$enable_lorica_debug" = "xyes"; then
	   	AX_CXXFLAGS_GCC_OPTION(-Werror-implicit-function-declaration, LORICA_CXXFLAGS)
		AX_CXXFLAGS_GCC_OPTION(-Wbad-function-cast, LORICA_CXXFLAGS)
		AX_CXXFLAGS_GCC_OPTION(-Wcast-align, LORICA_CXXFLAGS)
		AX_CXXFLAGS_GCC_OPTION(-Wmissing-declarations, LORICA_CXXFLAGS)
		AX_CXXFLAGS_GCC_OPTION(-ggdb , LORICA_CXXFLAGS)
		AX_CXXFLAGS_GCC_OPTION(-O1, LORICA_CXXFLAGS)
	fi
fi
if test "$lorica_target" = "OpenSUSE_10_2"; then
	LORICA_CXXFLAGS="$LORICA_CXXFLAGS $RPM_OPT_FLAGS"
fi
if test "$lorica_target" = "OpenSUSE_10_3"; then
	LORICA_CXXFLAGS="$LORICA_CXXFLAGS $RPM_OPT_FLAGS"
fi
AC_SUBST(LORICA_CXXFLAGS)

dnl LDFLAGS
LDFLAGS="$ACETAO_LDFLAGS"
if test "x$lorica_target" = "xDarwin"; then
	LDFLAGS="-flat_namespace $LDFLAGS"
fi
if test "x$enable_lorica_debug" = "xyes"; then
	LDFLAGS="-rdynamic $LDFLAGS"
fi
LDFLAGS="-pthread $LDFLAGS"
AC_SUBST(LDFLAGS)

dnl
dnl Environment variables to work around ':=' not being POSIX compliant
dnl
ENV_PATH=$PATH
AC_SUBST(ENV_PATH)

ENV_LD_LIBRARY_PATH=$LD_LIBRARY_PATH
AC_SUBST(ENV_LD_LIBRARY_PATH)

ENV_DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH
AC_SUBST(ENV_DYLD_LIBRARY_PATH)

dnl
dnl dh_install paths
dnl

dnl Utility macro to get dh_install .install paths
dnl LORICA_GET_DH_INSTALL_PATH(VAR-PREFIX, PATH_NAME, PATH)
AC_DEFUN([LORICA_GET_DH_INSTALL_PATH], [
	$1_$2=`echo $3 | cut -c 2-`
])

LORICA_GET_DH_INSTALL_PATH(DH_INSTALL, default_etcdir, /etc)
AC_SUBST(DH_INSTALL_default_etcdir)

LORICA_GET_DH_INSTALL_PATH(DH_INSTALL, default_includedir, /usr/include)
AC_SUBST(DH_INSTALL_default_includedir)

LORICA_GET_DH_INSTALL_PATH(DH_INSTALL, default_datadir, /usr/share)
AC_SUBST(DH_INSTALL_default_datadir)

LORICA_GET_DH_INSTALL_PATH(DH_INSTALL, default_bindir, /usr/bin)
AC_SUBST(DH_INSTALL_default_bindir)

LORICA_GET_DH_INSTALL_PATH(DH_INSTALL, default_sbindir, /usr/sbin)
AC_SUBST(DH_INSTALL_default_sbindir)

LORICA_GET_DH_INSTALL_PATH(DH_INSTALL, default_libdir, /usr/lib)
AC_SUBST(DH_INSTALL_default_libdir)

LORICA_GET_DH_INSTALL_PATH(DH_INSTALL, default_docdir, /usr/share/doc/$PACKAGE_NAME)
AC_SUBST(DH_INSTALL_default_docdir)

dnl
dnl Check if this is a distribution
dnl
AM_CONDITIONAL(THIS_IS_NOT_A_DISTRIBUTION, [test -f ./nodist])


dnl ******************************
dnl Makefiles
dnl ******************************

AC_CONFIG_FILES([
Makefile
lorica.spec
acetao/Makefile
deb/watch
deb/version.sed
deb/Makefile
deb/control
deb/copyright
deb/lorica-dev.install
deb/lorica.install
docs/Makefile
ebuild/Makefile
ebuild/lorica.ebuild
src/Makefile
src/OCI/Makefile
src/lorica/Makefile
src/proxy/Makefile
init_rc/Makefile
defines/Makefile
tests/Makefile
tests/exception/Makefile
tests/fileConfig/Makefile
tests/garbageCollector/Makefile
tests/genericEvaluator/Makefile
tests/genericEvaluator/basic/Makefile
tests/genericEvaluator/basic_collocate/Makefile
tests/genericEvaluator/bidir/Makefile
tests/genericEvaluator/callback/Makefile
tests/genericEvaluator/forward/Makefile
tests/genericEvaluator/logging/Makefile
tests/genericEvaluator/persistence_ifr/Makefile
tests/genericEvaluator/server_agent/Makefile
tests/genericEvaluator/unbind/Makefile
tests/genericEvaluator/union/Makefile
tests/mapForClient/Makefile
tests/mapForServer/Makefile
tests/secureBridge/Makefile
tests/secureInvocation/Makefile
])
AC_OUTPUT

case "$lorica_target" in
	UNKNOWN)
		AC_MSG_WARN([[Unknown distribution - Please use --enable-lorica-target=<TARGET> to specify a distribution target for your build]])
		;;
	*)	
		lorica_target="`echo $LORICA_DIST_RELEASE | sed s/%20/' '/g`"
		;;
esac

echo "\

	Target Platform:                                      $lorica_target
	Making a distribution build:                          $msg_lorica_dist
	Making a debug build:                                 $msg_lorica_debug
	Making static executables:                            $msg_static
	Install development header and IDL files:             $msg_lorica_devel
	Download and build ACE and TAO locally if necessary:  $msg_tao_build
"
