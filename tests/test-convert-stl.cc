#include "Catch/include/catch.hpp"

#include <necomi/convert/stl.h>
using namespace necomi;


SCENARIO( "std::vector are convertible to strided arrays", "[convert]" ) {
  GIVEN( "a std::vector" ) {
    std::vector<int> vec { 80, 57, 17, 45,  7, 35, 32, 73, 38, 85, 75, 25 };

    WHEN( "the vector is converted to a strided array" ) {
      
      auto arr = from_vector(vec);      
      THEN( "they should have the same size" ) {
	REQUIRE( size(arr) == vec.size () );
      }
      THEN( "they should have the same element values" ) {
	for (auto i = 0UL; i < vec.size(); i++)
	  REQUIRE( vec[i] == arr(i) );
      }
      THEN( "they should be mapped to different memory regions" ) {
	REQUIRE( vec[3] == 45 );
	REQUIRE( arr(3) == 45 );
	
	vec[3] = 72;
	
	REQUIRE( vec[3] == 72 );
	REQUIRE( arr(3) == 45 );
      }
    }
  }
}
