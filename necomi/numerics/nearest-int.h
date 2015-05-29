// necomi/numerics/nearest-int.h – Nearest integer floating point ops
//
// Copyright © 2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <cmath>
#include <type_traits>

#include "../arrays/delayed.h"

namespace necomi {

template <typename Array,
	  typename std::enable_if_t<is_indexable<Array>::value
				    && std::is_floating_point<typename Array::dtype>::value>* = nullptr>
auto ceil(const Array& a)
{
  return make_delayed(a.dims(), [a](const auto& coords)
		      { return std::ceil(a(coords)); });
}


template <typename Array,
	  typename std::enable_if_t<is_indexable<Array>::value
				    && std::is_floating_point<typename Array::dtype>::value>* = nullptr>
auto floor(const Array& a)
{
  return make_delayed(a.dims(), [a](const auto& coords)
		      { return std::floor(a(coords)); });
}


template <typename Array,
	  typename std::enable_if_t<is_indexable<Array>::value
				    && std::is_floating_point<typename Array::dtype>::value>* = nullptr>
auto round(const Array& a)
{
  return make_delayed(a.dims(), [a](const auto& coords)
	              { return std::round(a(coords)); });
}

} // namespace necomi

// Local Variables:
// mode: c++
// End:
