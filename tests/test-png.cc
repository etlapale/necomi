#include <cstdio>
#include <iostream>
using namespace std;

#include "catch.hpp"

#include <cuiloa/cuiloa.h>
using namespace cuiloa;

static const char* path = "test.png";

TEST_CASE( "PNG storage", "[codecs]" ) {
  SECTION( "load lena image" ) {
    // Open the image
    auto lena = png_load("share/bitmaps/lena.png");

    // Check dimensions
    auto dims = lena.dimensions();
    REQUIRE( dims[0] == 512 );
    REQUIRE( dims[1] == 512 );
    REQUIRE( dims[2] == 3 );

    // Check pixel values
    REQUIRE( lena(305,122,0) == 0x4d );
    REQUIRE( lena(305,122,1) == 0x42 );
    REQUIRE( lena(305,122,2) == 0x63 );

    REQUIRE( lena(354,300,0) == 0x9a );
    REQUIRE( lena(354,300,1) == 0x56 );
    REQUIRE( lena(354,300,2) == 0x4d );
  }
}
