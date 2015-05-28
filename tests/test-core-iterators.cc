#include "Catch/include/catch.hpp"

#include <necomi/necomi.h>
using namespace necomi;

TEST_CASE( "iterators", "[core]" ) {
  SECTION( "basic iterator" ) {
    auto a = strided_array(range<int>(128));

    int i = 0;
    int s = 0;
    for (auto val : a) {
      REQUIRE( val == i );
      s += val;
      i++;
    }
    REQUIRE( sum(a) == s );
  }
}
