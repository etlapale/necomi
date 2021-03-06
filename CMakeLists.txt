cmake_minimum_required (VERSION 3.2)
project (Necomitl)

# Enable code coverage if performing a debug build
if (CMAKE_BUILD_TYPE EQUAL "Debug")
  option(PERFORM_COVERAGE "Perform code coverage analysis" ON)
else ()
  option(PERFORM_COVERAGE "Perform code coverage analysis" OFF)
endif ()

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-braces -Wno-unused-value")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")

if (CMAKE_COMPILER_IS_GNUCXX AND ${PERFORM_COVERAGE})
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
endif ()

include_directories ("${PROJECT_SOURCE_DIR}")

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
if (${PERFORM_COVERAGE})
  include(CodeCoverage)
endif ()

# Check if PNG images were downloaded through Git LFS
file (SHA224 "${PROJECT_SOURCE_DIR}/share/bitmaps/baboon.png" baboon_hash)
if (baboon_hash STREQUAL "449b2b3437f608d18eaee1b2e0115635ae26cf8ad7275ac82e6faced")
  set (got_lfs_files TRUE)
  add_definitions ("-DNECOMI_SOURCE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"")
else ()
  message (WARNING "Large files were not downloaded through git-lfs, some checks will be disabled")
  set (got_lfs_files FALSE)
endif ()

# Optional libraries:
# - HDF5
find_package (HDF5 COMPONENTS "C;CXX")
# Make sure we got C++ support in HDF5
set(all_hdf5_libs ${HDF5_FOUND})
foreach (lib ${HDF5_CXX_LIBRARIES})
  if (NOT lib)
    set (all_hdf5_libs FALSE)
  endif ()
endforeach ()
if (${all_hdf5_libs})
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
  tests/test-codecs-streams.cc
  tests/test-concepts.cc
  tests/test-convert-stl.cc
  tests/test-core-iterators.cc
  tests/test-delayed-arithmetic.cc
  tests/test-delayed-comparisons.cc
  tests/test-delayed.cc
  tests/test-delayed-maps.cc
  tests/test-delayed-transforms.cc
  tests/test-filters-deriche.cc
  tests/test-filters-exponential.cc
  tests/test-numerics.cc
  tests/test-random.cc
  tests/test-slices.cc
  tests/test-traits-shape.cc
  tests/test-vararray.cc
  )
if (HDF5_FOUND)
  #set (tests_src ${tests_src} tests/test-hdf5.cc)
endif ()
if (PNG_FOUND AND got_lfs_files)
  set (tests_src ${tests_src} tests/test-png.cc)
endif ()
if (FFTW3_FOUND)
  #set (tests_src ${tests_src} tests/test-numerics-fft.cc)
endif ()
add_executable (necomi-catch-test EXCLUDE_FROM_ALL ${tests_src})
target_link_libraries (necomi-catch-test ${necomi_libraries})

# Code coverage (gcc)
if (CMAKE_COMPILER_IS_GNUCXX AND ${PERFORM_COVERAGE})
  setup_target_for_coverage(coverage necomi-catch-test coverage)
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
install (FILES "${CMAKE_BINARY_DIR}/necomi.pc"
	 DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig")
