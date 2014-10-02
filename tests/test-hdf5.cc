#include <cstdio>

#include "catch.hpp"

#include <cuiloa/cuiloa.h>
using namespace cuiloa;

static const char* path = "test-hdf5.h5";

TEST_CASE( "HDF5 storage", "[hdf5]" ) {
  SECTION( "save and load scalar" ) {
    Array<int,0> a0;
    a0() = 94;
    hdf5_save(a0, path, "a0");

    auto b0 = hdf5_load<int,0>(path, "a0");
    REQUIRE( b0.dimensions() == a0.dimensions() );
    REQUIRE( b0() == a0() );

    remove(path);
  }

  SECTION( "save and load tensor" ) {
    Array<int,3> a3(2,3,4);
    a3.map([](auto& path, auto& val) {
        val = path[0]*12 + path[1] * 4 + path[2];
      });
    hdf5_save(a3, path, "a3");
    
    auto b3 = hdf5_load<int,3>(path, "a3");
    REQUIRE( a3.dimensions() == b3.dimensions() );
    REQUIRE( a3(1,2,3) == b3(1,2,3) );
    REQUIRE( a3(0,0,0) == b3(0,0,0) );

    remove(path);
  }
}
 
