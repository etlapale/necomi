// necomi/core/mpl.h – Generic array metaprogramming
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

namespace necomi {

/**
 * Utility class to get an element amongst heterogeneous arrays.  Used
 * to get the element at coordinates c of the i-th array, you can use
 * \c choose_array<0,Array,Arrays...>::at(i, c, a, as), where the
 * arrays are denoted by a, as..., with respective types Array,
 * Arrays.... The 0 is the mandatory index to start the search.  A
 * simpler interface is provided by stack() which creates a delayed
 * array from a list of arrays.
 */
template <std::size_t, typename...>
struct choose_array;

template <std::size_t I, typename Array, typename... Arrays>
struct choose_array<I, Array, Arrays...>
{
  using dims_type = typename Array::dims_type;
  
  static double at(std::size_t n, const dims_type& coords,
		   const Array& a, const Arrays&... as)
  {
    if (n == I)
      return a(coords);
    else
      return choose_array<I+1, Arrays...>::at(n, coords, as...);
  }
};

template <std::size_t I>
struct choose_array<I>
{
  template <std::size_t N>
  static double at(std::size_t, const std::array<std::size_t,N>&)
  { throw std::range_error("invalid array chosen"); }
};

template <typename T>
class DebugType;

} // namespace necomi

// Local Variables:
// mode: c++
// End:
