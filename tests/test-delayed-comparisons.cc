#include "Catch/include/catch.hpp"

#include <necomi/necomi.h>

using namespace necomi;

TEST_CASE( "delayed arrays from comparison operations", "[delayed]" ) {
 
  SECTION( "comparison operators on mismatching sizes" ) {
    auto a = range<int>(10);
    auto b = range<int>(8);

    bool length_error_thrown = false;
    try {
      a == b;
    } catch (std::length_error&) {
      length_error_thrown = true;
    }
    REQUIRE( length_error_thrown );

    length_error_thrown = false;
    try {
      a != b;
    } catch (std::length_error&) {
      length_error_thrown = true;
    }
    REQUIRE( length_error_thrown );

    length_error_thrown = false;
    try {
      a > b;
    } catch (std::length_error&) {
      length_error_thrown = true;
    }
    REQUIRE( length_error_thrown );

    length_error_thrown = false;
    try {
      a < b;
    } catch (std::length_error&) {
      length_error_thrown = true;
    }
    REQUIRE( length_error_thrown );
  }
}
