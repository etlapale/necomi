// necomi/numerics/fft.h – Wrapper around FFTW
//
// Copyright © 2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#ifdef HAVE_FFTW

#include <fftw3.h>
#include <complex>
#include "../arrays/stridedarray.h"


namespace necomi {

/** Discrete Fourier transform of a 1D array of complex number. */
template <typename T, std::size_t N,
	  std::enable_if<1 < N && N <= 2>* = nullptr>
StridedArray<std::complex<T>,N> fft(StridedArray<std::complex<T>,N>& a)
{
  StridedArray<std::complex<T>,N> res(a.dims());
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

  fftw_execute(p);
  fftw_destroy_plan(p);
  
  return res;
}

/** Inverse discrete Fourier transform of a 1D array of complex number. */
template <typename T, std::size_t N,
	  std::enable_if<1 < N && N <= 2>* = nullptr>
StridedArray<std::complex<T>,N> ifft(StridedArray<std::complex<T>,N>& a)
{
  StridedArray<std::complex<T>,N> res(a.dims());
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

    
  fftw_execute(p);
  fftw_destroy_plan(p);

  // Normalize so that ifft(fft(a)) == a
  res /= size(a);
    
  return res;
}

} // namespace necomi

#endif // HAVE_FFTW

// Local Variables:
// mode: c++
// End:
