#include "catch.hpp"

#include <necomi/necomi.h>
using namespace necomi;

TEST_CASE( "transformations", "[delayed]" ) {

  SECTION( "concatenate" ) {
    
    auto a = constants({3}, 67);
    auto b = constants({5}, 38);
    auto c = concat(a, b);
    
    REQUIRE( c.ndim == 1 );
    REQUIRE( c.dim(0) == 8 );
    REQUIRE( c(0) == 67 );
    REQUIRE( c(2) == 67 );
    REQUIRE( c(3) == 38 );
    REQUIRE( c(7) == 38 );

    auto d = constants({7}, 42);
    //auto e = concat(a, b, a, d, 0);
  }
}
