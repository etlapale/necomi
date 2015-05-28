#include "Catch/include/catch.hpp"

#include <algorithm>

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

  SECTION( "random access iterator" ) {
    auto a = reshape(range(20), 4, 5);
    
    auto i = ArrayIterator<decltype(a)>(a, {2, 3});
    auto j = ArrayIterator<decltype(a)>(a, {1,1});

    auto m = j - i;
    auto n = i - j;
    REQUIRE( m == -n );
    REQUIRE( n == 7 );
  }

  SECTION( "bidirectional iterator" ) {
    auto a = strided_array(reshape(range(6), 3, 2));
    auto it = a.end();
    --it;
    REQUIRE( *it == 5 );
    --it; --it;
    REQUIRE( *it == 3 );
  }

  SECTION( "STL sorts" ) {
    auto a = litarray<int>(74, 67, 27, 44,  2,  8, 61, 59, 25, 72);
    std::sort(a.begin(), a.end());
    auto b = litarray<int>(2,  8, 25, 27, 44, 59, 61, 67, 72, 74);
    REQUIRE( all(a == b) );
    REQUIRE( a(1) == 8 );
  }
}
