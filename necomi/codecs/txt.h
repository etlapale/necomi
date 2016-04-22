// necomi/codecs/txt.h – Simple text codec
//
// Copyright © 2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <fstream>
#include <iterator>
#include <ostream>

#include "../traits/arrays.h"

namespace necomi {

template <typename Array,
	  typename std::enable_if_t<is_indexable<Array>::value>* = nullptr>
void savetxt(const std::string& path, const Array& a)
{
  static_assert(Array::ndim == 1, "savetxt is only available for 1D arrays");
  
  std::ofstream of(path);
  for (auto i = 0UL; i < a.dim(0); i++)
    of << a(i) << ' ';
  of << std::endl;
}

} // namespace necomi

// Local Variables:
// mode: c++
// End:

