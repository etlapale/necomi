#include <cmath>
#include <complex>
#include <cstdio>
#include <iostream>
using namespace std;
using namespace std::complex_literals;

#include "Catch/include/catch.hpp"

#include <necomi/necomi.h>
#include <necomi/numerics/fft.h>
using namespace necomi;

#include <necomi/codecs/streams.h>
using namespace necomi::streams;

#ifdef HAVE_FFTW

static const double epsilon = 1e-10;

SCENARIO( "contiguous arrays of complex numbers are Fourier transformable",
	  "[numerics]" ) {
  GIVEN( "a contiguous 1D array of complex numbers" ) {
    auto a = litarray(49.0 + 53i, 35.0 + 74i, 89.0 + 96i,
		      31.0 + 34i, 57.0 + 86i);
    WHEN( "its Fourier transform is computed" ) {
      auto b = fft(a);
      THEN( "its result is predetermined" ) {
	auto truth = litarray(261.+343i, 5.3775-15.8978i,
			      -54.3665+31.8220i, 77.6714-104.3631i,
			      -44.6825+10.4388i);
	REQUIRE( std::abs(sum(power<2>(b-truth))) < 1e-6 );
      }
      THEN( "its inverse is the original array" ) {
	auto c = ifft(b);
	REQUIRE( std::abs(sum(power<2>(c-a))) < 1e-12 );
      }
    }
  }
  GIVEN( "a contiguous 2D array of complex numbers" ) {
    auto a = strided_array(reshape<2>(litarray(38.00+81.00i, 99.00+62.00i, 30.00+76.00i, 84.00+22.00i, 64.00+42.00i, 33.00+5.00i, 69.00+88.00i, 57.00+19.00i, 83.00+46.00i, 96.00+59.00i, 7.00+99.00i, 25.00+84.00i, 62.00+25.00i, 55.00+16.00i, 57.00+88.00i, 89.00+2.00i, 52.00+35.00i, 78.00+54.00i, 35.00+44.00i, 19.00+12.00i, 74.00+27.00i, 20.00+51.00i, 17.00+92.00i, 33.00+81.00i, 95.00+48.00i, 51.00+91.00i, 47.00+2.00i, 63.00+97.00i, 89.00+87.00i, 81.00+38.00i, 31.00+45.00i, 27.00+73.00i, 61.00+59.00i, 10.00+81.00i, 26.00+18.00i, 61.00+89.00i, 8.00+83.00i, 72.00+43.00i, 63.00+36.00i, 9.00+56.00i, 76.00+88.00i, 13.00+63.00i, 63.00+27.00i, 15.00+21.00i, 95.00+97.00i, 44.00+76.00i, 31.00+22.00i, 47.00+66.00i, 33.00+30.00i, 8.00+60.00i, 63.00+74.00i, 14.00+99.00i, 28.00+68.00i, 67.00+55.00i, 83.00+91.00i, 98.00+28.00i, 14.00+18.00i, 31.00+62.00i, 17.00+2.00i, 70.00+8.00i, 10.00+66.00i, 94.00+35.00i, 27.00+69.00i, 82.00+42.00i, 17.00+24.00i, 65.00+6.00i, 59.00+62.00i, 19.00+50.00i, 16.00+80.00i, 48.00+45.00i, 59.00+13.00i, 86.00+97.00i, 52.00+58.00i, 84.00+58.00i, 50.00+79.00i, 97.00+79.00i, 51.00+91.00i), {11, 7}));
    WHEN( "its Fourier transform is computed" ) {
      auto b = fft(a);
      THEN( "its result is predetermined" ) {
	auto truth = strided_array(reshape<2>(litarray(3896.00+4165.00i, 92.35+-173.74i, -26.87+171.71i, -68.45+-280.62i, 50.04+212.49i, -120.11+-81.78i, 13.04+11.95i, 367.38+-201.72i, -55.44+-364.31i, 179.25+37.99i, -162.68+-99.32i, -92.32+-234.41i, 336.33+-32.95i, 278.79+207.11i, -20.86+176.52i, 130.01+-353.18i, -136.65+80.51i, 361.38+416.66i, 111.36+139.92i, 4.39+78.94i, -283.51+308.47i, -224.27+443.02i, -41.33+320.60i, 205.27+-341.72i, -437.75+-412.10i, -124.52+-110.46i, 214.26+360.64i, -102.24+-356.66i, -220.19+-25.78i, -585.79+117.41i, 9.77+299.46i, 77.10+-546.38i, 92.69+-124.90i, 11.14+-351.25i, -383.25+320.75i, 322.28+-130.80i, 297.27+116.65i, 221.62+146.27i, -205.86+518.53i, -351.75+211.49i, 223.53+286.12i, 71.00+372.16i, -332.50+-180.48i, -165.76+32.47i, 45.63+160.70i, -58.87+410.30i, -174.18+234.57i, 497.49+-83.99i, 226.32+132.44i, -81.50+-266.73i, 72.45+468.36i, -101.47+140.49i, -514.06+-204.91i, 521.66+22.32i, 195.59+-32.08i, -83.95+277.51i, 268.30+8.88i, 174.25+312.54i, -0.94+-214.28i, -517.80+148.23i, -225.91+36.40i, -272.24+-5.92i, -278.06+203.88i, 377.23+217.52i, 226.08+170.22i, -161.78+-590.68i, -14.02+-44.71i, 33.55+-76.32i, -56.04+-189.94i, 171.57+-59.23i, 235.12+-69.42i, 247.50+118.90i, -282.06+105.51i, -585.34+-485.86i, -68.60+-124.35i, -191.19+376.25i, -122.85+-9.88i), {11, 7}));
	REQUIRE( std::abs(sum(power<2>(b-truth))) < 1e-3 );
      }
      THEN( "its inverse is the original array" ) {
	auto c = ifft(b);
	REQUIRE( std::abs(sum(power<2>(c-a))) < 1e-12 );
      }
    }
  }
  GIVEN( "a contiguous 2D array of real numbers" ) {
    auto a = strided_array(reshape<2>(litarray(0.95, 0.99, 0.16, 0.55, 0.16, 0.21, 0.72, 0.66, 0.24, 0.24, 0.80, 0.82, 0.23, 0.09, 0.20, 0.02, 0.25, 0.80, 0.65, 0.40, 0.33, 0.86, 0.77, 0.88, 0.20, 0.56, 0.98, 0.69, 0.74, 0.58, 0.06, 0.21, 0.54, 0.06, 0.23), {7,5}));
    WHEN( "its (real) Fourier transform is computed" ) {
      auto b = rfft(a);
      THEN( "its result is predetermined" ) {
	REQUIRE( b.ndim == 2 );
	REQUIRE( b.dim(0) == 7 );
	REQUIRE( b.dim(1) == 3 );
	auto truth = reshape<2>(litarray(16.83+0.00i, -0.67-3.06i, -0.42-1.05i, -1.13+1.02i, 1.33-1.36i, 0.19-0.31i, 0.20-2.28i, -1.61+0.40i, -0.48-0.40i, 2.35-0.63i, 2.68+0.78i, 1.01-1.27i, 2.35+0.63i, 1.35-1.21i, -0.01-2.03i, 0.20+2.28i, -0.07+0.12i, 1.38-0.98i, -1.13-1.02i, 2.10+0.40i, 0.01+0.03i), {7,3});
	REQUIRE( std::abs(sum(power<2>(truth-b))) < 1e-4 );
      }
      THEN( "its inverse is the original array" ) {
	auto c = irfft(b, a.dim(1));
	REQUIRE( a.ndim == c.ndim );
	REQUIRE( a.dims() == c.dims() );
	REQUIRE( std::abs(sum(power<2>(c-a))) < 1e-12 );
      }
    }
  }
}

#endif // HAVE_FFTW

