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
  
  SECTION( "nearest-neighbor interpolation" ) {
    auto a = linspace<double>(0, 10, 11);
    auto i = interpolation<InterpolationMethod::NearestNeighbor>(a);
    REQUIRE( i(0) == 0 );
    REQUIRE( i(1) == 1 );
    REQUIRE( i(10) == 10 );
    REQUIRE( i(0.3) == 0 );
    REQUIRE( i(0.8) == 1 );
    REQUIRE( i(6.2) == 6 );
    REQUIRE( i(6.9) == 7 );
  }
  
  SECTION( "linear interpolation" ) {
    auto a = linspace<double>(0, 10, 11);
    auto i = interpolation<InterpolationMethod::Linear>(a);
    REQUIRE( i(0) == 0 );
    REQUIRE( i(1) == 1 );
    REQUIRE( i(10) == 10 );
    REQUIRE( i(0.3) == 0.3 );
    REQUIRE( i(0.8) == 0.8 );
    REQUIRE( i(6.2) == 6.2 );
    REQUIRE( i(6.9) == 6.9 );
  }
  
  SECTION( "rescaling" ) {
    auto r1 = rescale<double>(0, 1, 0, 100);
    REQUIRE( r1(0) == 0 );
    REQUIRE( r1(0.25) == 25 );
    REQUIRE( r1(0.5) == 50 );
    REQUIRE( r1(1) == 100 );

    auto r2 = rescale<double>(1, 3, 10, 20);
    REQUIRE( r2(1) == 10 );
    REQUIRE( r2(1.5) == 12.5 );
    REQUIRE( r2(2) == 15 );
    REQUIRE( r2(3) == 20 );
  }
}
