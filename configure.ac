AC_PREREQ(2.59)
AC_INIT([ion-test],[0.0.1])

AC_CONFIG_FILES([Makefile])

AM_INIT_AUTOMAKE([1.9 subdir-objects no-dist-gzip dist-bzip2 -Wno-portability]) # 1.10 is needed for target_LIBTOOLFLAGS

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC_C99
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_MKDIR_P
AC_PROG_LN_S
PKG_PROG_PKG_CONFIG

AC_SUBST([pkgconfigdir], [$libdir/pkgconfig])

AC_OUTPUT
