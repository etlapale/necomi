#include "Catch/include/catch.hpp"

#include <necomi/filters/deriche.h>
using namespace necomi;


static const double epsilon = 1e-10;

SCENARIO( "Deriche filters approximation", "[filters]" ) {
  GIVEN( "a 1D discretized impulse signal" ) {

    constexpr std::size_t N = 1001;
    auto a = strided_array(zeros(N));
    a[N/2] = 1.0;
    
    WHEN( "blurred with a Deriche filter" ) {

      constexpr double sigma = 2.0;
      deriche(a, sigma);
      
      THEN( "" ) {
	//REQUIRE( filter.a().size() == n + 2 );
      }
    }
  }
}
