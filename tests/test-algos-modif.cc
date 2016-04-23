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

    auto b = strided(range<int>(10));
    auto c = strided(range<int>(10));
    transform(b, c, std::plus<>());

    REQUIRE( b(0) == 0 );
    REQUIRE( b(1) == 2 );
    REQUIRE( b(7) == 14 );

    auto d = strided(range<int>(8));
    bool length_error_thrown = false;
    try {
      transform(c, d, std::plus<>());
    } catch (std::length_error&) {
      length_error_thrown = true;
    }
    REQUIRE( length_error_thrown );
  }
}
