// necomi/traits/shape.h – Compile-time shape checks
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <type_traits>

namespace necomi {

/**
 * Indicate whether multiple arrays all have the same dimensionality.
 */
template <typename Array, typename...>
struct same_dimensionality;

template <typename Array1, typename Array2, typename... Arrays>
struct same_dimensionality<Array1, Array2, Arrays...>
  : std::integral_constant<bool, Array1::ndim == Array2::ndim
			   && same_dimensionality<Array2, Arrays...>::value>
{};

template <typename Array>
struct same_dimensionality<Array> : std::true_type {};


/**
 * Indicate whether multiple arrays all have the same dimensions.
 */
template <typename Array1, typename Array2, typename... Arrays>
static std::enable_if_t<Array1::ndim == Array2::ndim, bool>
same_dimensions(const Array1& a, const Array2& b, const Arrays&... as)
{
  return a.dims() == b.dims() && same_dimensions(b, as...);
}

template <typename Array1, typename Array2, typename... Arrays>
static std::enable_if_t<Array1::ndim != Array2::ndim, bool>
same_dimensions(const Array1&, const Array2&, const Arrays&...)
{
  return false;
}

template <typename Array>
bool same_dimensions(const Array&)
{
  return true;
}

/**
 * Indicate whethere multiple arrays have the same dimensions except one.
 */
template <typename Array1, typename Array2, typename... Arrays>
static std::enable_if_t<Array1::ndim == Array2::ndim, bool>
almost_same_dimensions(std::size_t idx, const Array1& a, const Array2& b, const Arrays&... as)
{
  for (auto i = 0UL; i < Array1::ndim; i++)
    if (idx != i && a.dims()[i] != b.dims()[i])
      return false;
  return same_dimensions(b, as...);
}

template <typename Array1, typename Array2, typename... Arrays>
static std::enable_if_t<Array1::ndim != Array2::ndim, bool>
almost_same_dimensions(std::size_t, const Array1&, const Array2&, const Arrays&...)
{
  return false;
}

template <typename Array>
bool almost_same_dimensions(std::size_t, const Array&)
{
  return true;
}

} // namespace necomi

// Local variables:
// mode: c++
// End:
