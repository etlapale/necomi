// necomi/delayed/comparisons.h – Array comparisons
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <stdexcept>
#include "../arrays/delayed.h"


namespace necomi {

/**
 * Compare two arrays element-wise.
 * This operator is disabled if the shapes do not match, to allow for
 * shape broadcasting by other operators.
 */
template <typename Array1, typename Array2,
	  typename std::enable_if_t<is_array<Array1>::value
				    && is_array<Array2>::value>* = nullptr,
	  typename std::enable_if_t<Array1::ndim == Array2::ndim>* = nullptr>
  auto operator==(const Array1& a, const Array2& b)
{
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions of a and b are the same
  if (a.dims() != b.dims())
    throw std::length_error("cannot compare arrays of different dimensions");
#endif
  return make_delayed<bool,Array1::ndim>(a.dims(),
					 [a,b] (const auto& coords) {
					   return a(coords) == b(coords);
					 });
}

template <typename Array1, typename Array2,
	  typename std::enable_if_t<is_array<Array1>::value &&
				    is_array<Array2>::value &&
				    Array1::ndim==Array2::ndim>* = nullptr>
  auto operator!=(const Array1& a, const Array2& b)
{
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions of a and b are the same
  if (a.dims() != b.dims())
    throw std::length_error("cannot compare arrays of different dimensions");
#endif
  return make_delayed<bool,Array1::ndim>(a.dims(),
					 [a,b] (const auto& coords) {
					   return a(coords) != b(coords);
					 });
}

template <typename Array, typename T,
	  std::enable_if_t<is_indexable<Array>::value
			   && ! is_array<T>::value>* = nullptr>
auto operator>(const Array& a, const T& val)
{
  return make_delayed<bool,Array::ndim>(a.dims(), [a,val](auto& path) {
      return a(path) > val;
    });
}

template <typename Array1, typename Array2,
	  typename std::enable_if_t<is_array<Array1>::value
				    && is_array<Array2>::value>* = nullptr,
	  typename std::enable_if_t<Array1::ndim == Array2::ndim>* = nullptr>
auto operator>(const Array1& a, const Array2& b)
{
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions of a and b are the same
  if (a.dims() != b.dims())
    throw std::length_error("cannot compare arrays of different dimensions");
#endif
  return make_delayed<bool,Array1::ndim>(a.dims(),
					 [a,b] (const auto& coords) {
					   return a(coords) > b(coords);
					 });
}

template <typename Array, typename T,
	  std::enable_if_t<is_indexable<Array>::value
			   && ! is_array<T>::value>* = nullptr>
  auto operator<(const Array& a, const T& val)
{
  return make_delayed<bool,Array::ndim>(a.dims(), [a,val](auto& path) {
      return a(path) < val;
    });
}


template <typename Array1, typename Array2,
	  typename std::enable_if_t<is_array<Array1>::value
				    && is_array<Array2>::value>* = nullptr,
	  typename std::enable_if_t<Array1::ndim == Array2::ndim>* = nullptr>
auto operator<(const Array1& a, const Array2& b)
{
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions of a and b are the same
  if (a.dims() != b.dims())
    throw std::length_error("cannot compare arrays of different dimensions");
#endif
  return make_delayed<bool,Array1::ndim>(a.dims(),
					 [a,b] (const auto& coords) {
					   return a(coords) < b(coords);
					 });
}
  

/**
 * Check if any element of an array of booleans or convertible to
 * booleans is false.
 * \see all()
 */
template <typename Array,
	  typename std::enable_if_t<std::is_convertible<typename Array::dtype, bool>::value>* = nullptr>
bool any(const Array& a)
{
  typename Array::dims_type coords;
  auto p = [](bool val) { return val; };
  return breakable_for_looper<Array,0,decltype(p)>(a, coords, p);
}

/**
 * Check that all elements of an array of booleans are true.
 * \see any()
 */
template <typename Array,
	  std::enable_if_t<std::is_convertible<typename Array::dtype, bool>::value>* = nullptr>
bool all(const Array& a)
{
  typename Array::dims_type coords;
  auto p = [](bool val) { return !val; };
  return !breakable_for_looper<Array,0,decltype(p)>(a, coords, p);
}


} // namespace necomi

// Local Variables:
// mode: c++
// End:
