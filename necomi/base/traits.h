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
 * Undefined template to elicit an error message to debug types.
 */
template <typename T> class DebugType;

} // namespace necomi

// Local variables:
// mode: c++
// End:
