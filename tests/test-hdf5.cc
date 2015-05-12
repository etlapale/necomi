#include <cstdio>

#include "catch.hpp"

#include <necomi/necomi.h>
using namespace necomi;

static const char* path = "test-hdf5.h5";

TEST_CASE( "HDF5 storage", "[hdf5]" ) {
  SECTION( "save and load scalar" ) {
    StridedArray<int,0> a0;
    a0() = 94;
    hdf5_save(path, "a0", a0);

    auto b0 = hdf5_load<int,0>(path, "a0");
    REQUIRE( b0.dims() == a0.dims() );
    REQUIRE( b0() == a0() );

    remove(path);
  }

  SECTION( "save and load tensor" ) {
    StridedArray<int,3> a3(2,3,4);
    a3.map([](auto& path, auto& val) {
        val = path[0]*12 + path[1] * 4 + path[2];
      });
    hdf5_save(path, "a3", a3);
    
    auto b3 = hdf5_load<int,3>(path, "a3");
    REQUIRE( a3.dims() == b3.dims() );
    REQUIRE( a3(1,2,3) == b3(1,2,3) );
    REQUIRE( a3(0,0,0) == b3(0,0,0) );

    remove(path);
  }
}
