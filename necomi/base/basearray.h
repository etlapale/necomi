// necomi/base/basearray.h – Common array definitions
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <array>
#include <functional>
#include <numeric>

#include "traits.h"

namespace necomi
{
/**
* The parent class of all array types.
* Useful for managing heterogenous set of pointer to arrays.
*/
class BaseArray
{
};
  
  /**
   * Remove a dimension or coordinate.
   */
  template <ArrayIndex N, typename std::enable_if_t<N!=0>* = nullptr>
  Coordinates<N-1> remove_coordinate(const Coordinates<N>& coords,
				     ArrayIndex dim)
  {
    Coordinates<N-1> c;
    
    auto oit = std::copy_n(coords.cbegin(), dim, c.begin());
    if (dim != N-1)
      std::copy(coords.cbegin()+dim+1, coords.cend(), oit);
    
    return c;
  }

  /**
   * Add a dimension or coordinate.
   * \note The added coordinate as an unspecified initial value. To
   *       set one, use the three arguments version.
   */
  template <ArrayIndex N>
  Coordinates<N+1> add_coordinate(const Coordinates<N>& coords,
				  ArrayIndex dim)
  {
    Coordinates<N+1> c;
    
    auto oit = std::copy_n(coords.cbegin(), dim, c.begin());
    if (dim != N)
      std::copy(coords.cbegin()+dim, coords.cend(), oit+1);
    
    return c;
  }
  
  template <ArrayIndex N>
  Coordinates<N+1> add_coordinate(const Coordinates<N>& coords,
				  ArrayIndex dim, ArrayDimension value)
  {
    auto c = add_coordinate(coords, dim);
    c[dim] = value;
    return c;
  }
  
  template <ArrayIndex N>
  Coordinates<N+1> append_coordinate(const Coordinates<N>& coords,
				     ArrayDimension value)
  {
    return add_coordinate(coords, N, value);
  }
  
  template <ArrayIndex N>
  Coordinates<N+1> prepend_coordinate(const Coordinates<N>& coords,
				      ArrayDimension value)
  {
    return add_coordinate(coords, 0, value);
  }

template <ArrayIndex N>
Coordinates<N> change_coordinate(const Coordinates<N>& coords,
				 ArrayIndex dim, ArrayDimension value)
{
  Coordinates<N> c = coords;
  c[dim] = value;
  return c;
}

#ifndef IN_DOXYGEN
/**
* Final case of for loops through template metaprogramming.
*/
template <typename UnaryOperation, ArrayIndex M, typename Array>
std::enable_if_t<M==Array::ndim>
for_looper(Array& a, Coordinates<Array::ndim>& path, UnaryOperation f)
{
  f(path, a(path));
}

/**
 * Recursion case of for loops through template metaprogramming.
 */
template <typename UnaryOperation, ArrayIndex M, typename Array>
std::enable_if_t<M<Array::ndim>
for_looper(Array& a, Coordinates<Array::ndim>& path, UnaryOperation f)
{
  for (auto i = 0UL; i < a.dimensions()[M]; i++) {
    path[M] = i;
    for_looper<UnaryOperation,M+1,Array>(a, path, f);
  }
}

/**
 * Final case of for loops through template metaprogramming
 * for constant arrays.
 */
template <typename ConstMapOperation, ArrayIndex M, typename Array>
std::enable_if_t<M==Array::ndim>
const_for_looper(const Array& a,
		 Coordinates<Array::ndim>& path,
		 ConstMapOperation f)
{
  f(path, a(path));
}
  
/**
 * Recursion case of for loops through template metaprogramming.
 * for constant arrays.
 */
template <typename ConstMapOperation, ArrayIndex M, typename Array>
std::enable_if_t<M<Array::ndim>
const_for_looper(const Array& a,
		 Coordinates<Array::ndim>& path,
		 ConstMapOperation f)
{
  for (ArrayIndex i = 0; i < a.dimensions()[M]; i++) {
    path[M] = i;
    const_for_looper<ConstMapOperation,M+1,Array>(a, path, f);
  }
}

/**
 * Final case of breakable for loops through template metaprogramming
 * for constant arrays.
 */
template <typename Predicate, ArrayIndex M, typename Array>
std::enable_if_t<M==Array::ndim,bool>
breakable_for_looper(const Array& a,
		     Coordinates<Array::ndim>& path,
		     Predicate p)
{
  return p(a(path));
}

/**
 * Recursion case of for loops through template metaprogramming
 * for constant arrays.
 *
 * \ingroup core
 * \see Array::map
 */
template <typename Predicate, ArrayIndex M, typename Array>
std::enable_if_t<(M<Array::ndim),bool>
breakable_for_looper(const Array& a,
		     Coordinates<Array::ndim>& path,
		     Predicate p)
{
  for (ArrayIndex i = 0; i < a.dimensions()[M]; i++) {
    path[M] = i;
    bool ret = breakable_for_looper<Predicate,M+1,Array>(a, path, p);
    if (ret) return true;
  }
  return false;
}
#endif // IN_DOXYGEN


/**
 * CRTP base class for immediate and delayed arrays.
 * This Curiously recurring template pattern allows static
 * polymorphism to share efficient general array definitions (indexing
 * and data access).
 */
template <ArrayIndex N>
class DimArray
{
public:
  DimArray(const Dimensions<N>& dimensions)
    : m_dims(dimensions)
  {
  }

