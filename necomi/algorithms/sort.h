// necomi/core/sort.h – Sorting algorithms
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <algorithm>
#include <iostream>

#include "../arrays/stridedarray.h"

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

} // namespace necomi

// Local Variables:
// mode: c++
// End:

