#include "Catch/include/catch.hpp"

#include <necomi/necomi.h>
using namespace necomi;

TEST_CASE( "sort", "[algorithms]" ) {
  SECTION( "sort indices" ) {
    auto a = litarray<int>(74, 67, 27, 44,  2,  8, 61, 59, 25, 72);
    auto i = sort_indices(a);
    REQUIRE( i(0)[0] == 4 );
    REQUIRE( i(1)[0] == 5 );
    REQUIRE( i(2)[0] == 8 );
    REQUIRE( i(3)[0] == 2 );
    REQUIRE( i(4)[0] == 3 );

    std::sort(a.begin(), a.end());
    auto j = sort_indices(a);
    for (auto x = 0UL; x < a.dim(0); x++)
      REQUIRE( j(x)[0] == x );
  }

  SECTION( "permute" ) {
    auto a = litarray<int>(74, 67, 27, 44,  2,  8, 61, 59, 25, 72);
    auto idx = sort_indices(a);
    
    auto b = permute(a, idx);
    for (auto j = 1UL; j < b.dim(0); j++)
      REQUIRE( b(j-1) <= b(j) );

    std::reverse(idx.begin(), idx.end());
    auto c = permute(a, idx);
    for (auto j = 1UL; j < b.dim(0); j++)
      REQUIRE( c(j-1) >= c(j) );

    auto x = idx.slice(slice(0UL, size(a)-1UL, 1UL));
    bool length_error_thrown = false;
    try {
      permute(a, x);
    } catch (std::length_error&) {
      length_error_thrown = true;
    }
    REQUIRE( length_error_thrown );
  }
}
