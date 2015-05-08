#include "catch.hpp"

#include <necomi/necomi.h>
using namespace necomi;


TEST_CASE( "dynamic arrays", "[core]" ) {

  SECTION( "creation" ) {
    DynArray<double> a(3, 7, 12, 5);
    std::vector<std::size_t> v{3,7,12,5};
    REQUIRE( a.dims() == v );
    REQUIRE( size(a) == 1260 );
  }
}
