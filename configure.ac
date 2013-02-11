dnl Init.
AC_INIT(libPlayerPRO,6.0.0)
AC_CONFIG_SRCDIR(README.rtf)

dnl Detect the canonical host and target build environment
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE
AC_CONFIG_MACRO_DIR([m4])

dnl Checks for programs.
AC_PROG_MAKE_SET
AC_PROG_CPP
AC_PROG_CC
AC_PROG_INSTALL
LT_INIT
AC_PROG_LIBTOOL

LIBRARY_CURRENT=6
LIBRARY_REVISION=0
LIBRARY_AGE=0

AC_SUBST(LIBRARY_CURRENT)dnl
AC_SUBST(LIBRARY_REVISION)dnl
AC_SUBST(LIBRARY_AGE)dnl

dnl Darwin doesn't need to use the configure script
if test x$target = xi386-pc-os2-emx ; then
	AC_MSG_ERROR([*** Configuration not required on Mac OS X])
fi

dnl Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T
AC_C_VOLATILE
AC_C_BIGENDIAN

AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_CHECK_FUNCS([memset modf strtol])


AC_OUTPUT([Makefile-other
MADDriver.source/Makefile
MADDriver.source/MADDriver.Source/Makefile])
