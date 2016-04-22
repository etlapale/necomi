// necomi/numerics/interpolation.h – Discretizations and interpolations
//
// Copyright © 2016 Émilien Tlapale
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <algorithm>

#include "../arrays/stridedarray.h"
#include "../delayed/ranges.h"

namespace necomi
{
/**
 * Discretization as nearest 1D element interpolation.
 */
template <typename T, typename Func>
class Discretization
{
public:
  Discretization(T min, T max, std::size_t size, Func func)
    : m_min(min)
    , m_max(max)
    , m_data(func(necomi::linspace<T>(min, max, size, true)))
  {}
  T operator()(T val) const
  {
    auto idx = static_cast<std::size_t>(0.5 + (val - m_min)/(m_max - m_min) * (size(m_data) - 1));
    //auto rect = std::max(static_cast<ArrayIndex>(0),
    //std::min(idx, m_data.size()));
    return m_data(idx);
  }
protected:
  T m_min;
  T m_max;
  necomi::StridedArray<T,1> m_data;
};
  
template <typename T, typename Func>
auto discretization(T min, T max, std::size_t size, Func func)
{
  return Discretization<T,Func>(min, max, size, func);
}
  
template <typename T>
auto discretization(T min, T max, std::size_t size)
{
  return discretization(min, max, size, [](auto&& a){ return a; });
}
  
enum class InterpolationMethod
{
  NearestNeighbor,
    Linear
    };
  
/**
 * Nearest-neighbor interpolation.
 */
template <InterpolationMethod method, typename U=double,
	  typename Array,
	  typename std::enable_if_t<
	    std::is_arithmetic<U>::value &&
	    method==InterpolationMethod::NearestNeighbor>* = nullptr>
U interpolation(const Array& a, U x)
{
  static_assert(Array::ndim() == 1,
		"nearest-neighbor interpolation only available for one-dimensional arrays");
  auto x0 = static_cast<std::size_t>(0.5 + x);
  return a(x0);
}
  
/**
 * Nearest-neighbor interpolation.
 */
template <InterpolationMethod method, typename U=double,
	  typename Array,
	  typename std::enable_if_t<
	    std::is_arithmetic<U>::value &&
	    method==InterpolationMethod::NearestNeighbor>* = nullptr>
  auto interpolation(const Array& a)
{
  static_assert(Array::ndim() == 1,
		"nearest-neighbor interpolation only available for one-dimensional arrays");
  return [a](U x) {
    auto x0 = static_cast<std::size_t>(0.5 + x);
    return a(x0);
  };
}
  
/**
 * Linear interpolation.
 */
template <InterpolationMethod method,
	  typename Array1, typename Array2,
	  typename std::enable_if_t<method==InterpolationMethod::Linear>* = nullptr>
auto interpolation(const Array1& a, const Array2& xvals)
{
  static_assert(Array2::ndim() == 1,
		"linear interpolation only available for one-dimensional arrays");
  return make_delayed<Array1::dtype,Array2::ndim()>(xvals.dimensions(),
						  [a,xvals](const auto& coords) {
						    typename Array2::dtype x = xvals(coords);
						    auto x0 = static_cast<std::size_t>(x);
						    auto y0 = a(x0);
						    auto y1 = a(x0 + 1);

						    return y0 + (y1 - y0)*(x - x0);
						  });
}
  
/**
 * Linear interpolation.
 */
template <InterpolationMethod method, typename U=double,
	  typename Array,
	  typename std::enable_if_t<
	    std::is_arithmetic<U>::value &&
	    method==InterpolationMethod::Linear>* = nullptr>
  U interpolation(const Array& a, U x)
{
  static_assert(Array::ndim() == 1,
		"linear interpolation only available for one-dimensional arrays");
  auto x0 = static_cast<std::size_t>(x);
  auto y0 = a(x0);
  auto y1 = a(x0 + 1);

  return y0 + (y1 - y0)*(x - x0);
}
  
/**
 * Linear interpolation.
 */
template <InterpolationMethod method, typename U=double,
	  typename Array,
	  typename std::enable_if_t<
	    std::is_arithmetic<U>::value &&
	    method==InterpolationMethod::Linear>* = nullptr>
  auto interpolation(const Array& a)
{
  static_assert(Array::ndim() == 1,
		"linear interpolation only available for one-dimensional arrays");
  return [a](U x) {
    auto x0 = static_cast<std::size_t>(x);
    auto y0 = a(x0);
    auto y1 = a(x0 + 1);

    return y0 + (y1 - y0)*(x - x0);
  };
}

template <typename T=double>
T rescale(T imin, T imax, T omin, T omax, T x)
{
  return (x - imin)*(omax-omin)/(imax-imin)+omin;
}

template <typename Array>
auto rescale(typename Array::dtype imin, typename Array::dtype imax,
	     typename Array::dtype omin, typename Array::dtype omax,
	     const Array& a)
{
  using T = typename Array::dtype;
  return make_delayed<T, Array::ndim()>(a.dimensions(),
				      [imin,imax,omin,omax,a](const auto& coords) {
					return rescale<T>(imin, imax, omin, omax, a(coords));
				      });
}
  
/**
 * Linear interpolation.
 */
template <InterpolationMethod method,
	  typename Array1, typename Array2,
	  typename std::enable_if_t<method==InterpolationMethod::Linear>* = nullptr>
auto scaled_interpolation(const Array1& a,
			  typename Array2::dtype xmin,
			  typename Array2::dtype xmax, const Array2& xvals)
{
  static_assert(Array1::ndim() == 1,
		"linear interpolation only available for one-dimensional arrays");
  return make_delayed<>(xvals.dims(),
			[xmin,xmax,a,xvals]
			(const auto& coords) {
			  typename Array2::dtype x = rescale<typename Array2::dtype>(xmin, xmax, 0, size(a), xvals(coords));
			  auto x0 = static_cast<std::size_t>(x);
			  auto y0 = a(x0);
			  auto y1 = a(x0 + 1);
			  
			  return y0 + (y1 - y0)*(x - x0);
			});
}
 
/**
 * Return a rescaling function mapping values in [imin,imax]
 * into values in [omin,omax].
 */
template <typename T=double>
auto rescale(T imin, T imax, T omin, T omax)
{
  return [imin,imax,omin,omax](T x) {
    return (x - imin)*(omax-omin)/(imax-imin)+omin;
  };
}
  
template <typename T=double, typename F1, typename F2>
auto compose(F1 f1, F2 f2) {
  return [f1,f2](T x) {
    return f1(f2(x));
  };
}

template <typename T=double, typename F1, typename F2, typename F3>
auto compose(F1 f1, F2 f2, F3 f3) {
  return [f1,f2,f3](T x) {
    return f1(f2(f3(x)));
  };
}

} // namespace necomi

// Local Variables:
// mode: c++
// End:
