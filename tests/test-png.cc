#include <cstdio>
#include <iostream>
using namespace std;

#include "catch.hpp"

#include <cuiloa/cuiloa.h>
using namespace cuiloa;

static const std::string lena_path = "share/bitmaps/lena.png";


TEST_CASE( "PNG storage", "[codecs]" ) {
  SECTION( "load lena image" ) {
    // Open the image
    auto lena = png_load(lena_path);

    // Check dimensions
    auto& dims = lena.dimensions();
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

  SECTION( "save PNG image" ) {
    // Open the image
    auto lena = png_load(lena_path);

    // Rotate the image
    auto new_lena =
      make_delayed<unsigned char>(lena.dimensions(), [&lena](auto& path) {
	  return lena(path[1], path[0], path[2]);
	});
    // Save a copy
    png_save(new_lena, "rotated-lena.png");
  }
}
