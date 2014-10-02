#include "catch.hpp"

#include <cuiloa/cuiloa.h>
using namespace cuiloa;
using namespace cuiloa::delayed;


TEST_CASE( "delayed arrays", "[delayed]" ) {
  SECTION( "product" ) {
    Array<double,1> a3(4);
    a3.map([](auto& path, auto& val) {
	val = path[0];
      });
    Array<double,1> b3(4);
    b3.map([](auto& path, auto& val) {
	val = 2*path[0];
      });

    auto c3 = a3 * b3;
    REQUIRE( c3(0) == 0 );
    REQUIRE( c3(3) == 18 );
  }

  SECTION( "product bounds" ) {
    Array<double,1> a3(3);
    Array<double,1> a5(5);

    bool exception_thrown = false;
    try {
      a3 * a5;
    } catch (std::length_error& e) {
      exception_thrown = true;
    }
    REQUIRE( exception_thrown );

    Array<double,1> b3(3);
    a3*b3;
  }

  SECTION( "memory references" ) {
    // Return an expression whose array dependencies are stack allocated
    auto fun = [](int x, int y) {
      Array<double,1> a(5);
      a.fill(x);
      Array<double,1> b(5);
      b.fill(y);
      return a * b;
    };

    // Try smashing the stack by successive calls
    auto d = fun(3, 4);
    auto e = fun(7, 8);
    REQUIRE( d(0) == 12 );
    REQUIRE( d(4) == 12 );
    REQUIRE( e(0) == 56 );
    REQUIRE( e(4) == 56) ;
  }

  SECTION( "copy delayed into regular array" ) {
    Array<int,2> a(3,4);
    a.fill(8);

    Array<int,1> b(4);
    b.fill(7);

    a[1] = b * b;

    REQUIRE( a(0,0) == 8 );
    REQUIRE( a(2,2) == 8 );
    REQUIRE( a(1,0) == 49 );
    REQUIRE( a(1,1) == 49 );
  }
}
