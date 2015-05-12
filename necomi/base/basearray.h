// necomi/base/basearray.h – Common array definitions
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <array>
#include <functional>
#include <numeric>

#include "traits.h"

namespace necomi
{
  /**
   * Remove a dimension or coordinate.
   */
  template <ArrayIndex N, typename std::enable_if_t<N!=0>* = nullptr>
  Coordinates<N-1> remove_coordinate(const Coordinates<N>& coords,
				     ArrayIndex dim)
  {
    Coordinates<N-1> c;
    
    auto oit = std::copy_n(coords.cbegin(), dim, c.begin());
    if (dim != N-1)
      std::copy(coords.cbegin()+dim+1, coords.cend(), oit);
    
    return c;
  }

  /**
   * Add a dimension or coordinate.
   * \note The added coordinate as an unspecified initial value. To
   *       set one, use the three arguments version.
   */
  template <ArrayIndex N>
  Coordinates<N+1> add_coordinate(const Coordinates<N>& coords,
				  ArrayIndex dim)
  {
    Coordinates<N+1> c;
    
    auto oit = std::copy_n(coords.cbegin(), dim, c.begin());
    if (dim != N)
      std::copy(coords.cbegin()+dim, coords.cend(), oit+1);
    
    return c;
  }
  
  template <ArrayIndex N>
  Coordinates<N+1> add_coordinate(const Coordinates<N>& coords,
				  ArrayIndex dim, ArrayDimension value)
  {
    auto c = add_coordinate(coords, dim);
    c[dim] = value;
    return c;
  }
  
  template <ArrayIndex N>
  Coordinates<N+1> append_coordinate(const Coordinates<N>& coords,
				     ArrayDimension value)
  {
    return add_coordinate(coords, N, value);
  }
  
  template <ArrayIndex N>
  Coordinates<N+1> prepend_coordinate(const Coordinates<N>& coords,
				      ArrayDimension value)
  {
    return add_coordinate(coords, 0, value);
  }

template <ArrayIndex N>
Coordinates<N> change_coordinate(const Coordinates<N>& coords,
				 ArrayIndex dim, ArrayDimension value)
{
  Coordinates<N> c = coords;
  c[dim] = value;
  return c;
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
  std::array<ArrayIndex,Array::ndim> path;
  auto p = [](bool val) { return val; };
  return breakable_for_looper<decltype(p),0,Array>(a, path, p);
}

/**
 * Check that all elements of an array of booleans are true.
 * \see any()
 */
template <typename Array,
	  std::enable_if_t<std::is_convertible<typename Array::dtype, bool>::value>* = nullptr>
bool all(const Array& a)
{
  std::array<ArrayIndex,Array::ndim> path;
  auto p = [](bool val) { return !val; };
  return !breakable_for_looper<decltype(p),0,Array>(a, path, p);
}

} // namespace necomi

// Local Variables:
// mode: c++
// End:
