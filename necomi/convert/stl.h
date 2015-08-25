// necomi/convert/stl.h – STL convertion tools
//
// Copyright © 2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <vector>

#include "../arrays/stridedarray.h"

namespace necomi {

/// Create a 1D strided array containing a std::vector copy.
template <typename T>
StridedArray<T,1> from_vector(const std::vector<T>& vec)
{
  StridedArray<T,1> arr(vec.size());
  std::copy(vec.cbegin(), vec.cend(), arr.begin());
  return arr;
}

template <typename T>
std::vector<T> to_vector(const StridedArray<T,1>& a)
{
  std::vector<T> vec;
  for (auto i = 0UL; i < a.dim(0); i++)
    vec.push_back(a(i));
  return vec;
}
  
} // namespace necomi

// Local Variables:
// mode: c++
// End:
