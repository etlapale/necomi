// necomi/numerics/exponents.h – Exponents and logarithms
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include "../arrays/delayed.h"

namespace necomi {

/**
 * Create a delayed array from the absolute values of another.
 * \param a	An \ref IndexableArray "indexable array".
 */
template <typename IndexableArray,
	  typename std::enable_if_t<is_indexable<IndexableArray>::value>* = nullptr>
auto abs(const IndexableArray& a)
{
  using dims_type = typename IndexableArray::dims_type;
  
  return make_delayed(a.dims(), [a](const dims_type& coords)
		      { return std::abs(a(coords)); });
}

/**
 * Norms available to function \ref norm.
 */
enum class Norm {
  /** Maximum of the absolute values. */
  Infinity
};

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
  return make_delayed<typename Array::dtype,Array::ndim>(a.dims(),
							 [a] (auto& x) {
							   return std::exp(a(x));
							 });
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
template <typename Array,
	  std::enable_if_t<is_array<Array>::value>* = nullptr>
auto sqrt(const Array& a)
{
  return make_delayed<typename Array::dtype, Array::ndim>(a.dims(),
			   [a] (const auto& x) { return std::sqrt(a(x)); });
}

/**
 * Non-normalized generalized Gaussian function with integral beta.
 */
template <unsigned beta, typename Array,
	  typename std::enable_if_t<std::is_floating_point<typename Array::dtype>::value>* = nullptr>
auto ggd(const Array& a, typename Array::dtype alpha, typename Array::dtype mu=0)
{
  return make_delayed<typename Array::dtype, Array::ndim>(a.dims(),
			   [a,alpha,mu]
			   (const auto& coords) {
			     return std::exp(-power<beta>(std::fabs(a(coords)-mu)/alpha));
			   });
  
}

template <typename Array>
auto norm_angle_diff(const Array& a)
{
  return make_delayed<typename Array::dtype, Array::ndim>(a.dims(),
			   [a](const auto& coords){
							    typename Array::dtype x = a(coords);
			     if (x >= -180 && x <= 180)
			       return x;
			     if (x > 180)
			       return 360 - x;
			     else // x < -180
			       return -x-360;
			   });
}


template <typename Array>
auto round(const Array& a)
{
  return make_delayed<typename Array::dtype, Array::ndim>(a.dims(),
			   [a](const auto& coords) {
			     return std::round(a(coords));
			   });
}
} // namespace necomi

// Local Variables:
// mode: c++
// End:
