#include <iostream>
#include "catch.hpp"

#include "../necomi/traits/arrays.h"
using namespace necomi;


// Arrays have some requirements
class NotArray1
{};

// Missing ndim
struct NotArray2
{
  using dtype = double;
};

// Missing element type
struct NotArray3
{
  static constexpr std::size_t ndim = 0;
};

// Missing dimensions
struct NotArray4
{
  typedef char value_type;
  static constexpr std::size_t ndim = 1;
};

// Minimal array definition
struct Array1
{
  using dtype = double;
  enum { ndim = 1 };
  using dim_type = std::size_t;
  using dims_type = std::array<dim_type,ndim>;
  
  dims_type dims() const
  { return std::array<std::size_t,ndim>(); };
};

// Minimal indexable array definition
struct IndexableArray1 : Array1
{
  dtype operator()(const dims_type&) const
  { return 42; };
};

TEST_CASE( "concepts", "[base]" ) {

  SECTION( "is_array" ) {
    REQUIRE( ! is_array<NotArray1>::value );
    REQUIRE( ! is_array<NotArray2>::value );
    REQUIRE( ! is_array<NotArray3>::value );
    REQUIRE( ! is_array<NotArray4>::value );

    REQUIRE( is_array<Array1>::value );
  }
  
  SECTION( "is_indexable" ) {
    REQUIRE( ! is_indexable<NotArray1>::value );
    REQUIRE( ! is_indexable<NotArray2>::value );
    REQUIRE( ! is_indexable<NotArray3>::value );
    REQUIRE( ! is_indexable<NotArray4>::value );

    REQUIRE( ! is_indexable<Array1>::value );

    REQUIRE( is_indexable<IndexableArray1>::value );
  }
}

// Local Variables:
// mode: c++
// End:
