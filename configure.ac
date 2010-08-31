#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([sdk_resolver],[1.0],[didelot.sylvai@gmail.com])
AC_CONFIG_AUX_DIR([build_aux])
AM_INIT_AUTOMAKE([1.9.6 -Wall -Werror dist-bzip2])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
LT_INIT

# Checks for GTK+
AC_ARG_ENABLE([gtk],AS_HELP_STRING([--enable-gtk],[Enable GTK+
               support]),[enable_gtk=$enableval],[disable_gtk="no"])

if test "x$disable_gtk" = "xno"
then
  AM_PATH_GTK_2_0([2.10.0],,AC_MSG_ERROR([Gtk+ 2.10.0 or higher required.]))
  GTK_ENABLE="-DGTK_ENABLE"
fi
AC_SUBST([GTK_ENABLE])

#endif !GTK

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# For Doxygen
DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(ON)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN(sdk_solver, doxygen.cfg)


# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([strerror strstr])

AC_CONFIG_FILES([Makefile
                 src/Makefile])

AC_OUTPUT
