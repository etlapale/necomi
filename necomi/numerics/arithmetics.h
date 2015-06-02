// necomi/numerics/arithmetics.h – Arithmetics on arrays
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <functional>

#include "../algorithms/modif.h"
#include "../traits/arrays.h"
#include "../traits/generic.h"


namespace necomi {

template <typename Array1, typename Array2,
	  std::enable_if_t<is_modifiable<Array1>::value
			   && is_indexable<Array2>::value
			   && is_promotable<typename Array2::dtype, typename Array1::dtype>::value>* = nullptr>
Array1& operator+=(Array1& a, const Array2& b)
{
  return transform(a, b, std::plus<>());
}

template <typename Array1, typename Array2,
	  std::enable_if_t<is_modifiable<Array1>::value
			   && is_indexable<Array2>::value
			   && is_promotable<typename Array2::dtype, typename Array1::dtype>::value>* = nullptr>
Array1& operator-=(Array1& a, const Array2& b)
{
  return transform(a, b, std::minus<>());
}

template <typename Array1, typename Array2,
	  std::enable_if_t<is_modifiable<Array1>::value
			   && is_indexable<Array2>::value
			   && is_promotable<typename Array2::dtype, typename Array1::dtype>::value>* = nullptr>
Array1& operator*=(Array1& a, const Array2& b)
{
  return transform(a, b, std::multiplies<>());
}

template <typename Array1, typename Array2,
	  std::enable_if_t<is_modifiable<Array1>::value
			   && is_indexable<Array2>::value
			   && is_promotable<typename Array2::dtype, typename Array1::dtype>::value>* = nullptr>
Array1& operator/=(Array1& a, const Array2& b)
{
  return transform(a, b, std::divides<>());
}

template <typename Array1, typename Array2,
	  std::enable_if_t<is_modifiable<Array1>::value
			   && is_indexable<Array2>::value
			   && is_promotable<typename Array2::dtype, typename Array1::dtype>::value>>
Array1& operator%=(Array1& a, const Array2& b)
{
  return transform(a, b, std::modulus<>());
}

template <typename Array1, typename T,
	  std::enable_if_t<is_modifiable<Array1>::value
			   && is_promotable<T, typename Array1::dtype>::value>* = nullptr>
Array1& operator/=(Array1& a, const T& val)
{
  return transform(a, [&val](const auto& x) { return x/val; });
}


} // namespace necomi

// Local Variables:
// mode: c++
// End:
