#include "Catch/include/catch.hpp"

#include <necomi/necomi.h>
using namespace necomi;


TEST_CASE( "broadcasting", "[core]" ) {
  SECTION( "widen" ) {
    auto a = reshape(range(20), 4, 5);
    std::array<std::size_t,3> dims{3,4,5};
    auto b = widen(dims, a);

    REQUIRE( b.dims() == dims );
    REQUIRE( b(0,3,2) == 17 );
    REQUIRE( b(2,3,2) == 17 );
    REQUIRE( b(0,1,2) == 7 );
    REQUIRE( b(1,1,2) == 7 );
  }

  SECTION( "right widening" ) {
    auto a = range(3);
    std::array<std::size_t,2> dims{3,4};
    auto b = widen_right(dims, a);

    REQUIRE( b.dims() == dims );
    REQUIRE( b(0,0) == 0 );
    REQUIRE( b(0,1) == 0 );
    REQUIRE( b(0,2) == 0 );
    REQUIRE( b(1,0) == 1 );
    REQUIRE( b(1,1) == 1 );
    REQUIRE( b(2,0) == 2 );
    REQUIRE( b(2,1) == 2 );
  }

  SECTION( "broadcasting products" ) {
    using namespace necomi::broadcasting;

    auto a = range(4);
    auto b = reshape(range(8), 2, 4);

    auto c = a * b;
    REQUIRE( c.dims() == b.dims() );
    REQUIRE( c(0,0) == 0 );
    REQUIRE( c(0,3) == 9 );
    REQUIRE( c(1,0) == 0 );
    REQUIRE( c(1,1) == 5 );
    REQUIRE( c(1,2) == 12 );
    REQUIRE( c(1,3) == 21 );

    auto d = b * a;
    REQUIRE( all(c == d) );
  }

  SECTION( "broadcasting heterogeneous arrays" ) {
    using namespace necomi::broadcasting;

    auto a = strided_array(range(4));
    auto b = reshape(range(8), 2, 4);

    auto c = a - b;
    auto d = a + b;
    auto e = a * b;
    auto f = a / b;

    REQUIRE( c.dims() == d.dims() );
    REQUIRE( d.dims() == e.dims() );
    REQUIRE( e.dims() == f.dims() );
  }
}
