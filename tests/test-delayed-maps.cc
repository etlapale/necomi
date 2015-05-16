#include "catch.hpp"

#include <necomi/necomi.h>

using namespace necomi;

TEST_CASE( "delayed arrays from map operations", "[delayed]" ) {
  SECTION( "map" ) {
    auto a = range<int>(10);
    auto b = map(a, std::negate<>());
    REQUIRE( a(0) == -b(0) );
    REQUIRE( a(3) == -b(3) );
    REQUIRE( a(4) == -b(4) );
    REQUIRE( a(7) == -b(7) );
    
    auto c = map(b, std::negate<>());
    REQUIRE( all(c == a) );

    auto d = map(a, [](const auto& x) { return x + 3; });
    REQUIRE( all(a + 3 == d ) );
    REQUIRE( d(0) == 3 );
    REQUIRE( d(1) == 4 );
  }
}
