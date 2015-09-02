#include "Catch/include/catch.hpp"

#include <necomi/necomi.h>
using namespace necomi;

#include <necomi/codecs/streams.h>
using namespace necomi::streams;


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

  SECTION( "fix_dimension" ) {

    auto a = strided(reshape(range(15), 3, 5));
    
    auto b = fix_dimension(a, 1, 1);
    REQUIRE( b(0) == 1 );
    REQUIRE( b(1) == 6 );
    REQUIRE( b(2) == 11 );

    auto& b1 = b(1);
    b1 = 42;
    REQUIRE( b(1) == 42 );

    REQUIRE( sum(a,0)(1) == 54 );
    //REQUIRE( sum(b) == 54 );

    auto c = delay(a);
    //DebugType<decltype(c(0,0))> dc00;
    //for_each(c, [](const auto&, int& x) {std::cout << x << ' ';});
    //std::cout << std::endl;
    //std::cout << sum(b) << std::endl;
  }
}

SCENARIO( "arrays can be zero padded" ) {
  GIVEN( "a small 2D array" ) {
    auto a = constants<2>({2,3}, 1);
    WHEN( "we zero pad it into a larger size" ) {
      auto b = pad(a, {6, 5});
      THEN(" its output should be predetermined" ) {
	auto truth = reshape<2>(litarray(0, 0, 0, 0, 0,
					 0, 0, 0, 0, 0,
					 0, 1, 1, 1, 0,
					 0, 1, 1, 1, 0,
					 0, 0, 0, 0, 0,
					 0, 0, 0, 0, 0),
	  {6, 5});
	REQUIRE( truth.dims() == b.dims() );
	REQUIRE( sum(power<2>(truth-b)) < 1e-20 );
      }
    }
  }
}
