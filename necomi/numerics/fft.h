// necomi/numerics/fft.h – Wrapper around FFTW
//
// Copyright © 2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#ifdef HAVE_FFTW

#include <fftw3.h>

#include <complex>
#include <type_traits>

#include "../arrays/stridedarray.h"


namespace necomi {

/** Discrete Fourier transform of a 1D array of complex numbers. */
template <std::size_t N>
StridedArray<std::complex<double>,N> fft(StridedArray<std::complex<double>,N>& a)
{
  static_assert( N >= 1,
		 "scalar arrays are not Fourier transformable" );
  
  StridedArray<std::complex<double>,N> res(a.dims());
  fftw_plan p;

  if (N == 1)
    p = fftw_plan_dft_1d(a.dim(0),
			 reinterpret_cast<fftw_complex*>(a.data()),
			 reinterpret_cast<fftw_complex*>(res.data()),
			 FFTW_FORWARD, FFTW_ESTIMATE);
  else if (N == 2)
    p = fftw_plan_dft_2d(a.dim(0), a.dim(1),
			 reinterpret_cast<fftw_complex*>(a.data()),
			 reinterpret_cast<fftw_complex*>(res.data()),
			 FFTW_FORWARD, FFTW_ESTIMATE);
  else if (N == 3)
    p = fftw_plan_dft_3d(a.dim(0), a.dim(1), a.dim(2),
			 reinterpret_cast<fftw_complex*>(a.data()),
			 reinterpret_cast<fftw_complex*>(res.data()),
			 FFTW_FORWARD, FFTW_ESTIMATE);
  else {
    int dims[N];
    for (int i = 0UL; i < a.ndim; i++)
      dims[i] = a.dim(i);
    p = fftw_plan_dft(N, dims,
		      reinterpret_cast<fftw_complex*>(a.data()),
		      reinterpret_cast<fftw_complex*>(res.data()),
		      FFTW_FORWARD, FFTW_ESTIMATE);
  }

  fftw_execute(p);
  fftw_destroy_plan(p);
  
  return res;
}

/** Inverse Fourier transform of a contiguous array of complex numbers. */
template <std::size_t N>
StridedArray<std::complex<double>,N> ifft(StridedArray<std::complex<double>,N>& a)
{
  static_assert( N >= 1,
		 "scalar arrays are not Fourier transformable" );
  
  StridedArray<std::complex<double>,N> res(a.dims());
  fftw_plan p;

  if (N == 1)
    p = fftw_plan_dft_1d(a.dim(0),
			 reinterpret_cast<fftw_complex*>(a.data()),
			 reinterpret_cast<fftw_complex*>(res.data()),
			 FFTW_BACKWARD, FFTW_ESTIMATE);
  else if (N == 2)
    p = fftw_plan_dft_2d(a.dim(0), a.dim(1),
			 reinterpret_cast<fftw_complex*>(a.data()),
			 reinterpret_cast<fftw_complex*>(res.data()),
			 FFTW_BACKWARD, FFTW_ESTIMATE);
  else if (N == 3)
    p = fftw_plan_dft_3d(a.dim(0), a.dim(1), a.dim(2),
			 reinterpret_cast<fftw_complex*>(a.data()),
			 reinterpret_cast<fftw_complex*>(res.data()),
			 FFTW_BACKWARD, FFTW_ESTIMATE);
  else {
    int dims[N];
    for (int i = 0UL; i < a.ndim; i++)
      dims[i] = a.dim(i);
    p = fftw_plan_dft(N, dims,
		      reinterpret_cast<fftw_complex*>(a.data()),
		      reinterpret_cast<fftw_complex*>(res.data()),
		      FFTW_BACKWARD, FFTW_ESTIMATE);
  }
  
  fftw_execute(p);
  fftw_destroy_plan(p);

  // Normalize so that ifft(fft(a)) == a
  res /= size(a);
    
  return res;
}



/** Discrete Fourier transform of a 1D array of real numbers. */
template <std::size_t N>
StridedArray<std::complex<double>,N> rfft(StridedArray<double,N>& a)
{
  static_assert( N >= 1,
		 "scalar arrays are not Fourier transformable" );

  auto res_dims = a.dims();
  res_dims[N-1] = res_dims[N-1]/2 + 1;
  StridedArray<std::complex<double>,N> res(res_dims);
  fftw_plan p;

  if (N == 1)
    p = fftw_plan_dft_r2c_1d(a.dim(0), a.data(),
			     reinterpret_cast<fftw_complex*>(res.data()),
			     FFTW_ESTIMATE);
  else if (N == 2)
    p = fftw_plan_dft_r2c_2d(a.dim(0), a.dim(1), a.data(),
			     reinterpret_cast<fftw_complex*>(res.data()),
			     FFTW_ESTIMATE);
  else if (N == 3)
    p = fftw_plan_dft_r2c_3d(a.dim(0), a.dim(1), a.dim(2), a.data(),
			     reinterpret_cast<fftw_complex*>(res.data()),
			     FFTW_ESTIMATE);
  else {
    int dims[N];
    for (int i = 0UL; i < a.ndim; i++)
      dims[i] = a.dim(i);
    p = fftw_plan_dft_r2c(N, dims, a.data(),
			  reinterpret_cast<fftw_complex*>(res.data()),
			  FFTW_ESTIMATE);
  }

  fftw_execute(p);
  fftw_destroy_plan(p);
  
  return res;
}


} // namespace necomi

#endif // HAVE_FFTW

// Local Variables:
// mode: c++
// End:
