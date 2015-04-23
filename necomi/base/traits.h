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

  /**
   * Check if a function can be called with the given arguments.
   *
   * The static boolean field value encodes the callability.
   */
  template <typename Func, typename... Args>
  struct is_callable
  {
    template <typename T> struct dummy;

    template <typename CheckType>
    static std::true_type check(dummy<decltype(std::declval<CheckType>()(std::declval<Args>()...))> *);

    template <typename CheckType>
    static std::false_type check(...);

    enum { value = decltype(check<Func>(nullptr))::value };
  };

  template <typename, class R = void>
  struct enable_if_type { typedef R type; };
  
  template <ArrayDimension N>
  struct enable_if_dimension {};
  
  /**
   * Check that a type has a scoped type with a given name.
   *
   * You could write:
   * \code{.cpp}
   * NECOMI_MAKE_HAS_TYPE_FIELD(iterator)
   * \endcode
   *
   * And then test for the existence of the scoped type:
   * \code{.cpp}
   * constexpr bool b = has_iterator<std::vector>::value;
   * \endcode
   */
#define NECOMI_MAKE_HAS_TYPE_FIELD(field)				\
  template <typename T, typename = void>				\
  struct has_##field : std::false_type {};				\
									\
  template <typename T>							\
  struct has_##field<T,							\
		     typename enable_if_type<typename T::field>::type>	\
    : std::true_type {};


/**
 * Undefined template to elicit an error message to debug types.
 */
template <typename T> class DebugType;

} // namespace necomi

// Local variables:
// mode: c++
// End:
