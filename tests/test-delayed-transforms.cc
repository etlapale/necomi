#include "Catch/include/catch.hpp"

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

    auto d = concat(a, b, a);
    REQUIRE( d.ndim == 1 );
    REQUIRE( d.dim(0) == 11 );
    REQUIRE( d(0) == 67 );
    REQUIRE( d(2) == 67 );
    REQUIRE( d(3) == 38 );
    REQUIRE( d(7) == 38 );
    REQUIRE( d(8) == 67 );
    REQUIRE( d(10) == 67 );

    auto e = reshape(range(15), 3, 5);
    auto f = reshape(range(35), 7, 5);
    auto g = concat(e, f);
    REQUIRE( g.ndim == 2 );
    REQUIRE( g.dim(0) == 10 );
    REQUIRE( g.dim(1) == 5 );
    REQUIRE( g(2,1) == 11 );
    REQUIRE( g(4,0) == 5 );
    REQUIRE( g(7,3) == 23 );

    auto m = reshape(range(15), 5, 3);
    auto n = reshape(range(35), 5, 7);
    auto p = concat(1, m, n);
    REQUIRE( p.ndim == 2 );
    REQUIRE( p.dim(0) == 5 );
    REQUIRE( p.dim(1) == 10 );
    REQUIRE( p(0,7) == 4 );
    REQUIRE( p(1,1) == 4 );
    REQUIRE( p(2,8) == 19 );
  }
}
