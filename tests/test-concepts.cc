#include "catch.hpp"

#include <cuiloa/base/concepts.h>
using namespace cuiloa;


TEST_CASE( "concepts", "[base]" ) {
  SECTION( "is_array" ) {
    // Arrays have some requirements
    class NotArray1
    {};
    REQUIRE( ! is_array<NotArray1>::value );

    // Minimal array definition
    struct Array1
    {
      using dtype = double;
      typedef int ndim;
    };
    REQUIRE( is_array<Array1>::value );
  }
}

// Local Variables:
// mode: c++
// End:
