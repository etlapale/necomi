// necomi/delayed/arithmetic.h – Arithmetic on arrays
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <functional>
#include <sstream>

#include "../codecs/streams.h"
#include "maps.h"

namespace necomi {

template <typename Array,
	  std::enable_if_t<is_indexable<Array>::value>* = nullptr>
auto operator-(const Array& a)
{
  return map(a, std::negate<>());
}


template <typename Array1, typename Array2,
	  typename std::enable_if_t<Array1::ndim==Array2::ndim>* = nullptr>
auto operator*(const Array1& a, const Array2& b)
{
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions of a and b are the same
  if (a.dims() != b.dims())
    throw std::length_error("cannot multiply arrays of different dimensions");
#endif
  using C = typename std::common_type<typename Array1::dtype,
				      typename Array2::dtype>::type;
  return make_delayed<C,Array1::ndim>(a.dims(),
				      [a,b] (const auto& x) { return a(x) * b(x); });
}

template <typename U, typename Array,
	  std::enable_if_t<is_indexable<Array>::value
			   && ! is_indexable<U>::value>* = nullptr>
auto operator*(const Array& a, U value)
{
  return map(a, [value](const auto& x) { return x*value; });
}

template <typename U, typename Array,
	  std::enable_if_t<is_indexable<Array>::value
			   && ! is_indexable<U>::value>* = nullptr>
auto operator*(U value, const Array& a)
{
  return map(a, [value](const auto& x) { return value*x; });
}

template <typename Array1, typename Array2,
	  typename std::enable_if_t<Array1::ndim==Array2::ndim>* = nullptr>
auto operator/(const Array1& a, const Array2& b)
{
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions of a and b are the same
  if (a.dims() != b.dims())
    throw std::length_error("cannot divide arrays of different dimensions");
#endif
  using C = typename std::common_type<typename Array1::dtype,
				      typename Array2::dtype>::type;
  return make_delayed<C,Array1::ndim>(a.dims(),
				      [a,b] (const auto& x) { return a(x) / b(x); });
}

template <typename Array, typename U,
	  std::enable_if_t<is_indexable<Array>::value
			   && ! is_array<U>::value>* = nullptr>
auto operator/(U value, const Array& a)
{
  using C = typename std::common_type<typename Array::dtype, U>::type;
  return make_delayed<C,Array::ndim>(a.dims(),
				     [a,value] (const auto& x)
				     { return value/a(x); });
}

template <typename Array, typename U,
	  std::enable_if_t<is_indexable<Array>::value
			   && ! is_array<U>::value>* = nullptr>
auto operator/(const Array& a, U value)
{
  using C = typename std::common_type<typename Array::dtype, U>::type;
  return make_delayed<C,Array::ndim>(a.dims(),
				     [a,value] (const auto& x)
				     { return a(x)/value; });
}


template <typename Array1, typename Array2,
	  typename std::enable_if_t<is_indexable<Array1>::value
				    && is_indexable<Array2>::value
				    && Array1::ndim==Array2::ndim>* = nullptr>
auto operator-(const Array1& a, const Array2& b)
{
  using C = typename std::common_type<typename Array1::dtype,
				      typename Array2::dtype>::type;
  
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions of a and b are the same
  if (a.dims() != b.dims()) {
    std::ostringstream msg;
    msg << "cannot subtract arrays of different dimensions (";
    copy_dims(a.dims(), msg) << " != ";
    copy_dims(b.dims(), msg) << ")";
    throw std::length_error(msg.str());
  }
#endif
  return make_delayed<C, Array1::ndim>(a.dims(),
				       [a,b](const auto& coords) {
					 return a(coords) - b(coords);
				       });
}

template <typename Array, typename U,
	  std::enable_if_t<is_indexable<Array>::value
			   && ! is_array<U>::value>* = nullptr>
auto operator-(U value, const Array& a)
{
  return map(a, [value](const auto& x) { return value - x; });
}

template <typename Array, typename U,
	  std::enable_if_t<is_indexable<Array>::value
			   && ! is_array<U>::value>* = nullptr>
auto operator-(const Array& a, U value)
{
  
  return map(a, [value](const auto& x) { return x - value; });
}

template <typename Array1, typename Array2,
	  typename std::enable_if_t<Array1::ndim==Array2::ndim>* = nullptr>
auto operator+(const Array1& a, const Array2& b)
{
  using C = typename std::common_type<typename Array1::dtype,
				      typename Array2::dtype>::type;
  
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions of a and b are the same
  if (a.dims() != b.dims()) {
    std::ostringstream msg;
    msg << "cannot sum arrays of different dimensions (";
    copy_dims(a.dims(), msg) << " != ";
    copy_dims(b.dims(), msg) << ")";
    throw std::length_error(msg.str());
  }
#endif
  return make_delayed<C, Array1::ndim>(a.dims(),
				       [a,b](const auto& coords) {
					 return a(coords) + b(coords);
				       });
}

template <typename T, typename Array,
	  std::enable_if_t<is_indexable<Array>::value
			   && ! is_array<T>::value>* = nullptr>
auto operator+(const T& value, const Array& a)
{
  return map(a, [value](const auto& x) { return value + x; });
}

template <typename T, typename Array,
	  std::enable_if_t<is_indexable<Array>::value
			   && ! is_array<T>::value>* = nullptr>
auto operator+(const Array& a, const T& value)
{
  
  return map(a, [value](const auto& x) { return x + value; });
}



template <typename Indexable, typename T=typename Indexable::dtype>
StridedArray<T,Indexable::ndim>& operator+=(StridedArray<T,Indexable::ndim>& a,
				     const Indexable& b)
{
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions of a and b are the same
  if (a.dims() != b.dims())
    throw std::length_error("cannot increment with array of different dimensions");
#endif
  a.map([&b](auto& path, auto& val) {val += b(path);});
  return a;
}

template <typename T, std::size_t N,
	  typename Indexable, typename U=typename Indexable::dtype,
	  std::enable_if_t<is_promotable<U,T>::value
			   && N==Indexable::ndim>* = nullptr>
StridedArray<T,N>& operator*=(StridedArray<T,N>& numerator, const Indexable& denominator)
{
  numerator.map([&denominator](auto& coords, auto& val) {
      val *= denominator(coords);
    });
  return numerator;
}

template <typename T, std::size_t N,
	  typename Indexable, typename U=typename Indexable::dtype,
	  std::enable_if_t<is_promotable<U,T>::value
			   && N==Indexable::ndim>* = nullptr>
StridedArray<T,N>& operator/=(StridedArray<T,N>& numerator, const Indexable& denominator)
{
  numerator.map([&denominator](auto& coords, auto& val) {
      val /= denominator(coords);
    });
  return numerator;
}

/**
 * Divide each element of the array by a number.
 */
template <typename T, std::size_t N, typename U,
	  std::enable_if_t<is_promotable<U,T>::value>* = nullptr>
StridedArray<T,N>&
operator/=(StridedArray<T,N>& numerator, const U& div)
{
  numerator.map([&div](const auto&, T& val) {
      val /= div;
    });
  return numerator;
}

} // namespace necomi

// Local Variables:
// mode: c++
// End:
