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
    for (int i = 0UL; i < a.ndim(); i++)
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
    for (int i = 0UL; i < a.ndim(); i++)
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
    for (int i = 0UL; i < a.ndim(); i++)
      dims[i] = a.dim(i);
    p = fftw_plan_dft_r2c(N, dims, a.data(),
			  reinterpret_cast<fftw_complex*>(res.data()),
			  FFTW_ESTIMATE);
  }

  fftw_execute(p);
  fftw_destroy_plan(p);
  
  return res;
}

/** Inverse Fourier transform of a 1D array of real numbers. */
template <std::size_t N>
StridedArray<double,N> irfft(StridedArray<std::complex<double>,N>& a,
			     std::size_t last_dim)
{
  static_assert( N >= 1,
		 "scalar arrays are not Fourier transformable" );

  auto res_dims = a.dims();
  res_dims[N-1] = last_dim;
  StridedArray<double,N> res(res_dims);
  fftw_plan p;

  if (N == 1)
    p = fftw_plan_dft_c2r_1d(res.dim(0),
			     reinterpret_cast<fftw_complex*>(a.data()),
			     res.data(), FFTW_ESTIMATE);
  else if (N == 2)
    p = fftw_plan_dft_c2r_2d(res.dim(0), res.dim(1),
			     reinterpret_cast<fftw_complex*>(a.data()),
			     res.data(), FFTW_ESTIMATE);
  else if (N == 3)
    p = fftw_plan_dft_c2r_3d(res.dim(0), res.dim(1), res.dim(2),
			     reinterpret_cast<fftw_complex*>(a.data()),
			     res.data(), FFTW_ESTIMATE);
  else {
    int dims[N];
    for (int i = 0UL; i < a.ndim(); i++)
      dims[i] = res.dim(i);
    p = fftw_plan_dft_c2r(N, dims,
			  reinterpret_cast<fftw_complex*>(a.data()),
			  res.data(), FFTW_ESTIMATE);
  }

  fftw_execute(p);
  fftw_destroy_plan(p);

  // Normalize so that irfft(rfft(a), a.dim(N-1)) == a
  res /= size(res);
  
  return res;
}

template <std::size_t N>
StridedArray<double,N> fftconvolve(const StridedArray<double,N>& input,
				   const StridedArray<double,N>& kernel)
{
  // Compute the padded dimensions
  auto pdims = input.dims();
  for (auto i = 0UL; i < N; i++)
    pdims[i] = 2*((input.dim(i) + kernel.dim(i))/2);

  // Pad the input and kernel arrays
  auto pinput = strided(pad(input, pdims));
  auto pkernel = strided(pad(kernel, pdims));

  // Compute the real Fourier tranforms of the input and kernel
  // TODO: reuse at least the plan here
  auto finput = rfft(pinput);
  auto fkernel = rfft(pkernel);

  // Center the FFT
  finput.map([](const auto& coords, auto& val) {
      auto s = std::accumulate(coords.cbegin(), coords.cend(), 0);
      auto sign = s % 2 == 0 ? 1 : -1;
      val *= sign;
    });

  // Multiply the transformed signals
  auto fres = strided(finput * fkernel);

  // Convert back the transformed signal into the real domain
  auto res = irfft(fres, pinput.dim(N-1));

  // Remove the padding
  std::array<std::array<std::size_t,3>,N> scs;
  for (auto i = 0UL; i < N; i++) {
    scs[i][0] = (res.dim(i) - input.dim(i)) / 2;
    scs[i][1] = input.dim(i);
    scs[i][2] = 1;
  }
  
  return strided(res.slice(Slice<std::size_t,N>(scs)));
}
				   

} // namespace necomi

#endif // HAVE_FFTW

// Local Variables:
// mode: c++
// End:
