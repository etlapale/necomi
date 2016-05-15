// necomi/traits/arrays.h – Compile-time array type checks
//
// Copyright © 2016 Émilien Tlapale
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include "generic.h"

namespace necomi {

// Define had_dtype and has_ndim
NECOMI_MAKE_HAS_TYPE_FIELD(dtype)
  
template <typename T, typename = void>
struct has_ndim : std::false_type {};

//template <std::size_t N>
//struct enable_if_dimension {};
    
template <typename T>
//struct has_ndim<T, typename enable_if_type<enable_if_dimension<T::ndim>>::type>
struct has_ndim<T, decltype(&T::ndim, void())>
  : std::true_type {};
  
template <typename T, typename = void>
struct has_dims : std::false_type {};
  
template <typename T> struct has_dims<T, decltype(&T::dims, void())>
  : std::true_type {};

template <typename T, typename = void>
struct has_strides : std::false_type {};
  
template <typename T> struct has_strides<T, decltype(&T::strides, void())>
  : std::true_type {};



template <typename T>
struct element_type
{
  using type = std::result_of_t<T(const typename T::dims_type&)>;
};

template <typename T>
using element_type_t = typename element_type<T>::type;



template <typename T>
struct is_array
  : std::integral_constant<bool,
			   has_dtype<T>::value
			   && has_ndim<T>::value
			   && has_dims<T>::value>
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

template <typename T, typename = void>
struct is_indexable : std::false_type {};
  
template <typename T>
struct is_indexable<T, std::enable_if_t<is_array<T>::value,void>>
  : std::integral_constant<bool,
			   is_callable<T,const typename T::dims_type&>::value>
{};

template <typename T, typename = void>
struct is_modifiable : std::false_type {};

template <typename T>
struct is_modifiable<T, std::enable_if_t<is_indexable<T>::value,void>>
  : std::integral_constant<bool,
			   std::is_convertible<typename std::result_of<T(const typename T::dims_type&)>::type, typename T::dtype&>::value>
			   {};

} // namespace necomi

// Local Variables:
// mode: c++
// End:

