#  FindCuiloa.cmake — CMake find module for cuiloa
#  Defines: CUILOA_FOUND, CUILOA_INCLUDE_DIRS, CUILOA_DEFINITIONS

# We first try search through pkg-config
find_package (PkgConfig)
pkg_check_modules (PC_CUILOA QUIET cuiloa)
set (CUILOA_DEFINITIONS ${PC_CUILOA_CFLAGS_OTHER})

# Otherwise search the source headers
find_path (CUILOA_INCLUDE_DIR cuiloa/cuiloa.h
           HINTS ${PC_CUILOA_INCLUDEDIR} ${PC_CUILOA_INCLUDE_DIRS}
           PATH_SUFFIXES cuiloa)

set (CUILOA_INCLUDE_DIRS ${CUILOA_INCLUDE_DIR})

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (Cuiloa DEFAULT_MSG CUILOA_INCLUDE_DIR)

mark_as_advanced (CUILOA_INCLUDE_DIR)
