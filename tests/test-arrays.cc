#include "catch.hpp"

#include <cuiloa/cuiloa.h>
using namespace cuiloa;


TEST_CASE( "basic array operations", "[base]" ) {
  SECTION( "array sizes" ) {
    Array<double,0> a0;
    REQUIRE( a0.size() == 1 );

    Array<double,1> a1(127);
    REQUIRE( a1.size() == 127 );

    Array<double,2> a2(3,4);
    REQUIRE( a2.size() == 3*4 );

    Array<double,5> a5(8,1,7,2,9);
    REQUIRE( a5.size() == 8*1*7*2*9 );
  }

  SECTION( "array dimensions" ) {
    Array<double,0> a0;
    REQUIRE(a0.dimensions().empty());

    Array<double,1> a1(127);
    REQUIRE( a1.dimensions().size() == 1 );
    REQUIRE( a1.dimensions()[0] == 127 );

    Array<double,2> a2(3,4);
    REQUIRE( a2.dimensions().size() == 2 );
    REQUIRE( a2.dimensions()[0] == 3 );
    REQUIRE( a2.dimensions()[1] == 4 );

    Array<double,5> a5(8,1,7,2,9);
    REQUIRE( a5.dimensions().size() == 5 );
    REQUIRE( a5.dimensions()[1] == 1 );
    REQUIRE( a5.dimensions()[4] == 9 );
  }

  SECTION( "array strides" ) {
    Array<double,0> a0;
    REQUIRE( a0.strides().empty() );

    Array<double,1> a1(127);
    REQUIRE( a1.strides().size() == 1);
    REQUIRE( a1.strides()[0] == 1 );

    Array<double,2> a2(3,4);
    REQUIRE( a2.strides().size() == 2 );
    REQUIRE( a2.strides()[0] == 4 );
    REQUIRE( a2.strides()[1] == 1 );

    Array<double,5> a5(8,1,7,2,9);
    REQUIRE( a5.strides().size() == 5 );
    REQUIRE( a5.strides()[1] == 1*7*2*9 );
    REQUIRE( a5.strides()[4] == 1 );
  }

  SECTION( "indices" ) {
    Array<double,0> a0;
    REQUIRE( a0.index() == 0 );

    Array<double,1> a1(127);
    REQUIRE( a1.index(4) == 4 );
    REQUIRE( a1.index(86) == 86 );

    Array<double,2> a2(3,4);
    REQUIRE( a2.index(0,2) == 2 );
    REQUIRE( a2.index(1,3) == 7 );
    REQUIRE( a2.index(2,1) == 9 );
  }

  SECTION( "data access" ) {
    Array<int,0> a0;
    a0() = 123;
    REQUIRE( a0() == 123 );
    REQUIRE( a0.data()[0] == 123 );

    Array<int,1> a1(127);
    a1(32) = 123;
    a1(69) = 456;
    REQUIRE( a1(32) == 123 );
    REQUIRE( a1(69) == 456 );
    REQUIRE( a1.data()[32] == 123 );
    REQUIRE( a1.data()[69] == 456 );

    Array<double,2> a2(3,4);
    for (unsigned int i = 0; i < a2.size(); i++)
      a2.data()[i] = i;
    REQUIRE( a2(1,2) == 6 );
    REQUIRE( a2(2,3) == a2.index(2,3) );
  }

  SECTION( "creation from existing data" ) {
    const unsigned int height = 7;
    const unsigned int width = 6;
    
    double* data = new double[height*width];
    for (unsigned int i = 0; i < height; i++)
      for (unsigned int j = 0; j < width; j++)
        data[j+i*width] = j+i*width;

    Array<double,2> a(data, height, width);
    REQUIRE( a(3,2) == 2+3*width );
    REQUIRE( a(5,7) == 7+5*width );

    delete [] data;

    const char* str = "Hello world!";
    Array<char,1> b(const_cast<char*>(str), 12);
    REQUIRE( b(0) == 'H' );
    REQUIRE( b(6) == 'w' );
  }

  SECTION( "slicing" ) {
    Array<int,1> a1(127);
    for (unsigned int i = 0; i < a1.size(); i++)
      a1(i) = i;
    auto a1s = a1[45];
    REQUIRE( a1s.dimensions().empty() );
    REQUIRE( a1s() == 45 );

    Array<int,2> a2(3,4);
    for (unsigned int i = 0; i < a2.size(); i++)
      a2.data()[i] = i;
    auto a2s = a2[2];
    REQUIRE( a2s.dimensions().size() == 1 );
    REQUIRE( a2s.size() == 4 );
    REQUIRE( a2s.dimensions()[0] == 4 );
    REQUIRE( a2s.strides().size() == 1 );
    REQUIRE( a2s.strides()[0] == 1 );
    REQUIRE( a2s(2) == 10 );
    REQUIRE( a2s(3) == 11 );
  }

  SECTION( "maps" ) {
    auto incr([](auto& path, auto& val) {
        (void) path;
        val += 1;
      });

    Array<int,0> a0;
    a0() = 123;
    a0.map(incr);
    REQUIRE( a0() == 124 );
    
    Array<int,1> a1(127);
    for (unsigned int i = 0; i < a1.size(); i++)
      a1(i) = i;
    a1.map(incr);
    REQUIRE( a1(43) == 44 );
    REQUIRE( a1(120) == 121 );

    Array<int,3> a3(2,3,4);
    a3.map([](auto& path, auto& val) {
        val = path[0]*12 + path[1]*4 + path[2];
      });
    REQUIRE( a3(1,2,0) == 20 );

    a1.fill(42);
    REQUIRE( a1(34) == 42 );
    REQUIRE( a1(56) == 42 );
  }

  SECTION( "copies" ) {
    Array<int,0> a0;
    a0() = 123;

    auto a0v(a0);
    REQUIRE( a0v() == 123 );
    a0() = 456;
    REQUIRE( a0v() == 456 );
    a0v() = 789;
    REQUIRE( a0() == 789 );

    auto a0c = a0.copy();
    a0() = 456;
    REQUIRE( a0() == 456 );
    REQUIRE( a0c() == 789 );
    REQUIRE( a0v() == 456 );
  }

  SECTION( "similar shapes" ) {
    Array<int,1> a(127);

    auto b = a.zeros_like();
    REQUIRE( b.dimensions() == a.dimensions() );
    REQUIRE( b(42) == 0 );
    REQUIRE( b(73) == 0 );

    auto c = a.constants_like(42);
    REQUIRE( c.dimensions() == a.dimensions() );
    REQUIRE( c(14) == 42 );
    REQUIRE( c(88) == 42 );
  }

  SECTION( "sums" ) {
    Array<int,3> a(2,3,4);
    a.map([](auto& path, auto& val) {
        val = path[0]*12 + path[1] * 4 + path[2];
      });

    auto a0 = a.sum(0);
    REQUIRE( a0.dimensions().size() == 2 );
    REQUIRE( a0.dimensions()[0] == 3 );
    REQUIRE( a0.dimensions()[1] == 4 );
    REQUIRE( a0(0,0) == 12 );
    REQUIRE( a0(1,1) == 22 );
    REQUIRE( a0(2,3) == 34 );

    auto a1 = a.sum(1);
    REQUIRE( a1.dimensions().size() == 2 );
    REQUIRE( a1.dimensions()[0] == 2 );
    REQUIRE( a1.dimensions()[1] == 4 );
    REQUIRE( a1(0,0) == 12 );
    REQUIRE( a1(0,1) == 15 );
    REQUIRE( a1(1,1) == 51 );

    auto a2 = a.sum(2);
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

  SECTION( "inplace mappping" ) {
    Array<int,3> a(2,3,4);
    a.map([](auto& path, auto& val) {
        val = path[0]*12 + path[1] * 4 + path[2];
      });

    auto& b = a /= 2;

    //REQUIRE( a == &b );
    REQUIRE( a(0,0,0) == 0 );
    REQUIRE( a(1,1,1) == 8 );
    REQUIRE( a(1,0,3) == 7 );
    REQUIRE( a(0,2,2) == 5 );
  }

  SECTION( "contiguity check" ) {
    Array<int,3> a(2,3,4);
    REQUIRE( a.contiguous() );

    auto b = a[1];
    REQUIRE( b.contiguous() );
  }

  SECTION( "predicate any" ) {
    Array<int,3> a(2,3,4);
    a.fill(42);
    REQUIRE( ! a.any([](auto val) { return val != 42; }) );
    REQUIRE( a.any([](auto val) { return val == 42; }) );
    REQUIRE( ! a.any([](auto val) { return val == 33; }) );

    a(1,1,1) = 33;
    REQUIRE( a.any([](auto val) { return val == 33; }) );
  }

}
