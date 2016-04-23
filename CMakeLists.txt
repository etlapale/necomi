cmake_minimum_required (VERSION 2.8)
project (Necomitl)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-braces -Wno-unused-value")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
if (CMAKE_COMPILER_IS_GNUCXX)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
endif()

include_directories ("${PROJECT_SOURCE_DIR}")
include(CodeCoverage)

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

# Optional libraries:
# - HDF5
find_package (HDF5 COMPONENTS "C;CXX")
if (HDF5_FOUND)
  add_definitions ("-DHAVE_HDF5")
  add_definitions (${HDF5_DEFINITIONS})
  include_directories ("${HDF5_INCLUDE_DIRS}")
  list (APPEND necomi_libraries ${HDF5_LIBRARIES})
else ()
  message (WARNING "disabling checks for the missing HDF5 library")
endif ()
# - libpng
find_package (PNG)
if (PNG_FOUND)
  add_definitions ("-DHAVE_LIBPNG ${PNG_DEFINITIONS}")
  include_directories ("${PNG_INCLUDE_DIRS}")
  list (APPEND necomi_libraries ${PNG_LIBRARIES})
else ()
  message (WARNING "disabling checks for the missing libpng library")
endif ()
# - Boost
find_package (Boost)
if (Boost_FOUND)
  add_definitions ("-DHAVE_BOOST")
  include_directories ("${Boost_INCLUDE_DIR}")
  list (APPEND necomi_libraries ${BOOST_MATH_LIBRARY})
else ()
  message (WARNING "disabling checks for the missing Boost library")
endif ()
# - FFTW
find_package (FFTW3)
if (FFTW3_FOUND)
  add_definitions ("-DHAVE_FFTW")
  include_directories ("${FFTW3_INCLUDE_DIRS}")
  list (APPEND necomi_libraries ${FFTW3_LIBRARIES})
else ()
  message (WARNING "disabling check for the missing FFTW library")
endif ()

# Unit tests
set (tests_src
  tests/catch-tests.cc
  tests/test-algos-modif.cc
  tests/test-algos-sort.cc
  tests/test-arrays.cc
  tests/test-broadcasting.cc
  tests/test-concepts.cc
  tests/test-convert-stl.cc
  tests/test-core-iterators.cc
  tests/test-delayed-arithmetic.cc
  tests/test-delayed.cc
  tests/test-delayed-maps.cc
  tests/test-delayed-transforms.cc
  tests/test-filters-exponential.cc
  tests/test-numerics.cc
  tests/test-random.cc
  tests/test-slices.cc
  tests/test-traits-shape.cc
  tests/test-vararray.cc)
if (HDF5_FOUND)
  set (tests_src ${tests_src} tests/test-hdf5.cc)
endif ()
if (PNG_FOUND)
  set (tests_src ${tests_src} tests/test-png.cc)
endif ()
if (FFTW3_FOUND)
  set (tests_src ${tests_src} tests/test-numerics-fft.cc)
endif ()
add_executable (necomi-catch-test EXCLUDE_FROM_ALL ${tests_src})
target_link_libraries (necomi-catch-test ${necomi_libraries})

# Code coverage (gcc)
if (CMAKE_COMPILER_IS_GNUCXX)
  target_link_libraries (necomi-catch-test ${necomi_libraries} "gcov")
endif()


add_custom_target (check)
add_dependencies (check necomi-catch-test)
add_custom_command (TARGET check POST_BUILD COMMAND necomi-catch-test)

# pkg-config support
configure_file (necomi.pc.in necomi.pc @ONLY)

# Installlation
install (DIRECTORY necomi DESTINATION include
         FILES_MATCHING PATTERN "*.h")
install (FILES necomi.pc DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig")
