// necomi/numerics/basic.h – Basic operations
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <cmath>
#include <type_traits>

#include "../arrays/delayed.h"

namespace necomi {

/**
 * Create a delayed array from the absolute values of another.
 * \param a	An \ref IndexableArray "indexable array".
 */
template <typename Array,
	  typename std::enable_if_t<is_indexable<Array>::value>* = nullptr>
auto abs(const Array& a)
{
  return make_delayed(a.dims(), [a](const auto& coords)
		      { return std::abs(a(coords)); });
}

template <typename Array,
	  typename std::enable_if_t<is_indexable<Array>::value
				    && std::is_floating_point<typename Array::dtype>::value>* = nullptr>
auto fmod(const Array& x, typename Array::dtype y)
{
  return make_delayed(x.dims(), [x,y](const auto& coords)
	    { return std::fmod(x(coords), y); });
}

template <typename Array,
	  typename std::enable_if_t<is_indexable<Array>::value
				    && std::is_floating_point<typename Array::dtype>::value>* = nullptr>
auto remainder(const Array& x, typename Array::dtype y)
{
  return make_delayed(x.dims(), [x,y](const auto& coords)
	    { return std::remainder(x(coords), y); });
}


/**
 * Norms available to function \ref norm.
 */
enum class Norm {
  /** Maximum of the absolute values. */
  Infinity
};

template <typename IndexableArray>
auto norm(const IndexableArray& a, Norm norm)
{
  switch (norm) {
  case Norm::Infinity:
    return max(abs(a));
  }
}

} // namespace necomi

// Local Variables:
// mode: c++
// End:
