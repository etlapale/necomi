// necomi/delayed/ranges.h – Delayed arrays from constants and ranges
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <array>
#include <cstdlib>

#include "../arrays/delayed.h"

namespace necomi {

/**
 * Create an array filled with a constant value.
 */
template <std::size_t N=1, typename T>
auto constants(const std::array<std::size_t,N>& dims, T value)
{
  return make_delayed(dims, [value](auto&){ return value; });
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
  std::array<std::size_t,sizeof...(Dims)> d{static_cast<std::size_t>(dims)...};
  return constants(d, static_cast<T>(0));
}

/**
 * Create an array with the same dimensions filled with a constant
 * value.
 * \see zeros_like
 */
template <typename T, typename Array>
auto constants_like(const Array& a, T value)
{
  return make_delayed(a.dims(), [value](const auto&){ return value; });
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
auto range(T start, T stop, T step=1)
{
#ifndef NECOMI_NO_BOUND_CHECKS
  if (stop <= start)
    throw std::out_of_range("stop must be greater than start for ranges");
  if (step <= 0)
    throw std::out_of_range("step must be positive for ranges");
#endif
  auto size = static_cast<std::size_t>(std::ceil(static_cast<double>(stop-start)/step));
  return make_delayed(std::array<std::size_t,1>{size},
                      [start,step](const auto& coords) {
                        return static_cast<T>(start+step*coords[0]);
                      });
}


template <typename T>
auto range(T stop)
{
  std::array<std::size_t,1> dims{static_cast<std::size_t>(stop)};
  return make_delayed(dims, [](const auto& coords){
      return static_cast<T>(coords[0]);
    });
}


/**
 * Return an array of evenly spaced floating point numbers.
 * If the type of the boundaries is floating, the resulting array elements
 * will be of that type. Otherwise, they will be double values.
 */
template <typename T>
auto linspace(T start, T stop, std::size_t size, bool endpoint=true)
{
  using U = typename std::conditional<std::is_floating_point<T>::value, T, double>::type;
  auto step = static_cast<U>(stop - start)/(endpoint ? size - 1 : size);
  return make_delayed(std::array<size_t,1>{size},
                      [start,step](const auto& coords)
                      { return static_cast<U>(start+step*coords[0]); });
}

/**
 * Create an identity matrix.
 */
template <typename T=double>
auto identity(std::size_t dim)
{
  // TODO: generalize (prod(coords) = 1)
  return make_delayed(std::array<std::size_t,2>{dim,dim},
                      [](const auto& coords) {
                        return coords[0] == coords[1];
                      });
}

template <typename T>
struct Range
{
  Range(T end)
    : Range(0, end)
  {}
  
  Range(T start, T end)
    : Range(start, end, 1)
  {}
  
  Range(T s, T e, T p)
    : start(s), end(e), step(p)
  {}
  
  T start;
  T end;
  T step;
};

template <typename T=double, typename ...Ranges>
auto meshgrid(Range<T> range, Ranges... ranges)
{
  constexpr std::size_t N = 1 + sizeof...(Ranges);

  // Dimensions of each of the resulting arrays
  auto size = [](Range<T> rg)
    { return static_cast<std::size_t>((rg.end-rg.start) / rg.step); };
  std::array<std::size_t,N> dims {size(range), size(ranges)...};

  // Function to create each coordinate array individually
  std::size_t i = 0;	// With a state (order guaranteed)
  auto builder = [&dims,&i](const Range<T>& rg) {
    auto a = make_delayed(dims, [i,&rg](const auto& coords) {
	auto x = static_cast<T>(coords[i]);
	return (x + rg.start) * rg.step;
      });
    i++;
    return strided_array(a);
  };

  return std::make_tuple(builder(range), builder(ranges)...);
}

} // namespace necomi

// Local Variables:
// mode: c++
// End:
