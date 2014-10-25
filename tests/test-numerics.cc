#include "catch.hpp"

#include <cuiloa/cuiloa.h>
using namespace cuiloa;


TEST_CASE( "basic numerics", "[base]" ) {
  SECTION( "discretizations" ) {
    auto d1 = discretization<double>(0, 1, 10);
    REQUIRE( d1(0) == 0 );
    REQUIRE( d1(1) == 1 );
  }
}
