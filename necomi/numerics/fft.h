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
template <typename T>
StridedArray<std::complex<T>,1> dft(StridedArray<std::complex<T>,1>& a)
{
  StridedArray<std::complex<T>,1> res(a.dims());

  fftw_plan p = fftw_plan_dft_1d(a.dim(0),
				 reinterpret_cast<fftw_complex*>(a.data()),
				 reinterpret_cast<fftw_complex*>(res.data()),
				 FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_execute(p);
  fftw_destroy_plan(p);
  
  return res;
}

} // namespace necomi

#endif // HAVE_FFTW

// Local Variables:
// mode: c++
// End:
