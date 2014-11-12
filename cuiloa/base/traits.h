/*
 * Copyright © 2014 University of California, Irvine
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <type_traits>

namespace cuiloa {

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
   * Checks if a type can be promoted to another type.
   */
  template <typename From, typename To>
  struct is_promotable
    : std::integral_constant<bool,
			     std::is_convertible<From,To>::value
			     && ! (std::is_floating_point<From>::value
				   && std::is_integral<To>::value)> {};


  template <typename To, typename ...From>
  struct all_convertible;

  template <typename To>
  struct all_convertible<To> : std::true_type
  {};

  template <typename To, typename From, typename ...Froms>
  struct all_convertible<To, From, Froms...>
    : std::integral_constant<bool,
			     std::is_convertible<From, To>::value &&
			     all_convertible<To, Froms...>::value>
  {};

} // namespace cuiloa

// Local variables:
// mode: c++
// End:
