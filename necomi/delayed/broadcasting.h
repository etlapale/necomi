// necomi/delayed/broadcasting.h – Shape broadcasting operations.
//
// Copyright © 2016 Émilien Tlapale
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include "../arrays/delayed.h"
#include "../arrays/stridedarray.h"
#include "../numerics/arithmetics.h"

namespace necomi {

/**
 * Prepend extra dimensions to an array.
 */
template <std::size_t M, typename Array>
auto widen(const std::array<std::size_t,M>& dims, const Array& a)
{
  static_assert(M > Array::ndim(), "array dimensions cannot be shrinked");
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions are matching
  for (std::size_t i = 0; i < Array::ndim(); i++)
    if (a.dims()[i] != dims[i+M-Array::ndim()])
      throw std::length_error("cannot broadcast arrays to different dimensions");
#endif
  return make_delayed(dims, [a](const auto& coords) {
      std::array<std::size_t,Array::ndim()> old_coords;
      std::copy(coords.cbegin()+(M-Array::ndim()), coords.cend(),
		old_coords.begin());
      return a(old_coords);
    });
}

/**
 * Append extra dimensions to an array.
 */
template <std::size_t M, typename Array,
	  std::enable_if_t<M != Array::ndim()>* = nullptr>
auto widen_right(const std::array<std::size_t,M>& dims, const Array& a)
{
  static_assert(M > Array::ndim(), "array dimensions cannot be shrinked");
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions are matching
  for (std::size_t i = 0; i < Array::ndim(); i++)
    if (a.dims()[i] != dims[i])
      throw std::length_error("cannot right-broadcast arrays to different dimensions");
#endif
  return make_delayed(dims, [a](const auto& coords) {
      std::array<std::size_t,Array::ndim()> coords_a;
      std::copy_n(coords.cbegin(), Array::ndim(), coords_a.begin());
      return a(coords_a);
    });
}

template <std::size_t M, typename Array,
	  std::enable_if_t<M == Array::ndim()>* = nullptr>
auto widen_right(const std::array<std::size_t,M>& dims, const Array& a)
{
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions are matching
  for (std::size_t i = 0; i < Array::ndim(); i++)
    if (a.dims()[i] != dims[i])
      throw std::length_error("cannot right-broadcast arrays to different dimensions");
#endif
  return a;
}

namespace broadcasting {

template <typename Array1, typename Array2,
	  typename std::enable_if<(Array2::ndim()>Array1::ndim())>::type* = nullptr
  >
auto operator*(const Array1& a, const Array2& b)
{
  return necomi::operator*(widen<Array2::ndim()>(b.dims(), a), b);
}

template <typename Array1, typename Array2,
	  typename std::enable_if<(Array1::ndim()>Array2::ndim())>::type* = nullptr
  >
auto operator*(const Array1& a, const Array2& b)
{
  return necomi::operator*(a, widen<Array1::ndim()>(a.dims(), b));
}

template <typename Array1, typename Array2,
	  typename std::enable_if<(Array2::ndim()>Array1::ndim())>::type* = nullptr
  >
auto operator/(const Array1& a, const Array2& b)
{
  return necomi::operator/(widen<Array2::ndim()>(b.dims(), a), b);
}

template <typename Array1, typename Array2,
	  typename std::enable_if<(Array1::ndim()>Array2::ndim())>::type* = nullptr
  >
auto operator/(const Array1& a, const Array2& b)
{
  return necomi::operator/(a, widen<Array1::ndim()>(a.dims(), b));
}

template <typename Array1, typename Array2,
	  typename std::enable_if<(Array2::ndim()>Array1::ndim())>::type* = nullptr
  >
auto operator+(const Array1& a, const Array2& b)
{
  return necomi::operator+(widen<Array2::ndim()>(b.dims(), a), b);
}

template <typename Array1, typename Array2,
	  typename std::enable_if<(Array1::ndim()>Array2::ndim())>::type* = nullptr
  >
auto operator+(const Array1& a, const Array2& b)
{
  return necomi::operator+(a, widen<Array1::ndim()>(a.dims(), b));
}

template <typename Array1, typename Array2,
	  typename std::enable_if<(Array2::ndim()>Array1::ndim())>::type* = nullptr
  >
auto operator-(const Array1& a, const Array2& b)
{
  return necomi::operator-(widen<Array2::ndim()>(b.dims(), a), b);
}

template <typename Array1, typename Array2,
	  typename std::enable_if<(Array1::ndim()>Array2::ndim())>::type* = nullptr
  >
auto operator-(const Array1& a, const Array2& b)
{
  return necomi::operator-(a, widen<Array1::ndim()>(a.dims(), b));
}


template <typename T, std::size_t N,
	  typename Indexable, typename U=typename Indexable::dtype,
	  std::enable_if_t<(N>Indexable::ndim()) && is_promotable<U,T>::value>* = nullptr>
StridedArray<T,N>& operator/=(StridedArray<T,N>& numerator, const Indexable& denominator)
{
  return necomi::operator/=(numerator,
			    widen(numerator.dims(), denominator));
}


} // namespace broadcasting
} // namespace necomi

// Local Variables:
// mode: c++
// End:
