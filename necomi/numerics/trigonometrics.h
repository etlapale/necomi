// necomi/numerics/trigonometrics.h – Scalar and delayed trigonometrics
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <cmath>
#include <type_traits>

#include "../arrays/delayed.h"

namespace necomi
{

/**
 * Get the cosinus of each element interpreted as a angle in radians.
 */
template <typename Array,
	  std::enable_if_t<is_indexable<Array>::value>* = nullptr>
auto cos(const Array& a)
{
  return make_delayed(a.dims(), [a](const auto& x) {
      return std::cos(a(x));
    });
}

/**
 * Get the sinus of each element interpreted as a angle in radians.
 */
template <typename Array,
	  std::enable_if_t<is_array<Array>::value>* = nullptr>
auto sin(const Array& a)
{
  using std::sin;
  using C = decltype(sin(std::declval<typename Array::dtype>()));
  return make_delayed<C,Array::ndim>(a.dims(), [a] (const auto& x) {
      return sin(a(x));
    });
}


template <typename Array1, typename Array2,
	  std::enable_if_t<is_indexable<Array1>::value
			   && is_indexable<Array2>::value
			   && Array1::ndim == Array2::ndim>* = nullptr>
auto atan2(const Array1& ys, const Array2& xs)
{
#ifndef NECOMI_NO_BOUND_CHECKS
  if (xs.dims() != ys.dims())
    throw std::length_error("atan2 can only process same dimension arrays");
#endif
  // TODO: check xs and ys dims
  return make_delayed<>(xs.dims(), [xs,ys] (const auto& coords) {
      using std::atan2;
      return atan2(ys(coords), xs(coords));
    });
}


template <typename T,
	  std::enable_if_t<std::is_floating_point<T>::value>* = nullptr>
constexpr T radians(T angle)
{
  return angle *  M_PI / 180.;
}

/**
 * Convert each element interpreted as an angle in degrees into radians.
 */
template <typename Array,
	  std::enable_if_t<is_array<Array>::value>* = nullptr>
auto radians(const Array& a)
{
  return make_delayed(a.dims(), [a](const auto& coords) {
      return radians(a(coords));
    });
}

template <typename T,
	  std::enable_if_t<std::is_floating_point<T>::value>* = nullptr>
constexpr T degrees(T angle)
{
  return angle * 180. / M_PI;
}

/**
 * Convert each element interpreted as a angle in radians into degrees.
 */
template <typename Array,
	  std::enable_if_t<is_array<Array>::value>* = nullptr>
auto degrees(const Array& a)
{
  return make_delayed<typename Array::dtype,Array::ndim>(a.dimensions(), [a] (const auto& x) {
      return degrees(a(x));
    });
}

} // namespace necomi

// Local Variables:
// mode: c++
// End:
