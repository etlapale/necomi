#include "Catch/include/catch.hpp"

#include <necomi/necomi.h>
using namespace necomi;


TEST_CASE( "dynamic arrays", "[core]" ) {

  SECTION( "creation" ) {
    VarArray<double> a(3, 7, 12, 5);
    std::vector<std::size_t> v{3,7,12,5};
    REQUIRE( a.dims() == v );
    REQUIRE( size(a) == 1260 );

    VarArray<double> b(3,4);
    std::vector<std::size_t> w{3,4};
    REQUIRE( b.dims() == w );
    REQUIRE( size(b) == 12 );
  }

  SECTION( "data access" ) {
    VarArray<int> a0;
    a0() = 123;
    REQUIRE( a0() == 123 );
    REQUIRE( a0.data()[0] == 123 );

#ifndef NECOMI_NO_BOUND_CHECKS
    bool exception_thrown = false;
    try {
      a0(1);
    } catch (std::length_error&) {
      exception_thrown = true;
    }
    REQUIRE( exception_thrown );
#endif // NECOMI_NO_BOUND_CHECKS
    
    VarArray<int> a1(127);
    a1(32) = 123;
    a1(69) = 456;
    REQUIRE( a1(32) == 123 );
    REQUIRE( a1(69) == 456 );
    REQUIRE( a1.data()[32] == 123 );
    REQUIRE( a1.data()[69] == 456 );

    VarArray<double> a2(3,4);
    for (unsigned int i = 0; i < size(a2); i++)
      a2.data()[i] = i;
    REQUIRE( a2(1,2) == 6 );
    REQUIRE( a2(2,3) == strided_index(a2, 2, 3) );

#ifndef NECOMI_NO_BOUND_CHECKS
    exception_thrown = false;
    try {
      a2(1);
    } catch (std::length_error&) {
      exception_thrown = true;
    }
    REQUIRE( exception_thrown );

    exception_thrown = false;
    try {
      a2(1,2,3);
    } catch (std::length_error&) {
      exception_thrown = true;
    }
    REQUIRE( exception_thrown );
#endif // NECOMI_NO_BOUND_CHECKS
  }

  SECTION( "convert from and to VarArray" ) {
    VarArray<int> a0;
    a0() = 123;

    VarArray<int> b0;
    b0() = 456;

    //StridedArray<int,0> s0 = a0;
    //REQUIRE( s0() == 123 );
    //s0 = b0;
    //REQUIRE( s0() == 456 );

    /*
    auto a = reshape(range(20), 4, 5);
    auto b = VarArray<int>(a);
    REQUIRE( b(1,2) == 7 );
    REQUIRE( b(3,1) == 16 );
    auto c = var_array(a);
    REQUIRE( c(2,4) == 14 );
    REQUIRE( c(3,1) == 16 );
    */
  }
}
