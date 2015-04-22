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



  template <typename Concrete, typename T, ArrayIndex N>
  auto exp(const AbstractArray<Concrete,T,N>& a)
  {
    return make_delayed<T,N>(a.dimensions(),
			     [a=a.shallow_copy()]
      (auto& path) { return std::exp(a(path)); });
  }

  /**
   * Sum an array across a given dimension.
   */
  template <typename Concrete, typename T, ArrayIndex N,
	    typename std::enable_if<N!=0>::type* = nullptr>
  auto sum(const AbstractArray<Concrete,T,N>&a, ArrayIndex dim)
  {
    return make_delayed<T,N-1>(remove_coordinate(a.dimensions(), dim),
      [a=a.shallow_copy(),dim] (auto& path) {
        // Path in the original array
	auto orig_path = add_coordinate(path, dim);
	// Sum all the elements in the dimension
	T val = 0;
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
  template <typename Concrete, typename T, ArrayIndex N,
	    typename std::enable_if_t<N!=0>* = nullptr>
  auto average(const AbstractArray<Concrete,T,N>& a, ArrayIndex dim)
  {
    return sum(a,dim) / static_cast<T>(a.dimensions()[dim]);
  }

/**
 * Average across all dimensions.
 */
template <typename Concrete, typename T, ArrayIndex N,
	  typename std::enable_if_t<N!=0>* = nullptr>
auto average(const AbstractArray<Concrete,T,N>& a)
{
  return sum(a) / static_cast<T>(a.size());
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
  template <typename Concrete, typename T, ArrayIndex N>
  auto sqrt(const AbstractArray<Concrete,T,N>& a)
  {
    return make_delayed<T,N>(a.dimensions(),
			     [a=a.shallow_copy()]
      (auto& path) { return std::sqrt(a(path)); });
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
  template <typename Concrete, typename T, ArrayIndex N,
	    typename std::enable_if_t<N!=0>* = nullptr>
  auto variance(const AbstractArray<Concrete,T,N>& a, ArrayIndex dim,
		bool bessel_correction)
  {
    auto avg = immediate(average(a, dim));
    return make_delayed<T,N-1>(remove_coordinate(a.dimensions(), dim),
      [a=a.shallow_copy(),dim,avg=avg.shallow_copy(),bessel_correction]
      (auto& path)
      {
        // Path in the original array
	auto orig_path = add_coordinate(path, dim);
	// Sum the squared deviations to the mean
	T val = 0;
	for (ArrayIndex i = 0; i < a.dimensions()[dim]; i++) {
          orig_path[dim] = i;
	  val += power<2>(a(orig_path) - avg(path));
        }
	return val / (bessel_correction ? a.dim(dim) - 1 : a.dim(dim));
      });
  }

template <typename Concrete, typename T, ArrayIndex N,
	  typename std::enable_if_t<N!=0>* = nullptr>
auto variance(const AbstractArray<Concrete,T,N>& a,
	      bool bessel_correction)
{
  static_assert(std::is_floating_point<T>::value,
		"statistics on arrays require floating elements");
  auto avg = average(a);
  T res = 0;
  a.map([avg,&res](auto&, auto val) {
      res += power<2>(val - avg);
    });
  return res / (bessel_correction ? a.size() - 1 : a.size());
}

  template <typename Concrete, typename T, ArrayIndex N,
	    typename std::enable_if_t<N!=0>* = nullptr>
  auto deviation(const AbstractArray<Concrete,T,N>& a, ArrayIndex dim,
		 bool bessel_correction)
  {
    return sqrt(variance(a, dim, bessel_correction));
  }

template <typename Concrete, typename T, ArrayIndex N,
	  typename std::enable_if_t<N!=0>* = nullptr>
auto deviation(const AbstractArray<Concrete,T,N>& a,
	       bool bessel_correction)
{
  return std::sqrt(variance(a, bessel_correction));
}


  template <typename Concrete, typename T, ArrayDimension N, typename U,
	    typename std::enable_if_t<is_promotable<U,T>::value>* = nullptr>
  auto max(const AbstractArray<Concrete,T,N>& a, U value)
  {
    return make_delayed<T,N>(a.dimensions(),
			     [a=a.shallow_copy(),value=static_cast<T>(value)]
			     (auto& coords) { return std::max(a(coords), value); });
  }

/**
 * Non-normalized generalized Gaussian function with integral beta.
 */
template <unsigned beta, typename T, typename Concrete,
	  typename std::enable_if_t<std::is_floating_point<T>::value>* = nullptr>
auto ggd(const AbstractArray<Concrete,T,1>& a, T alpha, T mu=0)
{
  return make_delayed<T,1>(a.dimensions(),
			   [a=a.shallow_copy(),alpha,mu]
			   (auto& coords) {
			     return std::exp(-power<beta>(std::fabs(a(coords)-mu)/alpha));
			   });
  
}

template <typename T, ArrayDimension N, typename Concrete>
auto norm_angle_diff(const AbstractArray<Concrete,T,N>& a)
{
  return make_delayed<T,N>(a.dimensions(),
			   [a=a.shallow_copy()](auto& coords) {
			     T x = a(coords);
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
