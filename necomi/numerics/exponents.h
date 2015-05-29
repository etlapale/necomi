// necomi/numerics/exponents.h – Exponents and logarithms
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include "../arrays/delayed.h"

namespace necomi {

template <typename Array>
auto exp(const Array& a)
{
  return make_delayed<typename Array::dtype,Array::ndim>(a.dims(),
							 [a] (auto& x) {
							   return std::exp(a(x));
							 });
}
  
template <unsigned N, typename T,
	  std::enable_if_t<N==0>* = nullptr>
auto power(T)
{
  return 1;
}

template <unsigned N, typename T,
	  std::enable_if_t<N==1>* = nullptr>
auto power(T val)
{
  return val;
}
  
template <unsigned N, typename T,
	  std::enable_if_t<1<N>* = nullptr>
auto power(T val)
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


template <unsigned beta, typename Array1, typename Array2,
	  typename std::enable_if_t<std::is_floating_point<typename Array1::dtype>::value
				    && std::is_floating_point<typename Array2::dtype>::value>* = nullptr>
auto ggd(const Array1& a, const Array2& alpha, typename Array1::dtype mu=0)
{
  /*return make_delayed<typename Array::dtype, Array::ndim>(a.dims(),
			   [a,alpha,mu]
			   (const auto& coords) {
			     return std::exp(-power<beta>(std::fabs(a(coords)-mu)/alpha));
			     });*/
  
  using namespace necomi::broadcasting;
  //auto num = abs(a-mu);
  //auto foo = num  / alpha;
  //return a;
  return exp(-power<beta>(abs(a - mu)/alpha));
}


template <typename Array>
auto norm_angle_diff(const Array& a)
{
  // TODO: Rewrite with fmod, remainder or rem
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

template <typename T,
	  std::enable_if_t<std::is_floating_point<T>::value>* = nullptr>
T gaussian(T x, T mu=0, T sigma=1)
{
  return std::exp(-power<2>(x-mu)/(2*power<2>(sigma)))
    / (sigma*std::sqrt(2*M_PI));
}


template <typename Array,
	  typename std::enable_if_t<std::is_floating_point<typename Array::dtype>::value>* = nullptr>
auto gaussian(const Array& a, typename Array::dtype mu=0, typename Array::dtype sigma=1)
{
  return make_delayed(a.dims(), [a,mu,sigma](const auto& coords) {
      return gaussian(a(coords), mu, sigma);
    });
}


template <typename T, typename U, typename V>
auto gaussian(T x, U mu, V sigma)
{
  using namespace std;
  using namespace necomi;

  return exp(-power<2>(x-mu)/(2*power<2>(sigma)))
    / (sigma*std::sqrt(2*M_PI));
}



} // namespace necomi

// Local Variables:
// mode: c++
// End:
