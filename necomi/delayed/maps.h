// necomi/delayed/maps.h – Apply functions to all elements
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include "../arrays/delayed.h"

namespace necomi {

template <typename Array, typename Function,
	  std::enable_if_t<is_indexable<Array>::value>* = nullptr>
auto map(const Array& a, Function f)
{
  // Create a new delayed array mapping the function to each element
  return make_delayed(a.dims(), [a,f](const auto& coords) {
      return f(a(coords));
    });
}

} // namespace necomi

// Local Variables:
// mode: c++
// End:
