// cuiloa/funcs/numerics.h – Basic numerical functions
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include "../base/delayed.h"

namespace cuiloa {
namespace delayed {

/**
 * Create a delayed array from the absolute values of another.
 * \param a	An \ref IndexableArray "indexable array".
 */
template <typename IndexableArray,
	  typename std::enable_if_t<is_indexable<IndexableArray>::value>* = nullptr>
auto abs(const IndexableArray& a)
{
  return make_delayed(a.dimensions(),
		      [a](const Coordinates<IndexableArray::ndim>& path)
		      { return std::abs(a(path)); });
}

/**
 * Norms available to function \ref norm.
 */
enum class Norm {
  /** Maximum of the absolute values. */
  Infinity
};

/**
 * Average an array across a given dimension.
 * \param a	An \ref IndexableArray "indexable array".
 */
template <typename IndexableArray>
auto norm(const IndexableArray& a, Norm norm)
{
  switch (norm) {
  case Norm::Infinity:
    return max(abs(a));
  }
}

template <typename Array>
auto exp(const Array& a)
{
  return make_delayed<typename Array::dtype,Array::ndim>(a.dimensions(),
							 [a] (auto& x) {
							   return std::exp(a(x));
							 });
}

/**
 * Sum an array across a given dimension.
 */
template <typename Array,
	  typename std::enable_if<Array::ndim!=0>::type* = nullptr>
auto sum(const Array&a, ArrayIndex dim)
{
  return make_delayed<typename Array::dtype,Array::ndim-1>(remove_coordinate(a.dimensions(), dim), [a,dim] (auto& x) {
      // Path in the original array
      auto orig_path = add_coordinate(x, dim);
      // Sum all the elements in the dimension
      typename Array::dtype val = 0;
      for (ArrayIndex i = 0; i < a.dimensions()[dim]; i++) {
	orig_path[dim] = i;
	val += a(orig_path);
      }
      return val;
    });
}

/**
 * Average an array across a given dimension.
 */
template <typename Array,
	  typename std::enable_if_t<Array::ndim!=0>* = nullptr>
auto average(const Array& a, ArrayIndex dim)
{
  return sum(a,dim) / static_cast<typename Array::dtype>(a.dimensions()[dim]);
}

/**
 * Average across all dimensions.
 */
template <typename Array,
	  typename std::enable_if_t<Array::ndim!=0>* = nullptr>
auto average(const Array& a)
{
  return sum(a) / static_cast<typename Array::dtype>(a.size());
}
  
template <unsigned N, typename T>
std::enable_if_t<N==0,T>
power(T)
{
  return 1;
}

template <unsigned N, typename T>
std::enable_if_t<N==1,T>
power(T val)
{
  return val;
}
  
template <unsigned N, typename T>
std::enable_if_t<1<N,T>
power(T val)
{
  return val * power<N-1>(val);
}
  
/**
 * Square root.
 */
template <typename Array>
auto sqrt(const Array& a)
{
  return make_delayed<typename Array::dtype, Array::ndim>(a.dimensions(),
			   [a] (const auto& x) { return std::sqrt(a(x)); });
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
template <typename Array,
	  typename std::enable_if_t<Array::ndim!=0>* = nullptr>
auto variance(const Array& a, ArrayIndex dim, bool bessel_correction)
{
  auto avg = immediate(average(a, dim));
  return make_delayed<typename Array::dtype, Array::ndim-1>(remove_coordinate(a.dimensions(), dim),
			     [a,dim,avg,bessel_correction]
			     (const auto& x)
			     {
			       // Path in the original array
			       auto orig_path = add_coordinate(x, dim);
			       // Sum the squared deviations to the mean
			       typename Array::dtype val = 0;
			       for (ArrayIndex i = 0; i < a.dimensions()[dim]; i++) {
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
  a.map([avg,&res](auto&, auto val) {
      res += power<2>(val - avg);
    });
  return res / (bessel_correction ? a.size() - 1 : a.size());
}

template <typename Array,
	  typename std::enable_if_t<Array::ndim!=0>* = nullptr>
auto deviation(const Array& a, ArrayIndex dim, bool bessel_correction)
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
  return make_delayed<typename Array::dtype, Array::ndim>(a.dimensions(),
							  [a,value=static_cast<typename Array::dtype>(value)]
							  (const auto& coords) { return std::max(a(coords), value); });
  }

/**
 * Non-normalized generalized Gaussian function with integral beta.
 */
template <unsigned beta, typename Array,
	  typename std::enable_if_t<std::is_floating_point<typename Array::dtype>::value>* = nullptr>
auto ggd(const Array& a, typename Array::dtype alpha, typename Array::dtype mu=0)
{
  return make_delayed<typename Array::dtype, Array::ndim>(a.dimensions(),
			   [a,alpha,mu]
			   (const auto& coords) {
			     return std::exp(-power<beta>(std::fabs(a(coords)-mu)/alpha));
			   });
  
}

template <typename Array>
auto norm_angle_diff(const Array& a)
{
  return make_delayed<typename Array::dtype, Array::ndim>(a.dimensions(),
			   [a](const auto& coords) {
							    typename Array::dtype x = a(coords);
			     if (x >= -180 && x <= 180)
			       return x;
			     if (x > 180)
			       return 360 - x;
			     else // x < -180
			       return -x-360;
			   });
}

} // namespace delayed
} // namespace cuiloa

// Local Variables:
// mode: c++
// End:
