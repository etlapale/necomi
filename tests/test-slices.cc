#include "catch.hpp"

#include <cuiloa/cuiloa.h>
using namespace cuiloa;


TEST_CASE( "slices", "[base]" ) {
  SECTION( "creating 1D slices" ) {
    Slice<1> s1a(1, 3);
    REQUIRE( s1a.start().size() == 1 );
    REQUIRE( s1a.start()[0] == 1 );
    REQUIRE( s1a.size()[0] == 3 );
    REQUIRE( s1a.strides()[0] == 1 );

    Slice<1> s1c(4, 8, 2);
    REQUIRE( s1c.start()[0] == 4 );
    REQUIRE( s1c.size()[0] == 8 );
    REQUIRE( s1c.strides()[0] == 2 );
  }

  SECTION( "creating 2D slices" ) {
    Slice<2> s2 = (Slice<1>(1,3) , Slice<1>(1,2));

    REQUIRE( s2.start().size() == 2 );
    REQUIRE( s2.start()[0] == 1 );
    REQUIRE( s2.start()[1] == 1 );
    REQUIRE( s2.size()[0] == 3 );
    REQUIRE( s2.size()[1] == 2 );
    REQUIRE( s2.strides()[0] == 1 );
    REQUIRE( s2.strides()[1] == 1 );
  }

  SECTION( "slice factory" ) {
    auto s2 = (slice(1,3),slice(1,2));

    REQUIRE( s2.start().size() == 2 );
    REQUIRE( s2.start()[0] == 1 );
    REQUIRE( s2.start()[1] == 1 );
    REQUIRE( s2.size()[0] == 3 );
    REQUIRE( s2.size()[1] == 2 );
    REQUIRE( s2.strides()[0] == 1 );
    REQUIRE( s2.strides()[1] == 1 );
  }

  SECTION( "slice from initializer lists" ) {
    Slice<2> s2({{{{1,3}}, {{1,2}}}});

    REQUIRE( s2.start()[0] == 1 );
    REQUIRE( s2.start()[1] == 1 );
    REQUIRE( s2.size()[0] == 3 );
    REQUIRE( s2.size()[1] == 2 );
    REQUIRE( s2.strides()[0] == 1 );
    REQUIRE( s2.strides()[1] == 1 );
  }

  SECTION( "slicing an immediate array" ) {
    Array<int,2> a(4, 5);
    a.map([](auto& path, auto& val) { val = path[0]*5 + path[1]; });
    REQUIRE( a(0,0) == 0 );
    REQUIRE( a(1,1) == 6 );
    REQUIRE( a(3,2) == 17 );

    auto b = a.slice((slice(1,3),slice(1,2)));

    REQUIRE( b.dimensions().size() == 2 );
    REQUIRE( b.dimensions()[0] == 3 );
    REQUIRE( b.dimensions()[1] == 2 );

    REQUIRE( b.strides()[0] == a.strides()[0]);
    REQUIRE( b.strides()[1] == 1);

    REQUIRE( b(0,0) == a(1,1) );
    REQUIRE( b(0,1) == a(1,2) );
    REQUIRE( b(1,0) == a(2,1) );
    REQUIRE( b(1,1) == a(2,2) );
    REQUIRE( b(2,0) == a(3,1) );
    REQUIRE( b(2,1) == a(3,2) );
  }

  SECTION( "double slicing" ) {
    Array<int,2> a(4, 5);
    a.map([](auto& path, auto& val) { val = path[0]*5 + path[1]; });
    auto b = a.slice((slice(1,3),slice(1,4)));
    auto c = b.slice((slice(0,3),slice(2,2)));

    REQUIRE( c.dimensions().size() == 2 );
    REQUIRE( c.dimensions()[0] == 3 );
    REQUIRE( c.dimensions()[1] == 2 );

    REQUIRE( c.strides()[0] == 5 );
    REQUIRE( c.strides()[1] == 1 );

    REQUIRE( c(0,0) == 8 );
    REQUIRE( c(0,1) == 9 );
    REQUIRE( c(1,0) == 13 );
    REQUIRE( c(1,1) == 14 );
    REQUIRE( c(2,0) == 18 );
    REQUIRE( c(2,1) == 19 );
  }

  SECTION( "slicing with stride" ) {
    using namespace cuiloa::delayed;
    
    Array<int,1> a(100);
    a.map([](auto& path, auto& val) { val = path[0]; });

    auto evens = a.slice(slice(0,49,2));
    Array<int,1> b(49);
    b.map([](auto& path, auto& val) { val = 2*path[0]; });
    REQUIRE( all(evens == b) );
    REQUIRE( ! any(evens != b) );
  }
}
