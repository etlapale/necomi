#include "catch.hpp"

#include <cuiloa/cuiloa.h>
using namespace cuiloa;
using namespace cuiloa::delayed;

static double my_constant_function(const std::array<ArrayIndex,2>& path)
{
  (void) path;
  return 42.0;
}

TEST_CASE( "delayed arrays", "[core]" ) {
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
    auto pfun = [](int x, int y) {
      Array<double,1> a(5);
      a.fill(x);
      Array<double,1> b(5);
      b.fill(y);
      return a * b;
    };

    // Try smashing the stack by successive calls
    auto d = pfun(3, 4);
    auto e = pfun(7, 8);
    REQUIRE( d(0) == 12 );
    REQUIRE( d(4) == 12 );
    REQUIRE( e(0) == 56 );
    REQUIRE( e(4) == 56) ;

    // Same with sum
    auto sfun = [](int x, int y) {
      Array<double,1> a(5);
      a.fill(x);
      Array<double,1> b(5);
      b.fill(y);
      return a + b;
    };
    auto f = sfun(9, 4);
    auto g = sfun(7, 5);
    REQUIRE( f(0) == 13 );
    REQUIRE( f(4) == 13 );
    REQUIRE( g(0) == 12 );
    REQUIRE( g(4) == 12) ;
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

  SECTION( "mixing delayed and regular arrays" ) {
    Array<int,1> a(7);
    Array<int,1> b(7);
    b.fill(13);

    a = b*b + b*b;

    REQUIRE( a(0) == 338 );
    REQUIRE( a(5) == 338 );
  }

  SECTION( "bool any" ) {
    Array<int,1> a(34);
    a.fill(8);
    Array<int,1> b(34);
    b.fill(10);
    
    REQUIRE( ! any(a > b) );
    a(5) = 42;
    REQUIRE( any(a > b) );
  }

  SECTION( "bool all" ) {
    Array<int,1> a(34);
    a.fill(8);
    Array<int,1> b(34);
    b.fill(10);
    
    REQUIRE( all(b > a) );
    REQUIRE( ! all(a > b) );
    a(5) = 42;
    REQUIRE( ! all(b > a) );
    REQUIRE( ! all(a > b) );
  }

  SECTION( "comparison with constant element" ) {
    Array<int,1> a(34);
    a.fill(8);

    REQUIRE( ! any(a > 10) );
    a(5) = 42;
    REQUIRE( any(a > 10) );
  }

  SECTION( "convert an immediate to a delay and back" ) {
    // Create an immediate array
    Array<int,1> a(7);
    a.map([](auto& path, auto& val) {
	val = path[0];
      });
    // Delay the immediate array
    auto b = delay(a);
    REQUIRE( a(0) == b(0) );
    REQUIRE( a(5) == b(5) );

    // Cast back to an immediate array
    Array<int,1> c = b;
    REQUIRE( c(0) == b(0) );
    REQUIRE( c(5) == b(5) );
    REQUIRE( c(0) == a(0) );
    REQUIRE( c(3) == a(3) );
  }

  SECTION( "delayed from standalone function" ) {
    std::array<ArrayIndex,2> dimensions{{11,21}};
    DelayedArray<int,2,double(&)(const std::array<ArrayIndex,2>&)>
      a(dimensions, my_constant_function);
    Array<int,2> b = a;
    REQUIRE( a(1,1) == b(1,1) );
    REQUIRE( b(4,2) == 42 );
  }

  SECTION( "delayed one-liner" ) {
    auto a = make_delayed<int,2>({{11,21}}, [](auto&) { return 42; });
    REQUIRE( a.dimensions().size() == 2 );
    REQUIRE( a.dimensions()[0] == 11 );
    REQUIRE( a(3,7) == 42 );
  }

  SECTION( "infer make_delayed size template argument" ) {
    std::array<ArrayIndex,2> dims{{11,21}};
    auto a = make_delayed<int>(dims, [](auto&) { return 42; });
    REQUIRE( a.dimensions().size() == 2 );
    REQUIRE( a.dimensions()[0] == 11 );
    REQUIRE( a(3,7) == 42 );
  }
}
