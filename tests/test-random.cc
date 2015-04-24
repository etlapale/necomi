#include <cstdio>

#include "catch.hpp"

#include <necomi/necomi.h>
using namespace necomi;


TEST_CASE( "random number generation", "[random]" ) {
  SECTION( "Mersenne-Twister 64 bits" ) {
    // Setup a random number generator
    RandomDevSeedSequence rdss;
    std::mt19937_64 prng(rdss);

    // Create a array filled from a normal distribution
    ArrayIndex size = 1e6;
    double avg = 95, dev = 4.3;
    auto a = normal<double>(avg, dev, size, prng);

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

  SECTION( "PRNG types" ) {
    
    RandomDevSeedSequence rdss;
    std::mt19937_64 prng(rdss);
    
    auto a = normal<float>(42, prng);
    bool ok = std::is_same<decltype(a)::dtype, float>::value; 
    REQUIRE( ok );
    
    auto b = normal<double,3>({5,3,7}, prng);
    ok = std::is_same<decltype(b)::dtype, double>::value;
    REQUIRE( ok );

    Dimensions<3> d = {5,3,7};
    auto c = normal(d, prng);
    ok = std::is_same<decltype(c)::dtype, double>::value;
    REQUIRE( ok );
    REQUIRE( c.ndim == 3 );
  }
}
 