  /**
   * Dimensions of the array.
   */
  const Dimensions<N>& dimensions() const
  {
    return m_dims;
  }

  const Dimensions<N>& dims() const
  {
    return m_dims;
  }

  /**
   * Return a given dimension.
   */
  ArrayDimension dim(ArrayIndex i) const
  {
    return m_dims[i];
  }
protected:
  /// Storage for the array dimensions.
  Dimensions<N> m_dims;
};

template <typename Array, typename dim_type=typename Array::dim_type>
dim_type size(const Array& a)
{
  return std::accumulate(a.dims().cbegin(), a.dims().cend(), 1,
			 std::multiplies<>());
}


/**
 * Sum all the elements of an array.
 * \warning This may overflow.
 */
template <typename Array, typename T=typename Array::dtype>
T sum(const Array& a)
{
  T total = 0;
  a.map([&](auto&, auto val) { total += val; });
  return total;
}

/**
 * Maximum value in the array.
 */
template <typename Array, typename T=typename Array::dtype>
T max(const Array& a)
{
  Coordinates<Array::ndim> first;
  first.fill(0);
  T res = a(first);
  a.map([&res,&a](auto&, auto val) { res = val > res ? val : res; });
  return res;
}

/**
 * Minimum value in the array.
 */
template <typename Array, typename T=typename Array::dtype>
T min(const Array& a) {
  Coordinates<Array::ndim> first;
  first.fill(0);
  T res = a(first);
  a.map([&res,&a](auto&, auto val) { res = val < res ? val : res; });
  return res;
}

/**
 * Check if any element of an array of booleans or convertible to
 * booleans is false.
 * \see all()
 */
template <typename Array,
	  typename std::enable_if_t<std::is_convertible<typename Array::dtype, bool>::value>* = nullptr>
bool any(const Array& a)
{
  std::array<ArrayIndex,Array::ndim> path;
  auto p = [](bool val) { return val; };
  return breakable_for_looper<decltype(p),0,Array>(a, path, p);
}

/**
 * Check that all elements of an array of booleans are true.
 * \see any()
 */
template <typename Array,
	  std::enable_if_t<std::is_convertible<typename Array::dtype, bool>::value>* = nullptr>
bool all(const Array& a)
{
  std::array<ArrayIndex,Array::ndim> path;
  auto p = [](bool val) { return !val; };
  return !breakable_for_looper<decltype(p),0,Array>(a, path, p);
}

  /**
   * Compute the default strides for the given dimensions.
   */
  template <ArrayIndex N>
  std::array<ArrayIndex,N>
  default_strides(const std::array<ArrayIndex,N>& dims)
  {
    std::array<ArrayIndex,N> strides;
    if (N > 0) {
      auto prev = strides[N - 1] = 1;
      for (long i = N - 2; i >= 0; i--)
	prev = strides[i] = dims[i + 1] * prev;
    }
    return strides;
  }

  /**
   * Convert a scalar index into a multi-dimensional indexing path.
   * \param strides The strides of the underlying array.
   * \see default_strides to get strides for some dimensions.
   */
  template <ArrayIndex N>
  std::array<ArrayIndex,N>
  index_to_path(ArrayIndex idx, const std::array<ArrayIndex,N>& strides)
  {
    std::array<ArrayIndex,N> res;
    for (ArrayIndex i = 0; i < N; i++) {
      res[i] = idx / strides[i];
      idx %= strides[i];
    }
    return res;
  }

} // namespace necomi

// Local Variables:
// mode: c++
// End:
