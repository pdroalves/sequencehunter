#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
AM_INIT_AUTOMAKE(shunter, 1.1)
AC_CONFIG_SRCDIR([shunter-cmd.c])
AM_CONFIG_HEADER(config.h)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_CPP
AC_OPENMP
AC_PROG_INSTALL

PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.28])
PKG_CHECK_MODULES([GIO], [gio-2.0 >= 2.28])
PKG_CHECK_MODULES([SQLITE3], [sqlite3])

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lcuda':
#AC_CHECK_LIB([cuda], [main])
# FIXME: Replace `main' with a function in `-lcudart':
#AC_CHECK_LIB([cudart], [main])
# FIXME: Replace `main' with a function in `-lsqlite3':
#AC_CHECK_LIB([sqlite3], [main])

# Checks for header files.
AC_CHECK_HEADERS([mach/mach.h stdlib.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

AC_CONFIG_FILES([Makefile])

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([clock_gettime gethrtime gettimeofday])

# Setup CUDA paths
# ------------------------------------------------------------------------------
AC_ARG_WITH([cuda],
   [  --with-cuda=PATH    prefix where cuda is installed [default=/usr/local/cuda]])

ARCH64="x86_64"
ARCH32="i686"
ARCH=`uname -m`

if test -n "$with_cuda"
then
   CUDA_CFLAGS="-I$with_cuda/include"
   CUDA_LIBS="-lcudart"
   if test "$ARCH" == "$ARCH64" 
   then      
      CUDA_LDFLAGS="-L$with_cuda/lib64$SUFFIX"
   elif test "$ARCH" == "$ARCH32"
   then
      CUDA_LDFLAGS="-L$with_cuda/lib$SUFFIX"
   fi
   NVCC="$with_cuda/bin/nvcc"
else
   CUDA_CFLAGS="-I/usr/local/cuda/include"
   CUDA_LIBS="-lcudart"
   if test "$ARCH"=="$ARCH64"
   then      
       CUDA_LDFLAGS="-L/usr/local/cuda/lib64$SUFFIX"
   elif test "$ARCH"=="$ARCH32" 
   then
      CUDA_LDFLAGS="-L/usr/local/cuda/lib$SUFFIX"
   fi
   NVCC="nvcc"
fi
AC_SUBST(CUDA_CFLAGS)
AC_SUBST(CUDA_LIBS)
AC_SUBST(CUDA_LDFLAGS)
AC_SUBST(NVCC)

AC_OUTPUT
