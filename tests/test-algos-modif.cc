#include "Catch/include/catch.hpp"

#include <necomi/necomi.h>

using namespace necomi;

TEST_CASE( "modifying array operations", "[algorithms]" ) {
  SECTION( "transform" ) {
    auto a = strided(range<int>(10));
    transform(a, std::negate<>());
    
    REQUIRE( a(0) == 0 );
    REQUIRE( a(3) == -3 );
    REQUIRE( a(4) == -4 );
    REQUIRE( a(7) == -7 );
  }
}
