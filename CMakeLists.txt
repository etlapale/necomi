cmake_minimum_required (VERSION 3.0)
project (Cuiloa)

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
  include_directories (${CPPUNIT_INCLUDE_DIRS})
  add_executable (cuiloa-cppunit-test tests/main.cc)
  target_link_libraries (cuiloa-cppunit-test "${CPPUNIT_LIBRARIES} ${HDF5_LIBRARIES}")
endif ()
