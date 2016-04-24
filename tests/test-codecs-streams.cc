#include <cstdio>
#include <iostream>
#include <sstream>
using namespace std;

#include "Catch/include/catch.hpp"

#include <necomi/necomi.h>
using namespace necomi;
using namespace necomi::streams;


TEST_CASE( "Text streams", "[codecs]" ) {
  SECTION( "print a std::array" ) {

    // Create a array
    std::array<std::size_t,4> a {3, 7, 2, 5};

    // Convert the dimensions to string
    std::ostringstream ss;
    ss << a;
    auto str = ss.str();

    // Check the format
    REQUIRE( str == "[3, 7, 2, 5]" );
  }
  
  SECTION( "print a std::array" ) {
    
    // Create a 2D array
    auto a = reshape(range(12), 3, 4);
    
    // Convert the dimensions to string
    std::ostringstream ss;
    ss << a;
    auto str = ss.str();

    // Check the format
    REQUIRE( str == "[[0, 1, 2, 3], [4, 5, 6, 7], [8, 9, 10, 11]]" );
  }
  
  SECTION( "copy dimensions on text streams" ) {

    // Create a 2D array
    auto a = reshape(range(15), 3, 5);

    // Convert the dimensions to string
    std::ostringstream ss;
    necomi::copy_dims(a.dims(), ss);
    auto str = ss.str();

    // Check the format
    REQUIRE( str == "3×5" );
  }
}
