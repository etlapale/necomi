// necomi/numerics/statistics.h – Basic statistical functions
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include "../arrays/delayed.h"
#include "../arrays/stridedarray.h"

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
  for_each(a, [&](const auto&, auto val) { total += val; });
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
  for_each(a, [&a,&max_coords,&max_val](const auto& coords, auto val)
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
 * Sum an array across a given dimension.
 */
template <typename Array, typename dim_type = typename Array::dim_type,
	  typename std::enable_if<Array::ndim!=0>::type* = nullptr>
auto sum(const Array&a, dim_type dim)
{
  return make_delayed<typename Array::dtype,Array::ndim-1>(remove_coordinate(a.dims(), dim), [a,dim] (auto& x) {
      // Path in the original array
      auto orig_path = add_coordinate(x, dim);
      // Sum all the elements in the dimension
      typename Array::dtype val = 0;
      for (std::size_t i = 0; i < a.dims()[dim]; i++) {
	orig_path[dim] = i;
	val += a(orig_path);
      }
      return val;
    });
}

/**
 * Average an array across a given dimension.
 */
template <typename Array, typename dim_type = typename Array::dim_type,
	  typename std::enable_if_t<Array::ndim!=0>* = nullptr>
auto average(const Array& a, dim_type dim)
{
  return sum(a,dim) / static_cast<typename Array::dtype>(a.dims()[dim]);
}

/**
 * Average across all dimensions.
 */
template <typename Array,
	  typename std::enable_if_t<Array::ndim!=0>* = nullptr>
auto average(const Array& a)
{
  return sum(a) / static_cast<typename Array::dtype>(size(a));
}



/**
 * Compute a sample variance with a two-pass formula.
 *
 * When the function is called, the average along the given dimension
 * is computed and stored in an immediate array.
 *
 * When the Bessel correction is enabled, an N-1 divider is used
 * (default in Matlab), otherwise a N divider is used (default
 * in NumPy).
 */
template <typename Array, typename dim_type = typename Array::dim_type,
	  typename std::enable_if_t<Array::ndim!=0>* = nullptr>
auto variance(const Array& a, dim_type dim, bool bessel_correction)
{
  auto avg = strided_array(average(a, dim));
  return make_delayed<typename Array::dtype, Array::ndim-1>(remove_coordinate(a.dims(), dim),
			     [a,dim,avg,bessel_correction]
			     (const auto& x)
			     {
			       // Path in the original array
			       auto orig_path = add_coordinate(x, dim);
			       // Sum the squared deviations to the mean
			       typename Array::dtype val = 0;
			       for (std::size_t i = 0; i < a.dims()[dim]; i++) {
				 orig_path[dim] = i;
				 val += power<2>(a(orig_path) - avg(x));
			       }
			       return val / (bessel_correction ? a.dim(dim) - 1 : a.dim(dim));
			     });
}

template <typename Array,
	  typename std::enable_if_t<Array::ndim!=0>* = nullptr>
auto variance(const Array& a, bool bessel_correction)
{
  static_assert(std::is_floating_point<typename Array::dtype>::value,
		"statistics on arrays require floating elements");
  auto avg = average(a);
  typename Array::dtype res = 0;
  for_each(a, [avg,&res](const auto&, auto val) {
      res += power<2>(val - avg);
    });
  return res / (bessel_correction ? size(a) - 1 : size(a));
}

template <typename Array, typename dim_type = typename Array::dim_type,
	  typename std::enable_if_t<Array::ndim!=0>* = nullptr>
auto deviation(const Array& a, dim_type dim, bool bessel_correction)
{
  return sqrt(variance(a, dim, bessel_correction));
}

template <typename Array,
	  typename std::enable_if_t<Array::ndim!=0>* = nullptr>
auto deviation(const Array& a, bool bessel_correction)
{
  return std::sqrt(variance(a, bessel_correction));
}


template <typename Array, typename U,
	  typename std::enable_if_t<is_promotable<U,typename Array::dtype>::value>* = nullptr>
auto max(const Array& a, U value)
{
  return make_delayed<typename Array::dtype, Array::ndim>(a.dims(),
							  [a,value=static_cast<typename Array::dtype>(value)]
							  (const auto& coords) { return std::max(a(coords), value); });
  }



/**
 * Cumulative sum.
 */
template <typename Indexable, typename T=typename Indexable::dtype>
StridedArray<T,Indexable::ndim> cumsum(const Indexable& a, std::size_t dim = 0)
{
  StridedArray<T,Indexable::ndim> res(a.dims());
  res.map([&res,dim,&a](auto& path, auto& valx) {
      if (path[dim] == 0) {
	valx = a(path);
      }
      else {
	auto prev = path;
	prev[dim]--;
	valx = res(prev) + a(path);
      }
    });
  return res;
}


} // namespace necomi

// Local Variables:
// mode: c++
// End:
