# Find FFTW3 includes and libraries
#
# This module defines
# FFTW3_INCLUDE_DIRS, where to find fftw3.h, etc.
# FFTW3_LIBRARIES, the libraries to link against to use FFTW3.
# FFTW3_FOUND, If false, do not try to use FFTW3.

find_package (PkgConfig)
pkg_check_modules (PC_FFTW3 "fftw3" QUIET)

find_path (FFTW3_INCLUDE_DIRS
  NAMES fftw3.h
  HINTS ${PC_FFTW3_INCLUDEDIR}
  ${PC_FFTW3_INCLUDE_DIRS}
  /usr/local/include
  /usr/include
  )

find_library (FFTW3_LIBRARIES
  NAMES fftw3
  HINTS ${PC_FFTW3_LIBDIR}
  ${PC_FFTW3_LIBRARY_DIRS}
  ${FFTW3_INCLUDE_DIRS}/../lib
  /usr/local/lib
  /usr/lib)

if (FFTW3_INCLUDE_DIRS)
  if (FFTW3_LIBRARIES)
    set (FFTW3_FOUND "YES")
    set (FFTW3_LIBRARIES ${FFTW3_LIBRARIES})
  endif ()
endif ()
  
include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (FFTW3 DEFAULT_MSG FFTW3_LIBRARIES FFTW3_INCLUDE_DIRS)

