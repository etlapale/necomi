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
  
} // namespace necomi

// Local Variables:
// mode: c++
// End:
