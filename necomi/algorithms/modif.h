// necomi/delayed/maps.h – Apply functions to all elements
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include "../core/loops.h"
#include "../traits/arrays.h"

namespace necomi {

template <typename Array, typename Function,
	  std::enable_if_t<is_modifiable<Array>::value>* = nullptr>
auto transform(Array& a, Function f)
{
  for_each(a, [f](const auto&, auto& val){ val = f(val); });
  return a;
}
 
} // namespace necomi

// Local Variables:
// mode: c++
// End:
