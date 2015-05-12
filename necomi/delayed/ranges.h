// necomi/delayed/ranges.h – Delayed arrays from constants and ranges
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include "../arrays/delayed.h"

namespace necomi {

/**
 * Create an array filled with a constant value.
 */
template <typename T=double, std::size_t N=1,
	  typename dims_type = std::array<std::size_t,N>>
auto constants(const dims_type& dims, T value)
{
  return make_delayed<T>(dims, [value](auto&){ return value; });
}
  
template <typename T=double, std::size_t N,
	  typename dims_type = std::array<std::size_t,N>>
auto zeros(const dims_type& dims)
{
  return constants<T,N>(dims,0);
}

template <typename T=double,
	  typename ...Dims,
	  typename std::enable_if<all_convertible<Dims...,std::size_t>::value>* = nullptr>
auto zeros(Dims... dims)
{
  return constants<T,sizeof...(Dims)>({static_cast<std::size_t>(dims)...}, 0);
}

/**
 * Create an array with the same dimensions filled with a constant
 * value.
 * \see zeros_like
 */
template <typename T, typename Array>
auto constants_like(const Array& a, const T&& value)
{
  return make_delayed<T>(a.dims(),
			 [value](const auto&){ return value; });
}

/**
 * Create an array with the same dimensions filled with zero values.
 * \see constants_like
 */
template <typename Array>
auto zeros_like(const Array& a)
{
  return constants_like<typename Array::dtype>(a, 0);
}

/**
 * Create an array with the same dimensions filled with zero values.
 * \see constants_like
 */
template <typename T, typename Array>
auto zeros_like(const Array& a)
{
  return constants_like<T,Array>(a, 0);
}

template <typename T>
auto range(T stop)
{
  return make_delayed<T,1>({{static_cast<std::size_t>(stop)}},
			   [](const auto& coords){ return coords[0]; });
}

template <typename T>
auto range(T start, T stop, T step=1)
{
#ifndef NECOMI_NO_BOUND_CHECKS
  if (stop <= start)
    throw std::out_of_range("stop must be greater than start for ranges");
  if (step <= 0)
    throw std::out_of_range("step must be positive for ranges");
#endif
  auto size = static_cast<std::size_t>(std::ceil(static_cast<double>(stop-start)/step));
  return make_delayed<T,1>({size},
			   [start,step](auto& coords)
			   { return start+step*coords[0]; });
}
  
/**
 * Return an array of evenly spaced floating point numbers.
 * If the type of the boundaries is floating, the resulting array elements
 * will be of that type. Otherwise, they will be double values.
 */
template <typename T,
	  typename U=typename std::conditional<std::is_floating_point<T>::value,
					       T,double>::type,
	  std::enable_if_t<std::is_convertible<T,U>::value>* = nullptr>
auto linspace(T start, T stop, std::size_t size, bool endpoint=true)
{
  auto step = static_cast<U>(stop - start)/(endpoint ? size - 1 : size);
  return make_delayed<U,1>({size},
			   [start,step](auto& coords)
			   { return start+step*coords[0]; });
}

/**
 * Create an identity matrix.
 */
template <typename T=double>
auto identity(std::size_t dim)
{
  // TODO: generalize (prod(coords) = 1)
  return make_delayed<T,2>({dim,dim}, [](auto coords) {
      return coords[0] == coords[1];
    });
}

} // namespace necomi

// Local Variables:
// mode: c++
// End:
