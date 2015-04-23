#  FindNecomi.cmake — CMake find module for necomi
#  Defines: NECOMI_FOUND, NECOMI_INCLUDE_DIRS, NECOMI_DEFINITIONS

# We first try search through pkg-config
find_package (PkgConfig)
pkg_check_modules (PC_NECOMI QUIET necomi)
set (NECOMI_DEFINITIONS ${PC_NECOMI_CFLAGS_OTHER})

# Otherwise search the source headers
find_path (NECOMI_INCLUDE_DIR necomi/necomi.h
           HINTS ${PC_NECOMI_INCLUDEDIR} ${PC_NECOMI_INCLUDE_DIRS}
           PATH_SUFFIXES necomi)

set (NECOMI_INCLUDE_DIRS ${NECOMI_INCLUDE_DIR})

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (Necomi DEFAULT_MSG NECOMI_INCLUDE_DIR)

mark_as_advanced (NECOMI_INCLUDE_DIR)
