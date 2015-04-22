// cuiloa/base/trigonometrics.h – Scalar and delayed trigonometrics
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include "../base/delayed.h"

namespace cuiloa
{

/**
 * Get the cosinus of each element interpreted as a angle in radians.
 */
template <typename Array,
	  std::enable_if_t<is_array<Array>::value>* = nullptr>
auto cos(const Array& a)
{
  using std::cos;
  using C = decltype(cos(std::declval<typename Array::dtype>()));
  return make_delayed<C,Array::ndim>(a.dimensions(), [a] (const auto& x) {
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
  return make_delayed<C,Array::ndim>(a.dimensions(), [a] (const auto& x) {
      return sin(a(x));
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
  return make_delayed<typename Array::dtype,Array::ndim>(a.dimensions(), [a] (const auto& x) {
      return radians(a(x));
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

} // namespace cuiloa

// Local Variables:
// mode: c++
// End:
