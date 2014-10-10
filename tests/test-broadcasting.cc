#include "catch.hpp"

#include <cuiloa/cuiloa.h>
using namespace cuiloa;
using namespace cuiloa::delayed;


TEST_CASE( "broadcasting", "[core]" ) {
  SECTION( "widen" ) {
    auto a = reshape<2>(range(20), {4,5});
    Dimensions<3> dims{3,4,5};
    auto b = widen(a, dims);

    REQUIRE( b.dimensions() == dims );
    REQUIRE( b(0,3,2) == 17 );
    REQUIRE( b(2,3,2) == 17 );
    REQUIRE( b(0,1,2) == 7 );
    REQUIRE( b(1,1,2) == 7 );
  }
}
