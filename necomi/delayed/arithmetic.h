// necomi/delayed/arithmetic.h – Arithmetic on arrays
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include "../arrays/delayed.h"

namespace necomi {

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
  using C = typename std::common_type<typename Array::dtype, U>::type;
  return make_delayed<C,Array::ndim>(a.dims(),
				     [a,value] (const auto& x)
				     { return a(x)*value; });
}

template <typename U, typename Array,
	  std::enable_if_t<is_indexable<Array>::value
			   && ! is_indexable<U>::value>* = nullptr>
auto operator*(U value, const Array& a)
{
  using C = typename std::common_type<typename Array::dtype, U>::type;
  return make_delayed<C,Array::ndim>(a.dims(),
				     [a,value] (const auto& x)
				     { return value*a(x); });
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
			   && ! is_array<U>::valu>* = nullptr>
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
	  typename std::enable_if_t<Array1::ndim==Array2::ndim>* = nullptr>
auto operator-(const Array1& a, const Array2& b)
{
  using C = typename std::common_type<typename Array1::dtype,
				      typename Array2::dtype>::type;
  
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions of a and b are the same
  if (a.dims() != b.dims())
    throw std::length_error("cannot sum arrays of different dimensions");
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
  using C = typename std::common_type<typename Array::dtype, U>::type;
  return make_delayed<C,Array::ndim>(a.dims(),
				     [a,value] (const auto& x)
				     { return value - a(x); });
}

template <typename Array, typename U,
	  std::enable_if_t<is_indexable<Array>::value
			   && ! is_array<U>::value>* = nullptr>
auto operator-(const Array& a, U value)
{
  using C = typename std::common_type<typename Array::dtype, U>::type;
  return make_delayed<C,Array::ndim>(a.dims(),
				     [a,value] (const auto& x)
				     { return a(x) - value; });
}

template <typename Array1, typename Array2,
	  typename std::enable_if_t<Array1::ndim==Array2::ndim>* = nullptr>
auto operator+(const Array1& a, const Array2& b)
{
  using C = typename std::common_type<typename Array1::dtype,
				      typename Array2::dtype>::type;
  
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions of a and b are the same
  if (a.dims() != b.dims())
    throw std::length_error("cannot sum arrays of different dimensions");
#endif
  return make_delayed<C, Array1::ndim>(a.dims(),
				       [a,b](const auto& coords) {
					 return a(coords) + b(coords);
				       });
}

template <typename T, typename Array,
	  typename C = typename std::common_type<typename Array::dtype>::type,
	  std::enable_if_t<is_indexable<Array>::value
			   && ! is_array<T>::value>* = nullptr>
auto operator+(const T& value, const Array& a)
{
  return make_delayed<C, Array::ndim>(a.dims(), [value,a](const auto& coords) { return value + a(coords); });
}

template <typename T, typename Array,
	  typename C = typename std::common_type<typename Array::dtype>::type,
	  std::enable_if_t<is_indexable<Array>::value
			   && ! is_array<T>::value>* = nullptr>
auto operator+(const Array& a, const T& value)
{
  return make_delayed<C, Array::ndim>(a.dims(), [value,a](const auto& coords) { return a(coords) + value; });
}

} // namespace necomi

// Local Variables:
// mode: c++
// End:
