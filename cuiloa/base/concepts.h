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

#include "traits.h"

/**
 * \defgroup Concepts Abstract library interfaces
 * Define checkers for the concepts used throughout the library.
 * @{
 */

namespace cuiloa
{
  // Define had_dtype and has_ndim
  CUILOA_MAKE_HAS_TYPE_FIELD(dtype)
  
  template <typename T, typename = void>
  struct has_ndim : std::false_type {};
  
  template <typename T>
  struct has_ndim<T, typename enable_if_type<enable_if_dimension<T::ndim>>::type>
    : std::true_type {};
  
  /**
   * Check if a given type is a cuiloa array.
   * A cuiloa array must have a compile-time element type, name dtype.
   * A minimal array class might look like:
   *
   * \code{.cpp}
   * struct MyArray {
   *   using dtype = double;
   *   using ndim = ndim;
   * };
   * \endcode
   *
   * \note The standard library define a similar function
   *       for standard array types.
   */
  template <typename T>
  struct is_array
    : std::integral_constant<bool,
          has_dtype<T>::value && has_ndim<T>::value>
  {
  };
  
  /**
   * Check if individual array elements are accessible.
   *
   * An indexable array provides a Coordinates-based access to its
   * elements in the form of an () operator whose return type must
   * be convertible to its element type.
   */
  template <typename T>
  struct is_indexable
    : std::integral_constant<bool,
			     is_array<T>::value>
  {};
} // namespace cuiloa

/**@}*/

// Local Variables:
// mode: c++
// End:

