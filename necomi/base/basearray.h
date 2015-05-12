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
 * Compute the size of an array (product of dimensions).
 */
template <typename Array, typename dim_type=typename Array::dim_type>
dim_type size(const Array& a)
{
  return std::accumulate(a.dims().cbegin(), a.dims().cend(), 1,
			 std::multiplies<>());
}


/**
 * Sum all the elements of an array.
 * \warning This may overflow.
 */
template <typename Array, typename T=typename Array::dtype>
T sum(const Array& a)
{
  T total = 0;
  a.map([&](auto&, auto val) { total += val; });
  return total;
}

/**
 * Coordinate of the maximal value in an array.
 */
template <typename Array, typename dims_type = typename Array::dims_type>
dims_type argmax(const Array& a)
{
  // Set initial result to first coordinate
  dims_type max_coords;
  max_coords.fill(0);
  auto max_val = a(max_coords);

  // Search for the maximal value
  a.map([&a,&max_coords,&max_val](const auto& coords, auto val)
	{ if (val > max_val) {
	    max_val = val;
	    max_coords = coords;
	  }
	});

  return max_coords;
}

/**
 * Maximum value in the array.
 */
template <typename Array, typename T=typename Array::dtype>
T max(const Array& a)
{
  return a(argmax(a));
}

template <typename Array, typename dims_type = typename Array::dims_type>
dims_type argmin(const Array& a)
{
  // Set initial result to first coordinate
  dims_type min_coords;
  min_coords.fill(0);
  auto min_val = a(min_coords);

  // Search for the maximal value
  a.map([&a,&min_coords,&min_val](const auto& coords, auto val)
	{ if (val < min_val) {
	    min_val = val;
	    min_coords = coords;
	  }
	});

  return min_coords;
}

/**
 * Minimum value in the array.
 */
template <typename Array, typename T=typename Array::dtype>
T min(const Array& a)
{
  return a(argmin(a));
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
