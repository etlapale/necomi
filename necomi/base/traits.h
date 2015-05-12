// necomi/base/traits.h – Basic data types
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <type_traits>

namespace necomi {

  /**
   * Single coordinate component.
   */
  using ArrayIndex = std::size_t;

  /**
   * Single dimension component.
   */
  using ArrayDimension = std::size_t;

  /**
   * Coordinates to access an element in an array.
   */
  template <ArrayIndex N> using Coordinates = std::array<ArrayIndex,N>;

  /**
   * Dimensions on a multidimensional array.
   */
  template <ArrayDimension N> using Dimensions = std::array<ArrayDimension,N>;

  /**
   * Checks if a pack of types are all valid array indexes.
   */
  template <typename ...Indices>
  struct all_indices;

  template <>
  struct all_indices<> : std::true_type
  {};

  template <typename Index, typename ...Indices>
  struct all_indices<Index, Indices...>
    : std::integral_constant<bool,
			     std::is_convertible<Index, ArrayIndex>::value &&
			     all_indices<Indices...>::value>
  {};


/**
 * Utility class to get an element amongst heterogeneous arrays.  Used
 * to get the element at coordinates c of the i-th array, you can use
 * \c choose_array<0,Array,Arrays...>::at(i, c, a, as), where the
 * arrays are denoted by a, as..., with respective types Array,
 * Arrays.... The 0 is the mandatory index to start the search.  A
 * simpler interface is provided by stack() which creates a delayed
 * array from a list of arrays.
 */
template <std::size_t, typename...>
struct choose_array;

template <std::size_t I, typename Array, typename... Arrays>
struct choose_array<I, Array, Arrays...>
{
  static double at(std::size_t n, const Coordinates<Array::ndim>& coords,
		   const Array& a, const Arrays&... as)
  {
    if (n == I)
      return a(coords);
    else
      return choose_array<I+1, Arrays...>::at(n, coords, as...);
  }
};

template <std::size_t I>
struct choose_array<I>
{
  template <ArrayDimension N>
  static double at(std::size_t, const Coordinates<N>&)
  { throw std::range_error("invalid array chosen"); }
};


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

/**
 * Undefined template to elicit an error message to debug types.
 */
template <typename T> class DebugType;

} // namespace necomi

// Local variables:
// mode: c++
// End:
