#include "Catch/include/catch.hpp"

#include <necomi/necomi.h>

using namespace necomi;

TEST_CASE( "delayed arrays from arithmetic operations", "[delayed]" ) {
  SECTION( "unary minus operator" ) {
    auto a = range<int>(10);
    auto b = -a;
    REQUIRE( a(0) == -b(0) );
    REQUIRE( a(3) == -b(3) );
    REQUIRE( a(4) == -b(4) );
    REQUIRE( a(7) == -b(7) );
    
    auto c = -b;
    REQUIRE( all(c == a) );
  }
  
  SECTION( "arithmetic operators on mismatching sizes" ) {
    auto a = range<int>(10);
    auto b = range<int>(8);

    bool length_error_thrown = false;
    try {
      a * b;
    } catch (std::length_error&) {
      length_error_thrown = true;
    }
    REQUIRE( length_error_thrown );
    
    length_error_thrown = false;
    try {
      a / b;
    } catch (std::length_error&) {
      length_error_thrown = true;
    }
    REQUIRE( length_error_thrown );
    
    length_error_thrown = false;
    try {
      a - b;
    } catch (std::length_error&) {
      length_error_thrown = true;
    }
    REQUIRE( length_error_thrown );
    
    length_error_thrown = false;
    try {
      a + b;
    } catch (std::length_error&) {
      length_error_thrown = true;
    }
    REQUIRE( length_error_thrown );
  }
}
