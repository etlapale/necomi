cmake_minimum_required (VERSION 3.0)
project (Cuiloa VERSION 0.2.0)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")

include_directories ("${PROJECT_SOURCE_DIR}")

find_package (HDF5 COMPONENTS "C;CXX")
if (HDF5_FOUND)
  add_definitions ("-DHAVE_HDF5")
endif ()

# Unit tests
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
find_package (CppUnit)
if (CPPUNIT_FOUND)
  # Build the unit tests
  include_directories (${CPPUNIT_INCLUDE_DIRS})
  add_executable (cuiloa-cppunit-test EXCLUDE_FROM_ALL tests/main.cc)
  target_link_libraries (cuiloa-cppunit-test "${CPPUNIT_LIBRARIES} ${HDF5_LIBRARIES}")
  # Add a `check` target
  add_custom_target (check)
  add_dependencies (check cuiloa-cppunit-test)
  add_custom_command (TARGET check POST_BUILD COMMAND cuiloa-cppunit-test)
endif ()

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
install (DIRECTORY cuiloa DESTINATION include)
install (FILES cuiloa.pc DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig")
