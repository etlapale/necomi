#include <cmath>
#include <cstdio>
#include <iostream>
using namespace std;

#include "Catch/include/catch.hpp"

#include <necomi/necomi.h>
#include <necomi/filters.h>
using namespace necomi;


#ifdef HAVE_BOOST

static const double epsilon = 1e-10;

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
      
      THEN( "its input coefficients are fixed" ) {
	REQUIRE( coefs.size() == filter.a().size() );
	
	double err = 0;
	for (auto i = 0UL; i < coefs.size(); i++)
	  err += std::abs(coefs[i] - filter.a()[i]);

	REQUIRE( err < epsilon );
      }

      THEN( "its output coefficient is fixed" ) {
	auto sum = std::accumulate(coefs.begin(), coefs.end(), 0.0);
	REQUIRE( std::abs(sum - 3.8066805263525794e-10) < epsilon );
	REQUIRE( std::abs(sum - filter.b()[0]) < epsilon );
      }
    }
  }
}


static constexpr uint64_t factorial(uint64_t n)
{ 
  return n == 0 ? 1 : n * factorial(n-1); 
}

double gamma(double t, std::size_t n, double tau)
{ 
  return std::pow(n*t, n)
    * std::exp(-static_cast<double>(n)*t/tau)
    / (factorial(n-1)*std::pow(tau, (n+1)));
}

SCENARIO( "exponential filter impulse response", "[filters]" ) {
  GIVEN( "an exponential cascade filter" ) {
    auto n = 8UL;
    auto tau = 85.0;
    auto filter = exp_cascade(n, tau);

    WHEN( "an impulse response is computed" ) {
      
      THEN( "its response should be approximated by a Gamma function" ) {
	auto err = 0.0;
	for (auto t = 0; t < 300; t++) {
	  auto res = filter.feed(t == 0 ? 1 : 0);
	  err += std::abs(res - gamma(t, n, tau));
	  std::cout << res << std::endl;
	}
	REQUIRE( err < 0.15 );
      }
    }
  }
}

#endif // HAVE_BOOST
