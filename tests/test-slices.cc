#include "catch.hpp"

#include <cuiloa/cuiloa.h>
using namespace cuiloa;


TEST_CASE( "slices", "[base]" ) {
  SECTION( "creating 1D slices" ) {
    Slice<1> s1a(1, 3);
    REQUIRE( s1a.start().size() == 1 );
    REQUIRE( s1a.start()[0] == 1 );
    REQUIRE( s1a.end()[0] == 3 );
    REQUIRE( s1a.strides()[0] == 1 );

    Slice<1> s1c(4, 8, 2);
    REQUIRE( s1c.start()[0] == 4 );
    REQUIRE( s1c.end()[0] == 8 );
    REQUIRE( s1c.strides()[0] == 2 );
  }

  SECTION( "creating 2D slices" ) {
    Slice<2> s2 = (Slice<1>(1,3) , Slice<1>(1,2));

    REQUIRE( s2.start().size() == 2 );
    REQUIRE( s2.start()[0] == 1 );
    REQUIRE( s2.start()[1] == 1 );
    REQUIRE( s2.end()[0] == 3 );
    REQUIRE( s2.end()[1] == 2 );
    REQUIRE( s2.strides()[0] == 1 );
    REQUIRE( s2.strides()[1] == 1 );
  }

  SECTION( "slice factory" ) {
    auto s2 = (slice(1,3),slice(1,2));

    REQUIRE( s2.start().size() == 2 );
    REQUIRE( s2.start()[0] == 1 );
    REQUIRE( s2.start()[1] == 1 );
    REQUIRE( s2.end()[0] == 3 );
    REQUIRE( s2.end()[1] == 2 );
    REQUIRE( s2.strides()[0] == 1 );
    REQUIRE( s2.strides()[1] == 1 );
  }

  SECTION( "slice from initializer lists" ) {
    Slice<2> s2({{{{1,3}}, {{1,2}}}});

    REQUIRE( s2.start()[0] == 1 );
    REQUIRE( s2.start()[1] == 1 );
    REQUIRE( s2.end()[0] == 3 );
    REQUIRE( s2.end()[1] == 2 );
    REQUIRE( s2.strides()[0] == 1 );
    REQUIRE( s2.strides()[1] == 1 );
  }
}
