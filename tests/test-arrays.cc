#include "Catch/include/catch.hpp"

#include <necomi/necomi.h>
using namespace necomi;


TEST_CASE( "basic array operations", "[base]" ) {
  SECTION( "remove coordinates" ) {
    std::array<std::size_t,4> c = {1, 2, 3, 4};

    auto c1 = remove_coordinate(c, 2);
    std::array<std::size_t,3> d1 = {1, 2, 4};
    REQUIRE( c1 == d1 );

    auto c2 = remove_coordinate(c, 0);
    std::array<std::size_t,3> d2 = {2, 3, 4};
    REQUIRE( c2 == d2 );

    auto c3 = remove_coordinate(c, 3);
    std::array<std::size_t,3> d3 = {1, 2, 3};
    REQUIRE( c3 == d3 );
  }

  SECTION( "add coordinates" ) {
    std::array<std::size_t,3> c = {1, 2, 3};

    auto c1 = add_coordinate(c, 2);
    c1[2] = 0;
    std::array<std::size_t,4> d1 = {1, 2, 0, 3};
    REQUIRE( c1 == d1 );

    auto c2 = add_coordinate(c, 0);
    c2[0] = 0;
    std::array<std::size_t,4> d2 = {0, 1, 2, 3};
    REQUIRE( c2 == d2 );
    REQUIRE( c2 != d1 );

    auto c3 = add_coordinate(c, 3);
    c3[3] = 0;
    std::array<std::size_t,4> d3 = {1, 2, 3, 0};
    REQUIRE( c3 == d3 );
    REQUIRE( c3 != d2 );
  }
  
  SECTION( "append coordinate" ) {
    std::array<std::size_t,3> c = {1, 2, 3};

    auto c1 = append_coordinate(c, 7);
    std::array<std::size_t,4> d1 = {1, 2, 3, 7};
    REQUIRE( c1 == d1 );
  }

  SECTION( "concepts" ) {
    StridedArray<double,0> a0;
    REQUIRE( is_array<decltype(a0)>::value );
  }

  SECTION( "array sizes" ) {
    StridedArray<double,0> a0;
    REQUIRE( size(a0) == 1 );

    StridedArray<double,1> a1(127);
    REQUIRE( size(a1) == 127 );

    StridedArray<double,2> a2(3,4);
    REQUIRE( size(a2) == 3*4 );

    StridedArray<double,5> a5(8,1,7,2,9);
    REQUIRE( size(a5) == 8*1*7*2*9 );
  }

  SECTION( "array dimensions" ) {
    StridedArray<double,0> a0;
    REQUIRE( a0.ndim() == 0 );
    REQUIRE( size(a0) == 1 );

    StridedArray<double,1> a1(127);
    REQUIRE( a1.ndim() == 1 );
    REQUIRE( a1.dim(0) == 127 );

    StridedArray<double,2> a2(3,4);
    REQUIRE( a2.ndim() == 2 );
    REQUIRE( a2.dim(0) == 3 );
    REQUIRE( a2.dim(1) == 4 );

    StridedArray<double,5> a5(8,1,7,2,9);
    REQUIRE( a5.ndim() == 5 );
    REQUIRE( a5.dim(1) == 1 );
    REQUIRE( a5.dim(4) == 9 );
  }

  SECTION( "array strides" ) {
    StridedArray<double,0> a0;
    REQUIRE( a0.strides().empty() );

    StridedArray<double,1> a1(127);
    REQUIRE( a1.strides().size() == 1);
    REQUIRE( a1.strides()[0] == 1 );

    StridedArray<double,2> a2(3,4);
    REQUIRE( a2.strides().size() == 2 );
    REQUIRE( a2.strides()[0] == 4 );
    REQUIRE( a2.strides()[1] == 1 );

    StridedArray<double,5> a5(8,1,7,2,9);
    REQUIRE( a5.strides().size() == 5 );
    REQUIRE( a5.strides()[1] == 1*7*2*9 );
    REQUIRE( a5.strides()[4] == 1 );
  }

  SECTION( "indices" ) {
    StridedArray<double,0> a0;
    REQUIRE( strided_index(a0) == 0 );

    StridedArray<double,1> a1(127);
    REQUIRE( strided_index(a1, 4) == 4 );
    REQUIRE( strided_index(a1, 86) == 86 );

    StridedArray<double,2> a2(3,4);
    REQUIRE( strided_index(a2, 0, 2) == 2 );
    REQUIRE( strided_index(a2, 1, 3) == 7 );
    REQUIRE( strided_index(a2, 2, 1) == 9 );
  }

  SECTION( "data access" ) {
    StridedArray<int,0> a0;
    a0() = 123;
    REQUIRE( a0() == 123 );
    REQUIRE( a0.data()[0] == 123 );

    StridedArray<int,1> a1(127);
    a1(32) = 123;
    a1(69) = 456;
    REQUIRE( a1(32) == 123 );
    REQUIRE( a1(69) == 456 );
    REQUIRE( a1.data()[32] == 123 );
    REQUIRE( a1.data()[69] == 456 );

    StridedArray<double,2> a2(3,4);
    for (unsigned int i = 0; i < size(a2); i++)
      a2.data()[i] = i;
    REQUIRE( a2(1,2) == 6 );
    REQUIRE( a2(2,3) == strided_index(a2, 2, 3) );
  }

  SECTION( "creation from existing data" ) {
    const unsigned int height = 7;
    const unsigned int width = 6;
    
    double* data = new double[height*width];
    for (unsigned int i = 0; i < height; i++)
      for (unsigned int j = 0; j < width; j++)
        data[j+i*width] = j+i*width;

    StridedArray<double,2> a(data, [](double*){}, height, width);
    REQUIRE( a(3,2) == 2+3*width );
    REQUIRE( a(5,7) == 7+5*width );

    delete [] data;

    const char* str = "Hello world!";
    StridedArray<char,1> b(const_cast<char*>(str), [](char*){}, 12);
    REQUIRE( b(0) == 'H' );
    REQUIRE( b(6) == 'w' );
  }

  SECTION( "slicing" ) {
    StridedArray<int,1> a1(127);
    for (unsigned int i = 0; i < size(a1); i++)
      a1(i) = i;
    auto a1s = a1[45];
    REQUIRE( a1s.ndim() == 0 );
    REQUIRE( size(a1s) == 1);
    REQUIRE( a1s() == 45 );

    StridedArray<int,2> a2(3,4);
    for (unsigned int i = 0; i < size(a2); i++)
      a2.data()[i] = i;
    auto a2s = a2[2];
    REQUIRE( a2s.ndim() == 1 );
    REQUIRE( size(a2s) == 4 );
    REQUIRE( a2s.dim(0) == 4 );
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

    StridedArray<int,0> a0;
    a0() = 123;
    a0.map(incr);
    REQUIRE( a0() == 124 );
    
    StridedArray<int,1> a1(127);
    for (unsigned int i = 0; i < size(a1); i++)
      a1(i) = i;
    a1.map(incr);
    REQUIRE( a1(43) == 44 );
    REQUIRE( a1(120) == 121 );

    StridedArray<int,3> a3(2,3,4);
    a3.map([](auto& path, auto& val) {
        val = path[0]*12 + path[1]*4 + path[2];
      });
    REQUIRE( a3(1,2,0) == 20 );

    a1.fill(42);
    REQUIRE( a1(34) == 42 );
    REQUIRE( a1(56) == 42 );
  }

  SECTION( "copies" ) {
    StridedArray<int,0> a0;
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
    StridedArray<int,3> a(2,3,4);
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
    StridedArray<int,3> a(2,3,4);
    REQUIRE( a.contiguous() );

    auto b = a[1];
    REQUIRE( b.contiguous() );
  }

  SECTION( "cumulative sum" ) {
    auto a = range(1,7);
    auto b = cumsum(a);

    REQUIRE( b.dims() == a.dims() );
    REQUIRE( b(0) == 1 );
    REQUIRE( b(1) == 3 );
    REQUIRE( b(2) == 6 );
    REQUIRE( b(3) == 10 );
    REQUIRE( b(4) == 15 );
    REQUIRE( b(5) == 21 );

    auto c = reshape(range(56), 7, 8);
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
    auto dims = std::array<std::size_t,2>{4, 5};
    auto strides = default_strides(dims);
    REQUIRE( strides.size() == 2 );
    REQUIRE( strides[0] == 5 );
    REQUIRE( strides[1] == 1 );
    
    auto dims2 = std::vector<std::size_t>{4, 5};
    REQUIRE( dims2.size() == 2 );
    auto strides2 = default_strides(dims2);
    REQUIRE( strides.size() == strides2.size() );
    REQUIRE( strides.size() == 2 );
    REQUIRE( strides[0] == strides2[0] );
    REQUIRE( strides[1] == strides2[1] );

    auto path = strided_index_to_coords(17, strides);
    REQUIRE( path.size() == 2 );
    REQUIRE( path[0] == 3 );
    REQUIRE( path[1] == 2 );
  }

  SECTION( "increment operator" ) {
    StridedArray<int,2> a = reshape(range(20), 4, 5);
    StridedArray<int,2> b = 3 * reshape(range(20), 4, 5);
    
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
    auto a = strided_array(reshape(range(24), 6,4));
    auto a0 = a[0];
    REQUIRE( a0(0) == 0 );
    REQUIRE( a0(1) == 1 );
    REQUIRE( a0(2) == 2 );
    
    auto b = strided_array(range(8,12));
    a[0] = b;

    REQUIRE( a0(1) == 9 );
    REQUIRE( a0(2) == 10 );
    REQUIRE( a0(3) == 11 );

    REQUIRE( a(0,0) == 8 );
    REQUIRE( a(0,1) == 9 );
    REQUIRE( a(0,2) == 10 );
  }
  
  SECTION( "immediate behavior" ) {
    auto a = strided_array(range(24));

    // Ensure that immediate always returns a copy
    auto b = strided_array(a);
    REQUIRE( a(2) == b(2) );
    b(2) = 42;
    REQUIRE( a(2) == 2 );
    REQUIRE( b(2) == 42 );
  }
}
