#include <limits>

#include "catch.hpp"

#include "time.h"

#include <cuiloa/cuiloa.h>
using namespace cuiloa;
using namespace cuiloa::delayed;

static constexpr double float_tol = 1e-2;

static double my_constant_function(const std::array<ArrayIndex,2>& path)
{
  (void) path;
  return 42.0;
}

TEST_CASE( "delayed arrays", "[core]" ) {

  SECTION( "concepts" ) {
    auto a = linspace(0, 42, 10);
    REQUIRE( is_array<decltype(a)>::value );
    REQUIRE( is_indexable<decltype(a)>::value );
  }
  
  SECTION( "from indexable array") {
    // Minimal array definition
    struct IdxArray
    {
      using dtype = double;
      enum { ndim = 1 };
      Dimensions<ndim> dimensions() const { return Dimensions<ndim>(); }
      dtype operator()(const Coordinates<ndim>&) const { return 42; };
    } a;
    auto b = abs(a);
    Coordinates<1> x{37};
    REQUIRE( b(x) == 42 );
  }
  
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

  SECTION( "constant creation" ) {
    auto a = zeros(5,4);
    REQUIRE( a.ndim == 2 );
    REQUIRE( a.dim(0) == 5 );
    REQUIRE( a.dim(1) == 4 );
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
    REQUIRE( std::fabs(norm(a, Norm::Infinity) - 54.72) < float_tol );
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

  SECTION( "identity" ) {
    auto a = identity(2);
    REQUIRE( a(0,0) == 1 );
    REQUIRE( a(1,1) == 1 );
    REQUIRE( a(0,1) == 0 );
    REQUIRE( a(1,0) == 0 );

    auto b = identity(3);
    REQUIRE( a(0,0) == 1 );
    REQUIRE( a(3,3) == 1 );
    REQUIRE( a(1,2) == 0 );
    REQUIRE( a(2,0) == 0 );
  }
  
  SECTION( "1D delayed array function" ) {
    auto a = make_delayed(13, [](auto& coords){ return coords[0]; });
    REQUIRE( a.dim(0) == 13 );
    REQUIRE( a(0) == 0 );
    REQUIRE( a(3) == 3 );
    REQUIRE( a(12) == 12 );
  }
  
  SECTION( "convertible product" ) {
    auto sz = 100;
    auto a = range(sz)*180./sz;
    REQUIRE( a(0) == 0 );
    REQUIRE( a(1) == 1.8 );
    REQUIRE( a(10) == 18 );

    auto b = 180.*range(sz)/sz;
    REQUIRE( b(0) == 0 );
    REQUIRE( b(1) == 1.8 );
    REQUIRE( b(10) == 18 );
  }
  
  SECTION( "power" ) {
    REQUIRE( power<2>(3) == 9 );
    REQUIRE( power<3>(2) == 8 );
    REQUIRE( power<7>(2) == 128 );
    REQUIRE( power<1>(392) == 392 );
    REQUIRE( power<0>(-219) == 1 );
  }
  
  SECTION( "ggd" ) {
    auto a = ggd<2>(range(100.), 7.);
    
    REQUIRE( fabs(a(0) - 1) < float_tol );
    REQUIRE( fabs(a(13) - 0.03177804641749838) < float_tol );
  }
  
  SECTION( "linspace" ) {
    auto a = linspace<double>(0, 30, 10);
    REQUIRE( a.size() == 10 );
    REQUIRE( fabs(a(0) - 0) < float_tol );
    REQUIRE( fabs(a(1) - 3.3333333) < float_tol );
    REQUIRE( fabs(a(7) - 23.3333333) < float_tol );
    REQUIRE( fabs(a(9) - 30) < float_tol );

    auto b = linspace<double>(0, 30, 10, false);
    REQUIRE( b.size() == 10 );
    REQUIRE( fabs(b(0) - 0) < float_tol );
    REQUIRE( fabs(b(1) - 3) < float_tol );
    REQUIRE( fabs(b(7) - 21) < float_tol );
    REQUIRE( fabs(b(9) - 27) < float_tol );

    bool exception_thrown = false;
    try {
      auto c = range<double>(10, 3, 1);
    } catch (std::out_of_range& e) {
      exception_thrown = true;
    }
    REQUIRE( exception_thrown );
    
    exception_thrown = false;
    try {
      auto c = range<double>(3, 10, 0);
    } catch (std::out_of_range& e) {
      exception_thrown = true;
    }
    REQUIRE( exception_thrown );

    auto d = linspace<double>(0, 1, 10);
    REQUIRE( d.size() == 10 );
    REQUIRE( fabs(d(0) - 0) < float_tol );
    REQUIRE( fabs(d(9) - 1) < float_tol );
  }
  
  SECTION( "delayed constant arrays" ) {
    auto a = constants({5}, 13);
    REQUIRE( decltype(a)::ndim == 1 );
    REQUIRE( a.dim(0) == 5 );
    REQUIRE( a(0) == 13 );
    REQUIRE( a(2) == 13 );
  }
  
  /*SECTION( "constexpr delayed arrays" ) {
    auto a = constants({5}, 7);
    Array<double,a(2)> b;
    //REQUIRE( sizeof(b) == sizeof(int)*13 );
    REQUIRE( b.ndim() == 7 );

    //auto func = [](){ return static_cast<int>(time(0)); };
    //int c[func()];
    }*/
  
  SECTION( "average" ) {
    auto a = range<double>(10);
    REQUIRE( fabs(average(a, 0)() - 4.5) < float_tol );

    REQUIRE( fabs(average(a) - 4.5) < float_tol );

    auto b = reshape<2>(range<double>(24), {4,6});
    REQUIRE( fabs(average(b) - 11.5) < float_tol );
  }
  
  SECTION( "variance" ) {
    auto a = range<double>(17);

    REQUIRE( fabs(variance(a, 0, true)() - 25.5) < float_tol );
    REQUIRE( fabs(variance(a, 0, false)() - 24) < float_tol );

    REQUIRE( fabs(variance(a, true) - 25.5) < float_tol );
    REQUIRE( fabs(variance(a, false) - 24) < float_tol );
  }
  
  SECTION( "deviation" ) {
    auto a = range<double>(10);

    REQUIRE( fabs(deviation(a, 0, true)() - 3.0276503540974917 ) < float_tol );
    REQUIRE( fabs(deviation(a, 0, false)() - 2.8722813232690143) < float_tol );
    
    REQUIRE( fabs(deviation(a, true) - 3.0276503540974917 ) < float_tol );
    REQUIRE( fabs(deviation(a, false) - 2.8722813232690143) < float_tol );
  }
  
  SECTION( "zip" ) {
    auto a = range<int>(7);
    auto b = 3 * range<int>(7);
    
    auto c = zip(a,b);
    REQUIRE( c.ndim == 2);
    REQUIRE( c.dim(0) == 7 );
    REQUIRE( c.dim(1) == 2 );

    REQUIRE( c(4,0) == 4 );
    REQUIRE( c(4,1) == 12 );
    REQUIRE( c(5,0) == 5 );
    REQUIRE( c(5,1) == 15 );
  }
  
  SECTION( "round" ) {
    auto a = round(linspace(0, 90, 8));
    auto b = litarray(0, 13, 26, 39, 51, 64, 77, 90);

    REQUIRE( fabs(a(0) - b(0)) < std::numeric_limits<double>::epsilon() );
    REQUIRE( fabs(a(3) - b(3)) < std::numeric_limits<double>::epsilon() );
    REQUIRE( fabs(a(7) - b(7)) < std::numeric_limits<double>::epsilon() );
  }
  
  SECTION( "min/max" ) {
    auto a = litarray(90, 99, 88, 25, 4, 67, 17, 7, 18, 32);

    REQUIRE( min(a) == 4 );
    REQUIRE( max(a) == 99 );
  }
  
  SECTION( "shifted" ) {
    auto a = reshape<2>(range(24), {4,6});
    
    // Positive 2D shift
    auto b = shifted(a, {1,2});
    REQUIRE( b(0,0) == 8 );
    REQUIRE( b(1,0) == 14 );
    REQUIRE( b(4,1) == 0 );
    REQUIRE( b(2,2) == 22 );
    REQUIRE( b(2,4) == 0 );
    
    // Specific out of bounds value
    auto c = shifted(a, {1,2}, 42);
    REQUIRE( c(1,0) == 14 );
    REQUIRE( c(4,1) == 42 );
    REQUIRE( c(2,4) == 42 );
    
    // Pos/neg 2D shift
    auto d = shifted(a, {-1, 2});
    REQUIRE( d(0,0) == 0 );
    REQUIRE( d(1,0) == 2 );
    REQUIRE( d(3,2) == 16 );
    REQUIRE( d(2,5) == 0 );
    
    auto e = range(6);
    
    // 1D positive shifts
    auto f = shifted(e, {-2});
    REQUIRE( f(0) == 0 );
    REQUIRE( f(3) == 1 );
    
    auto g = shifted(e, {7}, 84);
    REQUIRE( g(0) == 84 );
    REQUIRE( g(2) == 84 );
    REQUIRE( g(3) == 84 );
  }
  
  SECTION( "fix dimension") {
    auto a = reshape<3>(range(24), {4, 2, 3});

    auto b = fix_dimension(a, 1, 1);
    Coordinates<2> dims_b{4, 3};
    REQUIRE( b.dimensions() == dims_b );
    REQUIRE( b(0,0) == 3 );
    REQUIRE( b(1,2) == 11 );
    REQUIRE( b(3,1) == 22 );
    
    auto c = fix_dimension(a, 2, 2);
    REQUIRE( c(0,0) == 2 );
    REQUIRE( c(0,1) == 5 );
    REQUIRE( c(2,0) == 14 );
    REQUIRE( c(3,1) == 23 );
    
    auto d = fix_dimension(b, 0, 2);
    REQUIRE( d.ndim == 1 );
    REQUIRE( d.dim(0) == 3 );
    REQUIRE( d(0) == 15 );
    REQUIRE( d(2) == 17 );

    /*
    auto e = immediate(a);
    auto f = fix_dimension(e, 1, 1);
    REQUIRE( f(1,2) == 11 );
    REQUIRE( e(1,1,2) == 11 );
    f(1,2) = 37;
    REQUIRE( f(1,2) == 37 );
    REQUIRE( e(1,1,2) == 37 );
    */
  }
  
  SECTION( "modifiable delayed arrays" ) {
    
    // some delayed arrays are immutable
    auto a = range<int>(24);
    REQUIRE( !a.is_modifiable() );
    
    // modifiable delayed arrays are constructible
    auto b = immediate(a);
    auto c = make_delayed<int>(b.dimensions(),
			       [&b](auto& coords) -> int& {
	return b(coords);
      });
    REQUIRE( c.is_modifiable() );
    
    // change modifiable delayed array
    REQUIRE( b(3) == 3 );
    REQUIRE( c(3) == 3 );
    c(3) = 42;
    REQUIRE( b(3) == 42 );
    REQUIRE( c(3) == 42 );
  }

  SECTION( "stacking" ) {

    auto a = range<int>(24);
    auto b = range<int>(24);

    auto d = stack(a,b);
    REQUIRE( d.ndim == 2 );
    REQUIRE( d.dim(0) == 2 );
    REQUIRE( d(0,3) == 3 );
    REQUIRE( d(1,3) == 3 );
    REQUIRE( d(0,12) == d(1,12) );

    auto e = stack(a,b,a);
    REQUIRE( e.ndim == 2 );
    REQUIRE( e.dim(0) == 3 );
    REQUIRE( e(0,3) == 3 );
    REQUIRE( e(1,3) == 3 );
    REQUIRE( e(2,3) == 3 );
    REQUIRE( e(0,12) == e(1,12) );
  }
}
