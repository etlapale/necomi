#include <cstdio>
#include <iostream>
using namespace std;

#include "Catch/include/catch.hpp"

#include <necomi/necomi.h>
#include <necomi/filters.h>
using namespace necomi;


#ifdef HAVE_BOOST

SCENARIO( "exponential filters are recursive filters", "[filters]" ) {
  GIVEN( "an exponential cascade filter" ) {
    auto n = 8UL;
    auto tau = 85.0;
    auto filter = exp_cascade(n, tau);

    WHEN( "an exponential filter of order N is constructed" ) {
      THEN( "its input memory must be N+2" ) {
	REQUIRE( filter.a().size() == n + 2 );
      }
      THEN( "its output memory must be 1" ) {
	REQUIRE( filter.b().size() == 1 );
      }
    }

    WHEN( "an exponential filter of order 8 and bandwidth 85.0 is constructed") {
      THEN( "its input coefficients are fixed" ) {
	std::vector<double> coefs = {1.0,
				     -8.1915810880534128,
				     29.823111432068593,
				     -63.336631450255496,
				     86.471192061486988,
				     -78.703975728034578,
				     47.756296231732065,
				     -18.628551097492043,
				     4.238813524112568,
				     -0.42867388518401917};
	REQUIRE( coefs.size() == filter.a().size() );
	
	double err = 0;
	for (auto i = 0UL; i < coefs.size(); i++)
	  err += std::abs(coefs[i] - filter.a()[i]);

	REQUIRE( err < 1e-5 );
      }
    }
  }
}

#endif // HAVE_BOOST
