// necomi/numerics/statistics.h – Basic statistical functions
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

namespace necomi
{

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

} // namespace necomi

// Local Variables:
// mode: c++
// End:
