#include "catch.hpp"

#include <cuiloa/cuiloa.h>
using namespace cuiloa;


TEST_CASE( "basic array operations", "[base]" ) {
  SECTION( "remove coordinates" ) {
    Coordinates<4> c = {1, 2, 3, 4};

    auto c1 = remove_coordinate(c, 2);
    Coordinates<3> d1 = {1, 2, 4};
    REQUIRE( c1 == d1 );

    auto c2 = remove_coordinate(c, 0);
    Coordinates<3> d2 = {2, 3, 4};
    REQUIRE( c2 == d2 );

    auto c3 = remove_coordinate(c, 3);
    Coordinates<3> d3 = {1, 2, 3};
    REQUIRE( c3 == d3 );
  }

  SECTION( "add coordinates" ) {
    Coordinates<3> c = {1, 2, 3};

    auto c1 = add_coordinate(c, 2);
    c1[2] = 0;
    Coordinates<4> d1 = {1, 2, 0, 3};
    REQUIRE( c1 == d1 );

    auto c2 = add_coordinate(c, 0);
    c2[0] = 0;
    Coordinates<4> d2 = {0, 1, 2, 3};
    REQUIRE( c2 == d2 );
    REQUIRE( c2 != d1 );

    auto c3 = add_coordinate(c, 3);
    c3[3] = 0;
    Coordinates<4> d3 = {1, 2, 3, 0};
    REQUIRE( c3 == d3 );
    REQUIRE( c3 != d2 );
  }
  
  SECTION( "append coordinate" ) {
    Coordinates<3> c = {1, 2, 3};

    auto c1 = append_coordinate(c, 7);
    Coordinates<4> d1 = {1, 2, 3, 7};
    REQUIRE( c1 == d1 );
  }

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

    Array<double,2> a(data, [](double*){}, height, width);
    REQUIRE( a(3,2) == 2+3*width );
    REQUIRE( a(5,7) == 7+5*width );

    delete [] data;

    const char* str = "Hello world!";
    Array<char,1> b(const_cast<char*>(str), [](char*){}, 12);
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

    // Out of bounds slicing

    bool exception_thrown = false;
    try {
      auto bad_a1 = a1[200];
    } catch (std::out_of_range& e) {
      exception_thrown = true;
    }
    REQUIRE( exception_thrown );

    exception_thrown = false;
    try {
      auto bad_a2 = a2[0][7];
    } catch (std::out_of_range& e) {
      exception_thrown = true;
    }
    REQUIRE( exception_thrown );

    exception_thrown = false;
    try {
      auto bad_a2 = a2[7][0];
    } catch (std::out_of_range& e) {
      exception_thrown = true;
    }
    REQUIRE( exception_thrown );
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

  SECTION( "cumulative sum" ) {
    using namespace delayed;
    auto a = range(1,7);
    auto b = cumsum(a);

    REQUIRE( b.dimensions() == a.dimensions() );
    REQUIRE( b(0) == 1 );
    REQUIRE( b(1) == 3 );
    REQUIRE( b(2) == 6 );
    REQUIRE( b(3) == 10 );
    REQUIRE( b(4) == 15 );
    REQUIRE( b(5) == 21 );

    auto c = reshape<2>(range(56), {{7,8}});
    auto c0a = cumsum(c);
    REQUIRE( c0a(0,0) == 0 );
    REQUIRE( c0a(3,5) == 68 );
    REQUIRE( c0a(6,2) == 182 );
    auto c0b = cumsum(c, 0);
    REQUIRE( c0b(0,0) == 0 );
    REQUIRE( c0b(3,5) == 68 );
    REQUIRE( c0b(6,2) == 182 );

    auto c1 = cumsum(c, 1);
    REQUIRE( c1(0,4) == 10 );
    REQUIRE( c1(3,2) == 75 );
    REQUIRE( c1(5,6) == 301 );
  }

  SECTION( "index to path" ) {
    auto strides = default_strides<2>({{4, 5}});
    REQUIRE( strides.size() == 2 );
    REQUIRE( strides[0] == 5 );
    REQUIRE( strides[1] == 1 );

    auto path = index_to_path(17, strides);
    REQUIRE( path.size() == 2 );
    REQUIRE( path[0] == 3 );
    REQUIRE( path[1] == 2 );
  }

  SECTION( "increment operator" ) {
    using namespace cuiloa::delayed;
    
    Array<int,2> a = reshape<2>(range(20), {4, 5});
    Array<int,2> b = 3 * reshape<2>(range(20), {4, 5});
    
    a += b;
    REQUIRE( a(0,0) == 0 );
    REQUIRE( a(1,0) == 20 );
    REQUIRE( a(3,4) == 76 );

    b += b;
    REQUIRE( b(0,0) == 0 );
    REQUIRE( b(2,4) == 84 );
    REQUIRE( b(3,1) == 96 );
  }
  
  SECTION( "literal construction" ) {
    auto b = litarray<int>(12);
    REQUIRE( b.dim(0) == 1 );
    REQUIRE( b(0) == 12 );
    
    auto c = litarray(12, 35, 19, 2, 982, 32, 56);
    REQUIRE( c.dim(0) == 7 );
    REQUIRE( c(4) == 982 );
  }
  
  SECTION( "copy to a slice" ) {
    using namespace cuiloa::delayed;

    auto a = immediate(reshape<2>(range(24), {6,4}));
    auto a0 = a[0];
    REQUIRE( a0(0) == 0 );
    REQUIRE( a0(1) == 1 );
    REQUIRE( a0(2) == 2 );
    
    auto b = immediate(range(8,12));
    a[0] = b;

    REQUIRE( a0(1) == 9 );
    REQUIRE( a0(2) == 10 );
    REQUIRE( a0(3) == 11 );

    REQUIRE( a(0,0) == 8 );
    REQUIRE( a(0,1) == 9 );
    REQUIRE( a(0,2) == 10 );
  }
  
  SECTION( "immediate behavior" ) {
    using namespace cuiloa::delayed;

    auto a = immediate(range(24));

    // Ensure that immediate always returns a copy
    auto b = immediate(a);
    REQUIRE( a(2) == b(2) );
    b(2) = 42;
    REQUIRE( a(2) == 2 );
    REQUIRE( b(2) == 42 );
  }
}
