#include <algorithm>
#include <cstdio>

#include "Catch/include/catch.hpp"

#include <necomi/necomi.h>
using namespace necomi;

static const char* path = "test-hdf5.h5";

TEST_CASE( "HDF5 storage", "[hdf5]" ) {
  SECTION( "save and load scalar" ) {
    StridedArray<int,0> a0;
    a0() = 94;
    hdf5_save(path, "a0", a0);

    auto b0 = hdf5_load<int>(path, "a0");
    auto a0_dims = a0.dims();
    auto b0_dims = b0.dims();
    REQUIRE( std::equal(a0_dims.cbegin(), a0_dims.cend(),
			b0_dims.cbegin(), b0_dims.cend()) );
    REQUIRE( b0() == a0() );

    remove(path);
  }

  SECTION( "save and load tensor" ) {
    StridedArray<int,3> a3(2,3,4);
    a3.map([](auto& path, auto& val) {
        val = path[0]*12 + path[1] * 4 + path[2];
      });
    hdf5_save(path, "a3", a3);
    
    auto b3 = hdf5_load<int>(path, "a3");
    auto a3_dims = a3.dims();
    auto b3_dims = b3.dims();
    REQUIRE( std::equal(a3_dims.cbegin(), a3_dims.cend(),
			b3_dims.cbegin(), b3_dims.cend()) );
    REQUIRE( a3(1,2,3) == b3(1,2,3) );
    REQUIRE( a3(0,0,0) == b3(0,0,0) );

    remove(path);
  }
}
