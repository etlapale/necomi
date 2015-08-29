cmake_minimum_required (VERSION 3.0)
project (Necomitl)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-braces -Wno-unused-value")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g")

include_directories ("${PROJECT_SOURCE_DIR}")

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Downloaded external dependencies
set (ext_deps
     "share/bitmaps/baboon.png http://necomi.atelo.org/data/share/bitmaps/baboon.png"
     "share/bitmaps/baboon-rot90.png http://necomi.atelo.org/data/share/bitmaps/baboon-rot90.png")
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
file (GLOB CATCH_SRC tests/test-*.cc)
add_executable (necomi-catch-test EXCLUDE_FROM_ALL tests/catch-tests.cc ${CATCH_SRC})
add_custom_target (check)
add_dependencies (check necomi-catch-test)
add_custom_command (TARGET check POST_BUILD COMMAND necomi-catch-test)

# Optional libraries:
# - HDF5
find_package (HDF5 COMPONENTS "C;CXX")
if (HDF5_FOUND)
  add_definitions ("-DHAVE_HDF5")
  add_definitions (${HDF5_DEFINITIONS})
  include_directories ("${HDF5_INCLUDE_DIRS}")
  target_link_libraries (necomi-catch-test ${HDF5_LIBRARIES})
endif ()
# - libpng
find_package (PNG)
if (PNG_FOUND)
  add_definitions ("-DHAVE_LIBPNG ${PNG_DEFINITIONS}")
  include_directories ("${PNG_INCLUDE_DIRS}")
  target_link_libraries (necomi-catch-test ${PNG_LIBRARIES})
endif ()
# - Boost
find_package (Boost)
if (Boost_FOUND)
  add_definitions ("-DHAVE_BOOST")
  include_directories ("${Boost_INCLUDE_DIR}")
  target_link_libraries (necomi-catch-test ${BOOST_MATH_LIBRARY})
else ()
  message (WARNING "disabling checks for the missing Boost library")
endif ()
# - FFTW
find_package (FFTW3)
if (FFTW3_FOUND)
  add_definitions ("-DHAVE_FFTW")
  include_directories ("${FFTW3_INCLUDE_DIRS}")
  target_link_libraries (necomi-catch-test ${FFTW3_LIBRARIES})
else ()
  message (WARNING "disabling check for the missing FFTW library")
endif ()

# pkg-config support
configure_file (necomi.pc.in necomi.pc @ONLY)

# Installlation
install (DIRECTORY necomi DESTINATION include
         FILES_MATCHING PATTERN "*.h")
install (FILES necomi.pc DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig")
