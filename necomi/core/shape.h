// necomi/arrays/stridedarray.h – In memory shared strided array
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <numeric>
#include <vector>

namespace necomi {

/**
 * Compute the size of an array (product of dimensions).
 */
template <typename Array, typename dim_type=typename Array::dim_type>
dim_type size(const Array& a)
{
  return std::accumulate(a.dims().cbegin(), a.dims().cend(), 1,
			 std::multiplies<>());
}

template <typename T, std::size_t N, typename Container>
std::array<T,N> to_array(const Container& c)
{
#ifndef NECOMI_NO_BOUND_CHECKS
  if (c.size() != N)
    throw std::length_error("cannot convert a size N container to a size M array with N!=M");
#endif
  std::array<T,N> res;
  std::copy_n(c.cbegin(), N, res.begin());
  return res;
}

template <typename T, typename Container>
std::vector<T> to_vector(const Container& c)
{
  std::vector<T> res;
  res.resize(c.size());
  std::copy_n(c.cbegin(), c.size(), res.begin());
  return res;
}

} // namespace necomi

// Local Variables:
// mode: c++
// End:
