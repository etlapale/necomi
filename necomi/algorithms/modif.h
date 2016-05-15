// necomi/algorithms/modif.h – Apply functions to all elements
//
// Copyright © 2016 Émilien Tlapale
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include "../core/loops.h"
#include "../traits/arrays.h"


namespace necomi {

template <typename Array, typename Function,
	  std::enable_if_t<is_modifiable<Array>::value>* = nullptr>
Array& transform(Array& a, Function f)
{
  for_each(a, [f](const auto&, auto& val){ val = f(val); });
  return a;
}


template <typename Array1, typename Array2, typename Function,
	  std::enable_if_t<Array1::ndim() == Array2::ndim()
			   && is_modifiable<Array1>::value
			   && is_indexable<Array2>::value>* = nullptr>
Array1& transform(Array1& a, const Array2& b, Function f)
{
  
#ifndef NECOMI_NO_BOUND_CHECKS
  if (a.dims() != b.dims())
    throw std::length_error("cannot transform arrays of different dimensions");
#endif
  
  for_each(a, [&b,f](const auto& coords, auto& val) mutable {
      val = f(val, b(coords));
    });
  return a;
}


} // namespace necomi

// Local Variables:
// mode: c++
// End:
