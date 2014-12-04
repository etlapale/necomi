cmake_minimum_required (VERSION 3.0)
project (Cuiloa)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-missing-braces")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g")

include_directories ("${PROJECT_SOURCE_DIR}")

# Optional libraries:
# - HDF5
find_package (HDF5 COMPONENTS "C;CXX")
if (HDF5_FOUND)
  add_definitions ("-DHAVE_HDF5")
endif ()
# - libpng
find_package (PNG)
if (PNG_FOUND)
  add_definitions ("-DHAVE_LIBPNG")
endif ()

# Downloaded external dependencies
set (ext_deps
     "tests/catch.hpp https://raw.githubusercontent.com/philsquared/Catch/master/single_include/catch.hpp"
     "share/bitmaps/baboon.png http://emilien.tlapale.com/data/cuiloa/share/bitmaps/baboon.png")
foreach (dep IN LISTS ext_deps)
  separate_arguments (dep)
  list (GET dep 0 filename)
  list (GET dep 1 url)
  if (NOT EXISTS ${filename})
    message ("-- Downloading ${url}")
    file (DOWNLOAD ${url} ${filename})
  endif ()
endforeach ()

# Unit tests
file (GLOB CATCH_SRC tests/*.cc)
add_executable (cuiloa-catch-test EXCLUDE_FROM_ALL ${CATCH_SRC})
target_link_libraries (cuiloa-catch-test ${HDF5_LIBRARIES} ${PNG_LIBRARIES})
add_custom_target (check)
add_dependencies (check cuiloa-catch-test)
add_custom_command (TARGET check POST_BUILD COMMAND cuiloa-catch-test)

# Documentation
find_package (Doxygen)
if (DOXYGEN_FOUND)
  add_custom_target (doc)
  configure_file (doc/Doxyfile.in doc/Doxyfile @ONLY)
  add_custom_command (TARGET doc PRE_BUILD COMMAND ${DOXYGEN_EXECUTABLE} doc/Doxyfile)
endif ()

# pkg-config support
configure_file (cuiloa.pc.in cuiloa.pc @ONLY)

# Installlation
install (DIRECTORY cuiloa DESTINATION include
         FILES_MATCHING PATTERN "*.h")
install (FILES cuiloa.pc DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig")
