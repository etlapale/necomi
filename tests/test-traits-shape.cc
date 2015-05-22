#include "Catch/include/catch.hpp"

#include <necomi/necomi.h>
using namespace necomi;

TEST_CASE( "shape", "[traits]" ) {

  SECTION( "almost same dimensions" ) {

    StridedArray<int,1> a(7);
    StridedArray<int,1> b(8);

    REQUIRE( almost_same_dimensions(0, a, b) );
    REQUIRE( almost_same_dimensions(0, a, b, a) );
    REQUIRE( almost_same_dimensions(0, b, b, a) );

    StridedArray<int,2> c(5,3);
    StridedArray<int,2> d(5,7);

    REQUIRE( ! almost_same_dimensions(0, c, d) );
    REQUIRE( ! almost_same_dimensions(0, c, c, d) );
    REQUIRE( almost_same_dimensions(1, c, d) );
    REQUIRE( almost_same_dimensions(1, c, d, d) );

    StridedArray<int,3> e(9,3,4);
    StridedArray<int,3> f(9,8,4);

    REQUIRE( ! almost_same_dimensions(0, e, f) );
    REQUIRE( almost_same_dimensions(1, e, f) );
    REQUIRE( ! almost_same_dimensions(2, e, f) );
  }
}
    
