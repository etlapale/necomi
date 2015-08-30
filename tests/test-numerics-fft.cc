#include <cmath>
#include <complex>
#include <cstdio>
#include <iostream>
using namespace std;
using namespace std::complex_literals;

#include "Catch/include/catch.hpp"

#include <necomi/necomi.h>
#include <necomi/codecs/streams.h>
#include <necomi/numerics/fft.h>
using namespace necomi;

#ifdef HAVE_FFTW

static const double epsilon = 1e-10;

SCENARIO( "simple 1D arrays of complex numbers are Fourier transformable",
	  "[numerics]" ) {
  GIVEN( "a simple 1D array of complex numbers" ) {
    auto a = litarray(49.0 + 53i, 35.0 + 74i, 89.0 + 96i,
		      31.0 + 34i, 57.0 + 86i);
    WHEN( "its Fourier transform is computed" ) {
      auto b = dft(a);
      THEN( "its result is predetermined" ) {
	auto truth = litarray(261.+343i, 5.3775-15.8978i,
			      -54.3665+31.8220i, 77.6714-104.3631i,
			      -44.6825+10.4388i);
	REQUIRE( std::abs(sum(power<2>(b-truth))) < 1e-6 );
      }
    }
  }
}

#endif // HAVE_FFTW

