#include "catch.hpp"

#include <cuiloa/cuiloa.h>
using namespace cuiloa;
using namespace cuiloa::delayed;

static constexpr double float_tol = 1e-3;

TEST_CASE( "basic numerics", "[base]" ) {
  SECTION( "discretizations" ) {
    auto d1 = discretization<double>(0, 1, 10);
    REQUIRE( d1(0) == 0 );
    REQUIRE( d1(1) == 1 );

    auto d2 = discretization<double>(0, 180, 100, [](auto&& a){ return cos(radians(a)); });
    REQUIRE( fabs(d2(0) - 1) < float_tol );
    REQUIRE( fabs(d2(90) - 0) < 2e-2 );
    REQUIRE( fabs(d2(180) + 1) < float_tol );
  }
}
