#include <cstdio>
#include <iostream>
using namespace std;

#include "Catch/include/catch.hpp"

#include <necomi/necomi.h>
using namespace necomi;

static const std::string baboon_path = NECOMI_SOURCE_DIR "/share/bitmaps/baboon.png";


TEST_CASE( "PNG storage", "[codecs]" ) {
  SECTION( "load baboon PNG image" ) {
    // Open the image
    auto img = png_load(baboon_path);

    // Check dimensions
    REQUIRE( img.dim(0) == 512 );
    REQUIRE( img.dim(1) == 512 );
    REQUIRE( img.dim(2) == 3 );

    // Check pixel values
    REQUIRE( img(305,122,0) == 0x2e );
    REQUIRE( img(305,122,1) == 0x3f );
    REQUIRE( img(305,122,2) == 0x4b );

    REQUIRE( img(354,300,0) == 0xfa );
    REQUIRE( img(354,300,1) == 0x44 );
    REQUIRE( img(354,300,2) == 0x26 );
  }

  SECTION( "load an invalid image" ) {
    // Create an invalid image
    std::ofstream fp("invalid.png", std::ios::binary);
    fp.write("abcdefgh", 8);
    fp.close();
    
    bool png_exception_thrown = false;
    try {
      png_load("invalid.png");
    } catch (necomi::png_exception&) {
      png_exception_thrown = true;
    }

    REQUIRE( png_exception_thrown );
  }

  SECTION( "save PNG image" ) {
    // Open the image
    auto img = png_load(baboon_path);

    // Rotate the image
    auto new_img = make_delayed(img.dims(), [&img](const auto& path) {
	return img(path[1], path[0], path[2]);
      });
    // Save a copy
    png_save(new_img, "rotated-baboon.png");
  }
}
