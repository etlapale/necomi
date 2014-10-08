#include <cstdio>

#include "catch.hpp"

#include <cuiloa/cuiloa.h>
using namespace cuiloa;


TEST_CASE( "random number generation", "[random]" ) {
  SECTION( "Mersenne-Twister 64 bits" ) {
    // Setup a random number generator
    RandomDevSeedSequence rdss;
    std::mt19937_64 prng(rdss);

    // Create a array filled from a normal distribution
    ArrayIndex size = 1e6;
    double avg = 95, dev = 4.3;
    auto a = normal_distribution<double>(avg, dev, size, prng);

    // Check the statistics
    REQUIRE( fabs(sum(a)/size - avg) < 1e-1 );
    double std = 0;
    a.map([&std,avg](auto& path, auto& val) {
	(void) path;
	std += (val-avg)*(val-avg);
      });
    std = sqrt(std/size);
    REQUIRE( fabs(std-dev) < 2e-2 );
  }
}
 
