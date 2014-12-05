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
  
  template <typename T, typename = void>
  struct has_dimensions : std::false_type {};
  
  template <typename T> struct has_dimensions<T, decltype(&T::dimensions, void())> : std::true_type {};
  
  /**
   * Check if a given type is a cuiloa array.
   * A cuiloa array must define the following types:
   * - \c dtype	Type of the elements.
   *
   * An array must also define the following compile-time constants:
   * - \c ndim	The number of dimensions in the array.
   *
   * The following runtime members must be accessible:
   * - \c dimensions	Dimensions as \c std::array<std::size_t,ndim>.
   *
   * A minimal array class definition might hence look like:
   * \code{.cpp}
   * struct MyArray {
   *   using dtype = double;
   *   static constexpr ArrayDimension ndim = 1;
   *   Dimensions<ndim> dimensions;
   * };
   * \endcode
   *
   * \note The standard library define a similar function
   *       for standard array types.
   */
  template <typename T>
  struct is_array
    : std::integral_constant<bool,
			     has_dtype<T>::value
			     && has_ndim<T>::value
			     && has_dimensions<T>::value>
  {
  };
  
  /**
   * Check if individual array elements are accessible.
   *
   * An indexable array provides a Coordinates-based access to its
   * elements. It must also be an array satisfying the conditions of
   * \ref is_array. Elements are accessed through an \c operator()
   * taking a constant \c Coordinates<ndim> parameter and returning a
   * value of the array element type (castable to \c dtype).
   */
  template <typename T, typename = void>
  struct is_indexable : std::false_type {};
  
  template <typename T>
  struct is_indexable<T, std::enable_if_t<is_array<T>::value,void>>
    : std::integral_constant<bool,
			     is_callable<T,const Coordinates<T::ndim>&>::value> {};

} // namespace cuiloa

/**@}*/

// Local Variables:
// mode: c++
// End:

