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

    Array<int,1> a3(4);
    a3.map([](auto& path, auto& val) {
	val = path[0];
      });
    Array<int,1> b3(4);
    b3.map([](auto& path, auto& val) {
	val = 2*path[0];
      });

    auto c3 = a3 * b3;
    REQUIRE( c3(0) == 0 );
    REQUIRE( c3(3) == 18 );

    int x = 13;
    auto d3 = x * a3;
    REQUIRE( d3(0) == 0 );
    REQUIRE( d3(1) == 13 );
    REQUIRE( d3(3) == 39 );
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

  SECTION( "similar shape constructors" ) {
    Array<int,1> a(127);

    auto b = zeros_like(a);
    REQUIRE( b.dimensions() == a.dimensions() );
    REQUIRE( b(42) == 0 );
    REQUIRE( b(73) == 0 );

    auto c = constants_like(a, 42);
    REQUIRE( c.dimensions() == a.dimensions() );
    REQUIRE( c(14) == 42 );
    REQUIRE( c(88) == 42 );
  }

  SECTION( "range creation" ) {
    auto a = range(8);
    REQUIRE( a.dimensions().size() == 1 );
    REQUIRE( a.dimensions()[0] == 8 );
    REQUIRE( a(0) == 0 );
    REQUIRE( a(4) == 4 );
    REQUIRE( a(7) == 7 );

    auto b = range(2,8);
    REQUIRE( b.dimensions().size() == 1 );
    REQUIRE( b.dimensions()[0] == 6 );
    REQUIRE( b(0) == 2 );
    REQUIRE( b(4) == 6 );
    REQUIRE( b(5) == 7 );

    auto c = range(0,6,2);
    REQUIRE( c.dimensions()[0] == 3 );
    REQUIRE( c(0) == 0 );
    REQUIRE( c(1) == 2 );
    REQUIRE( c(2) == 4 );

    auto d = range(0,6,2);
    REQUIRE( d.dimensions()[0] == 3 );
    REQUIRE( d(0) == 0 );
    REQUIRE( d(1) == 2 );
    REQUIRE( d(2) == 4 );

    auto e = range(0,7,2);
    REQUIRE( e.dimensions()[0] == 4 );
    REQUIRE( e(0) == 0 );
    REQUIRE( e(1) == 2 );
    REQUIRE( e(2) == 4 );
    REQUIRE( e(3) == 6 );
  }

  SECTION( "reshaping" ) {
    auto a = range(20);
    std::array<ArrayIndex,2> dims{{4,5}};
    auto b = reshape(a, dims);
    REQUIRE( b.dimensions() == dims );
    REQUIRE( b(0,0) == 0);
    REQUIRE( b(1,1) == 6 );
    REQUIRE( b(3,2) == 17 );
  }

  SECTION( "sums" ) {
    Array<int,3> a(2,3,4);
    a.map([](auto& path, auto& val) {
        val = path[0]*12 + path[1] * 4 + path[2];
      });

    auto a0 = sum(a, 0);
    REQUIRE( a0.dimensions().size() == 2 );
    REQUIRE( a0.dimensions()[0] == 3 );
    REQUIRE( a0.dimensions()[1] == 4 );
    REQUIRE( a0(0,0) == 12 );
    REQUIRE( a0(1,1) == 22 );
    REQUIRE( a0(2,3) == 34 );

    auto a1 = sum(a, 1);
    REQUIRE( a1.dimensions().size() == 2 );
    REQUIRE( a1.dimensions()[0] == 2 );
    REQUIRE( a1.dimensions()[1] == 4 );
    REQUIRE( a1(0,0) == 12 );
    REQUIRE( a1(0,1) == 15 );
    REQUIRE( a1(1,1) == 51 );

    auto a2 = sum(a, 2);
    REQUIRE( a2.dimensions().size() == 2 );
    REQUIRE( a2.dimensions()[0] == 2 );
    REQUIRE( a2.dimensions()[1] == 3 );
    REQUIRE( a2(0,0) == 6 );
    REQUIRE( a2(1,1) == 70 );
    REQUIRE( a2(1,2) == 86 );

    REQUIRE( sum(a) == 276 );
    REQUIRE( sum(a1) == 276 );
    REQUIRE( sum(a2) == 276 );
  }

  const double float_tol = 1e-2;

  SECTION( "absolute value" ) {
    auto a = abs(range(5) - 2);
    REQUIRE( a(0) == 2 );
    REQUIRE( a(1) == 1 );
    REQUIRE( a(2) == 0 );
    REQUIRE( a(3) == 1 );
    REQUIRE( a(4) == 2 );

    auto b = abs(range(-4.7, 3.2, 0.43));
    REQUIRE( std::fabs(b(0) - 4.7) < float_tol );
    REQUIRE( std::fabs(b(4) - 2.98) < float_tol );
    REQUIRE( std::fabs(b(14) - 1.32) < float_tol );
  }

  SECTION( "norms" ) {
    auto a = range(-4.7, 3.2, 0.43) * range(19.0);
    REQUIRE( std::fabs(norm(a, InfinityNorm) - 54.72) < float_tol );
  }

  SECTION( "roll" ) {
    auto a = roll(range(10), 1);
    REQUIRE( a(0) == 9 );
    REQUIRE( a(1) == 0 );
    REQUIRE( a(9) == 8 );

    auto b = roll(reshape<2>(range(10), {2,5}), 1, 0);
    REQUIRE( b(0,0) == 5 );
    REQUIRE( b(0,1) == 6 );
    REQUIRE( b(1,0) == 0 );
    REQUIRE( b(1,1) == 1 );

    auto c = roll(reshape<2>(range(10), {2,5}), 1, 1);
    REQUIRE( c(0,0) == 4 );
    REQUIRE( c(0,1) == 0 );
    REQUIRE( c(1,0) == 9 );
    REQUIRE( c(1,1) == 5 );
  }

  SECTION( "avoiding demotions" ) {
    Array<int,1> a = range(10);
    auto b = 2.3 * a;

    REQUIRE( std::fabs(b(1) - 2.3) < float_tol );
    REQUIRE( std::fabs(b(8) - 18.4) < float_tol );
  }
}
