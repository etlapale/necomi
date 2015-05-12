// necomi/core/coordinates.h – Coordinates manipulation
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <array>
#include <functional>
#include <numeric>

namespace necomi
{
  /**
   * Remove a dimension or coordinate.
   */
  template <std::size_t N, typename std::enable_if_t<N!=0>* = nullptr>
  std::array<std::size_t,N-1> remove_coordinate(const std::array<std::size_t,N>& coords,
				     std::size_t dim)
  {
    std::array<std::size_t,N-1> c;
    
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
  template <std::size_t N>
  std::array<std::size_t,N+1> add_coordinate(const std::array<std::size_t,N>& coords,
				  std::size_t dim)
  {
    std::array<std::size_t,N+1> c;
    
    auto oit = std::copy_n(coords.cbegin(), dim, c.begin());
    if (dim != N)
      std::copy(coords.cbegin()+dim, coords.cend(), oit+1);
    
    return c;
  }
  
  template <std::size_t N>
  std::array<std::size_t,N+1> add_coordinate(const std::array<std::size_t,N>& coords,
				  std::size_t dim, std::size_t value)
  {
    auto c = add_coordinate(coords, dim);
    c[dim] = value;
    return c;
  }
  
  template <std::size_t N>
  std::array<std::size_t,N+1> append_coordinate(const std::array<std::size_t,N>& coords,
				     std::size_t value)
  {
    return add_coordinate(coords, N, value);
  }
  
  template <std::size_t N>
  std::array<std::size_t,N+1> prepend_coordinate(const std::array<std::size_t,N>& coords,
				      std::size_t value)
  {
    return add_coordinate(coords, 0, value);
  }

template <std::size_t N>
std::array<std::size_t,N> change_coordinate(const std::array<std::size_t,N>& coords,
				 std::size_t dim, std::size_t value)
{
  std::array<std::size_t,N> c = coords;
  c[dim] = value;
  return c;
}

} // namespace necomi

// Local Variables:
// mode: c++
// End:
