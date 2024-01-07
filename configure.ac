#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.69])

m4_define([ver_major], [1])
m4_define([ver_minor], [0])
m4_define([ver_release], [0])
# Set build number to commit count only if ".git" folder is present
m4_define([ver_build], m4_if(m4_esyscmd_s([ls -1 -d ".git" 2>&1]),.git,[m4_esyscmd_s([git rev-list --count HEAD])],[0]))

m4_define([swdisasm_version],
      [ver_major.ver_minor.ver_release.ver_build])

AC_INIT([SW Disassembler], [swdisasm_version], [unavowed@vexillium.org], [swdisasm])

AC_CONFIG_SRCDIR([src/disassembler.cc])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_MACRO_DIRS([m4])

AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE([foreign])
ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_DEFINE([VER_MAJOR], [ver_major], [Major version number])
AC_DEFINE([VER_MINOR], [ver_minor], [Minor version number])
AC_DEFINE([VER_RELEASE], [ver_release], [Release number])
AC_DEFINE([VER_BUILD], [ver_build], [Build number])

# M4 defines __unix__ to be the empty string on UNIX-like systems.  This
# causes AC_DEFINE to fail if it is used to define __unix__.  This seems to
# fix the problem.
ifdef([__unix__], define([__unix__], [[__unix__]]))

# Set the proper prefix
AS_IF([test "$prefix" = "NONE"], [
  prefix="$ac_default_prefix"
])
AS_IF([test "$exec_prefix" = "NONE"], [
  exec_prefix="$prefix"
])

# Checks for programs.
AC_PROG_CXX
PKG_PROG_PKG_CONFIG
AC_CHECK_PROGS(WINDRES, $WINDRES "${target_alias}-windres" "${target}-windres", "windres", :)
AC_PROG_SED # `sed` is used in Makefile for packaging shared libraries on Windows

# Checks for libraries.
AC_MSG_CHECKING([[if we need a __unix__ define]])
AC_COMPILE_IFELSE([AC_LANG_SOURCE([
  #ifndef __unix__
  # if defined __APPLE__ /* add other known platforms here */
  #   error should have __unix__ but don't
  # endif
  #endif
])], [AC_MSG_RESULT([no])],
[
  AC_MSG_RESULT([yes])
  AC_DEFINE([__unix__], 1,
[Define to 1 if running on a UNIX-like system where __unix__ is missing])
])

have_posix=yes
have_windows=no
have_darwin=no

case "$host" in
  *-*-mingw*)
    have_windows=yes
    have_posix=no
    ;;
  *-*-darwin*)
    have_darwin=yes
    ;;
esac

AM_CONDITIONAL([TARGET_WINDOWS], [test "$have_windows" = "yes"])

# On Windows, we can optionaly include resources in the executable.
AM_CONDITIONAL([HAS_WINDRES], [test -n "${WINDRES}" -a "$have_windows" = "yes"])

# If a library is found after checking or searching like below, the
# LIBS variable is updated to include that library while linking.

AC_CHECK_LIB([intl], [libintl_version], [], [
  AC_MSG_FAILURE([library libintl not found])
])

# Required by libbfd
AC_CHECK_LIB([iberty], [lrealpath], [], [
  AC_MSG_FAILURE([library libiberty not found])
])

# Required by libbfd from binutils >= 2.40
AC_SEARCH_LIBS([sframe_encode], [sframe], [], [
  AC_MSG_WARN([unable to find function sframe_encode(), libbfd may require it])
])

# Required by libbfd from binutils >= 2.39
AC_CHECK_LIB([z], [deflate], [], [
  AC_MSG_WARN([library zlib not found, libbfd may require it])
])

# Required by libbfd from binutils >= 2.40
AC_CHECK_LIB([zstd], ZSTD_decompress, [], [
  AC_MSG_WARN([library zstd not found, libbfd may require it])
])
AC_CHECK_HEADERS([zstd.h])


AC_CHECK_LIB([bfd], [bfd_init], [], [
  AC_MSG_FAILURE([library libbfd not found])
])
AC_CHECK_HEADERS([bfd.h])

#print_insn_i386
AC_CHECK_LIB([opcodes], [print_i386_disassembler_options], [], [
  AC_MSG_FAILURE([library libopcodes not found])
])

# Arguments

#AC_ARG_ENABLE([debug],
#	      AS_HELP_STRING([--enable-debug=@<:@no|yes|full@:>@],
#			     [enable debugging options]),
#	      [use_debug=$enableval], [use_debug=no])

# Flags

CFLAGS="$CFLAGS -Wall -Wextra -Wno-unused-parameter"
CFLAGS="$CFLAGS -include \"\$(top_builddir)/src/config.h\""

AC_SUBST([WINDRES])

# Prepare makefiles from `.in` templates
AC_CONFIG_FILES([Makefile src/Makefile])

# Prepare files for Windows build
AC_CONFIG_LINKS([
  res/ledisasm-icon128.ico:res/ledisasm-icon128.ico
])

# Done - generate and execute `config.status`
AC_OUTPUT

echo
