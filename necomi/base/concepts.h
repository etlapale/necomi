// necomi/base/concepts.h – Type-level utilities
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include "traits.h"

/**
 * \defgroup Concepts Abstract library interfaces
 * Define checkers for the concepts used throughout the library.
 * @{
 */

namespace necomi
{
  // Define had_dtype and has_ndim
  NECOMI_MAKE_HAS_TYPE_FIELD(dtype)
  
  template <typename T, typename = void>
  struct has_ndim : std::false_type {};
  
  template <typename T>
  struct has_ndim<T, typename enable_if_type<enable_if_dimension<T::ndim>>::type>
    : std::true_type {};
  
  template <typename T, typename = void>
  struct has_dimensions : std::false_type {};
  
  template <typename T> struct has_dimensions<T, decltype(&T::dimensions, void())> : std::true_type {};
  
  /**
   * \anchor Array
   *
   * Check if a given type is a necomi array.
   * A necomi array must define the following types:
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
   *   enum { ndim = 1 };
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
                             // TODO: check dimensions type
  {};
  
  /**
   * Test whether two \ref Array arrays have matching dtype and ndim.
   */
  template <typename Array1, typename Array2>
  struct is_same_ndim_dtype
    : std::integral_constant<bool,
			     std::is_same<typename Array1::dtype,
					  typename Array2::dtype>::value
			     && Array1::ndim == Array2::ndim>
  {};

  /**
   * \struct necomi::is_indexable<T>
   * \anchor IndexableArray
   *
   * Check if individual array elements are accessible.
   *
   * An indexable array provides a Coordinates-based access to its
   * elements. It must also be an array satisfying the conditions of
   * \ref is_array. Elements are accessed through an <tt>operator()
   * const</tt> taking a constant \c Coordinates<ndim> parameter and
   * returning a value of the array element type (castable to \c
   * dtype).
   *
   * A minimal array class \c MyArray might be extended to an indexable
   * one by adding the adequate operator:
   * \code{.cpp}
   * struct MyIndexableArray : MyArray {
   *   dtype operator()(const Coordinates<ndim>&) const
   *   { return 42; };
   * };
   * \endcode
   * \ingroup Concepts
   *
   * \see is_array
   */
  template <typename T, typename = void>
  struct is_indexable : std::false_type {};
  
  template <typename T>
  struct is_indexable<T, std::enable_if_t<is_array<T>::value,void>>
    : std::integral_constant<bool,
			     is_callable<const T,const Coordinates<T::ndim>&>::value
			     // TODO: add a test for (x_1,x_2,…,x_ndim)
			     > {};

} // namespace necomi

/**@}*/

// Local Variables:
// mode: c++
// End:

