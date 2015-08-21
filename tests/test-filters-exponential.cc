#include <cmath>
#include <cstdio>
#include <iostream>
using namespace std;

#include "Catch/include/catch.hpp"

#include <necomi/necomi.h>
#include <necomi/codecs/streams.h>
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

    WHEN( "the filter is fed a scalar input" ) {
      THEN( "it just works" ) {
	(void) filter.feed(3);
	REQUIRE( true );
      }
    }
    
    WHEN( "the filter is fed a valid (0D) array" ) {
      THEN( "it just works" ) {
	auto input = zeros();
	(void) filter.feed(input);
	REQUIRE( true );
      }
    }
  }
}

SCENARIO( "recursive filters input should be well dimensioned", "[filters]" ) {
  GIVEN( "a 1D exponential cascade filter" ) {
    auto n = 8UL;
    auto tau = 85.0;
    auto filter = exp_cascade<double,1>(n, tau, {1});

    WHEN( "the filter is fed a valid 1D input" ) {
      auto input = zeros(1);
      THEN( "it just works" ) {
	(void) filter.feed(input);
	REQUIRE( true );
      }
    }
    
    WHEN( "the filter is fed a mismatched 1D input") {
	auto input = zeros(23);
	bool exception_thrown = false;
	try {
	  (void) filter.feed(input);
	} catch (std::length_error&) {
	  exception_thrown = true;
	}
	THEN( "a std::length_error exception is thrown" ) {
	  REQUIRE( exception_thrown );
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

SCENARIO( "exponential filter impulse response is an approximation of a Gamma function", "[filters]" ) {
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
	}
	REQUIRE( err < 0.15 );
      }
    }
  }
}


SCENARIO( "exponential filter response is predetermined", "[filters]" ) {
  GIVEN( "a 1D exponential cascade filter" ) {
    auto n = 8UL;
    auto tau = 85.0;
    auto filter = exp_cascade<double,1>(n, tau, {3});

    WHEN( "the filter is fed a series of fixed 1D input" ) {
      filter.feed(litarray(3.737023 ,  2.035292 ,  4.4884669));
      filter.feed(litarray(2.8989778,  7.6498662,  5.6221151));
      filter.feed(litarray(5.6696888,  5.4746131,  3.9854514));
      for (auto i = 0; i < 50; i++)
	filter.feed(zeros(3));
      THEN( "the filter output shoud be predetermined" ) {
	auto out = filter.feed(zeros(3));
	auto expected = litarray(0.0890974, 0.109397, 0.103007);
	auto err = sum(power<2>(out - expected));
	REQUIRE( err < 1e-12 );
      }
    }
  }
}

#endif // HAVE_BOOST
