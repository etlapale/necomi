#include <iostream>
#include "catch.hpp"

#include <cuiloa/base/concepts.h>
using namespace cuiloa;


// Arrays have some requirements
class NotArray1
{};

// Missing ndim
struct NotArray2
{
  using dtype = double;
};

// Minimal array definition
struct Array1
{
  using dtype = double;
  static constexpr ArrayDimension ndim = 0;
};

// Array definition
struct Array2
{
  using dtype = char;
  static constexpr ArrayDimension ndim = 0;
  double value;
};

// Missing element type
struct NotArray3
{
  static constexpr ArrayDimension ndim = 0;
};


TEST_CASE( "concepts", "[base]" ) {

  SECTION( "is_array" ) {
    std::array<double,NotArray3::ndim> a;
    REQUIRE( ! is_array<NotArray1>::value );
    REQUIRE( ! is_array<NotArray2>::value );

    REQUIRE( is_array<Array1>::value );
    REQUIRE( is_array<Array2>::value );
  }
}

// Local Variables:
// mode: c++
// End:
