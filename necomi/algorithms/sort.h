// necomi/algorithms/sort.h – Sorting algorithms
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <algorithm>
#include <iostream>
#include <string>

#include "../arrays/stridedarray.h"
#include "../arrays/delayed.h"

namespace necomi
{

template <typename T, std::size_t N>
StridedArray<std::array<std::size_t,N>,N> sort_indices(const StridedArray<T,N>& a)
{
  StridedArray<std::array<std::size_t,N>,N> indices(a.dims());
  indices.map([](const auto& coords, auto& val) { val = coords; });
  std::sort(indices.begin(), indices.end(),
	    [&a](auto i, auto j) { return a(i) < a(j); });
  return indices;
}

template <typename Array1, typename Array2,
	  std::enable_if_t<is_indexable<Array1>::value
			   && is_indexable<Array2>::value>* = nullptr>
auto permute(const Array1& a, const Array2& idx)
{
  static_assert(Array1::ndim == Array2::ndim,
		"array to be permuted and its indices must have same dimensionality");
  static_assert(std::is_same<typename Array1::dims_type,
		             typename Array2::dtype>::value,
		"invalid type for the permutation indices");
#ifndef NECOMI_NO_BOUND_CHECKS
  if (a.dims() != idx.dims())
    throw std::length_error("array to be permuted and its indices must have the same dimensions");
#endif

  return make_delayed<typename Array1::dtype,Array1::ndim>(a.dims(),
							   [a,idx](const auto& coords) {
							     return a(idx(coords));
							   });
}

} // namespace necomi

// Local Variables:
// mode: c++
// End:

