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

  SECTION( "broadcasting products" ) {
    using namespace cuiloa::delayed::broadcasting;

    auto a = range(4);
    auto b = reshape<2>(range(8), {2,4});

    auto c = a * b;
    REQUIRE( c.dimensions() == b.dimensions() );
    REQUIRE( c(0,0) == 0 );
    REQUIRE( c(0,3) == 9 );
    REQUIRE( c(1,0) == 0 );
    REQUIRE( c(1,1) == 5 );
    REQUIRE( c(1,2) == 12 );
    REQUIRE( c(1,3) == 21 );

    auto d = b * a;
    REQUIRE( all(c == d) );
  }
}
