// necomi/arrays/stridedarray.h – In memory shared strided array
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <functional>
#include <numeric>

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

} // namespace necomi

// Local Variables:
// mode: c++
// End:
