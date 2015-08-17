#include <cstdio>
#include <iostream>
using namespace std;

#include "Catch/include/catch.hpp"

#include <necomi/necomi.h>
using namespace necomi;


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
  }
}
