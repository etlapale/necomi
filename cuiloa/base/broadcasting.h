// cuiloa/base/broadcasting.h – Shape broadcasting operations.
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include "delayed.h"

namespace cuiloa {

/**
 * Prepend extra dimensions to an array.
 */
template <std::size_t M, typename Array>
auto widen(const Dimensions<M>& dims, const Array& a)
{
  static_assert(M > Array::ndim, "array dimensions cannot be shrinked");
#ifndef CUILOA_NO_BOUND_CHECKS
  // Make sure the dimensions are matching
  for (ArrayIndex i = 0; i < Array::ndim; i++)
    if (a.dimensions()[i] != dims[i+M-Array::ndim])
      throw std::length_error("cannot broadcast arrays to different dimensions");
#endif
  return make_delayed<typename Array::dtype,M>(dims,
					       [a](const auto& coords) {
      Coordinates<Array::ndim> old_coords;
      std::copy(coords.cbegin()+(M-Array::ndim), coords.cend(),
		old_coords.begin());
      return a(old_coords);
    });
}

/**
 * Append extra dimensions to an array.
 */
template <ArrayIndex M, typename Array>
auto widen_right(const Dimensions<M>& dims, const Array& a)
{
  static_assert(M > Array::ndim, "array dimensions cannot be shrinked");
#ifndef CUILOA_NO_BOUND_CHECKS
  // Make sure the dimensions are matching
  for (ArrayIndex i = 0; i < Array::ndim; i++)
    if (a.dimensions()[i] != dims[i])
      throw std::length_error("cannot right-broadcast arrays to different dimensions");
#endif
  return make_delayed<typename Array::dtype, M>(dims,
						[a](const auto& coords) {
      Coordinates<Array::ndim> coords_a;
      std::copy_n(coords.cbegin(), Array::ndim, coords_a.begin());
      return a(coords_a); });
}

namespace delayed {
namespace broadcasting {

template <typename Array1, typename Array2,
	  typename std::enable_if<(Array2::ndim>Array1::ndim)>::type* = nullptr
  >
auto operator*(const Array1& a, const Array2& b)
{
  return cuiloa::delayed::operator*(widen<Array2::ndim>(b.dimensions(), a), b);
}

template <typename Array1, typename Array2,
	  typename std::enable_if<(Array1::ndim>Array2::ndim)>::type* = nullptr
  >
auto operator*(const Array1& a, const Array2& b)
{
  return cuiloa::delayed::operator*(a, widen<Array1::ndim>(a.dimensions(), b));
}

      template <typename Concrete1, typename T, ArrayIndex N,
		typename Concrete2, ArrayIndex M,
		typename std::enable_if<(M>N)>::type* = nullptr
		>
      auto operator/(const AbstractArray<Concrete1,T,N>& a,
		     const AbstractArray<Concrete2,T,M>& b)
      {
	return cuiloa::delayed::operator/(widen<M>(b.dimensions(), a), b);
      }

      template <typename Concrete1, typename T, ArrayIndex N,
		typename Concrete2, ArrayIndex M,
		typename std::enable_if<(N>M)>::type* = nullptr
		>
      auto operator/(const AbstractArray<Concrete1,T,N>& a,
		     const AbstractArray<Concrete2,T,M>& b)
      {
	return cuiloa::delayed::operator/(a, widen<N>(a.dimensions(), b));
      }

      template <typename Concrete1, typename T, ArrayIndex N,
		typename Concrete2, ArrayIndex M,
		typename std::enable_if<(M>N)>::type* = nullptr
		>
      auto operator+(const AbstractArray<Concrete1,T,N>& a,
		     const AbstractArray<Concrete2,T,M>& b)
      {
	return cuiloa::delayed::operator+(widen<M>(b.dimensions(), a), b);
      }

      template <typename Concrete1, typename T, ArrayIndex N,
		typename Concrete2, ArrayIndex M,
		typename std::enable_if<(N>M)>::type* = nullptr
		>
      auto operator+(const AbstractArray<Concrete1,T,N>& a,
		     const AbstractArray<Concrete2,T,M>& b)
      {
	return cuiloa::delayed::operator+(a, widen<N>(a.dimensions(), b));
      }

      template <typename Concrete1, typename T, ArrayIndex N,
		typename Concrete2, ArrayIndex M,
		typename std::enable_if<(M>N)>::type* = nullptr
		>
      auto operator-(const AbstractArray<Concrete1,T,N>& a,
		     const AbstractArray<Concrete2,T,M>& b)
      {
	return cuiloa::delayed::operator-(widen<M>(b.dimensions(), a), b);
      }

      template <typename Concrete1, typename T, ArrayIndex N,
		typename Concrete2, ArrayIndex M,
		typename std::enable_if<(N>M)>::type* = nullptr
		>
      auto operator-(const AbstractArray<Concrete1,T,N>& a,
		     const AbstractArray<Concrete2,T,M>& b)
      {
	return cuiloa::delayed::operator-(a, widen<N>(a.dimensions(), b));
      }
    } // namespace broadcasting
  } // namespace delayed
} // namespace cuiloa

// Local Variables:
// mode: c++
// End:
