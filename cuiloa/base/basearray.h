/*
 * Copyright © 2014	University of California, Irvine
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

#include <array>
#include <numeric>

namespace cuiloa
{

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

/**
* The parent class of all array types.
* Useful for managing heterogenous set of pointer to arrays.
*/
class BaseArray
{
};


template <typename Concrete, typename T, ArrayIndex N> class AbstractArray;


#ifndef IN_DOXYGEN
/**
* Final case of for loops through template metaprogramming.
*/
template <typename UnaryOperation, ArrayIndex M,
typename Concrete, typename T, ArrayIndex N>
std::enable_if_t<M==N>
for_looper(AbstractArray<Concrete,T,N>& a,
           Coordinates<N>& path,
           UnaryOperation f)
{
  f(path, static_cast<Concrete&>(a)(path));
}

/**
 * Recursion case of for loops through template metaprogramming.
 */
template <typename UnaryOperation, ArrayIndex M,
	  typename Concrete, typename T, ArrayIndex N>
std::enable_if_t<M<N>
for_looper(AbstractArray<Concrete,T,N>& a,
           Coordinates<N>& path,
           UnaryOperation f)
{
  for (ArrayIndex i = 0; i < a.dimensions()[M]; i++) {
    path[M] = i;
    for_looper<UnaryOperation,M+1,Concrete,T,N>(a, path, f);
  }
}

/**
 * Final case of for loops through template metaprogramming
 * for constant arrays.
 */
template <typename ConstMapOperation, ArrayIndex M,
	  typename Concrete, typename T, ArrayIndex N>
std::enable_if_t<M==N>
const_for_looper(const AbstractArray<Concrete,T,N>& a,
		 Coordinates<N>& path,
		 ConstMapOperation f)
{
  f(path, static_cast<const Concrete&>(a)(path));
}
  
/**
 * Recursion case of for loops through template metaprogramming.
 * for constant arrays.
 */
template <typename ConstMapOperation, ArrayIndex M,
	  typename Concrete, typename T, ArrayIndex N>
std::enable_if_t<M<N>
const_for_looper(const AbstractArray<Concrete,T,N>& a,
		 Coordinates<N>& path,
		 ConstMapOperation f)
{
  for (ArrayIndex i = 0; i < a.dimensions()[M]; i++) {
    path[M] = i;
    const_for_looper<ConstMapOperation,M+1,Concrete,T,N>(a, path, f);
  }
}

/**
 * Final case of breakable for loops through template metaprogramming
 * for constant arrays.
 */
template <typename Predicate, ArrayIndex M,
	  typename Concrete, typename T, ArrayIndex N>
std::enable_if_t<M==N,bool>
breakable_for_looper(const AbstractArray<Concrete,T,N>& a,
		     Coordinates<N>& path,
		     Predicate p)
{
  return p(static_cast<const Concrete&>(a)(path));
}

/**
 * Recursion case of for loops through template metaprogramming
 * for constant arrays.
 *
 * \ingroup core
 * \see Array::map
 */
template <typename Predicate, ArrayIndex M,
	  typename Concrete, typename T, ArrayIndex N>
std::enable_if_t<(M<N),bool>
breakable_for_looper(const AbstractArray<Concrete,T,N>& a,
		     Coordinates<N>& path,
		     Predicate p)
{
  for (ArrayIndex i = 0; i < a.dimensions()[M]; i++) {
    path[M] = i;
    bool ret = breakable_for_looper<Predicate,M+1,Concrete,T,N>(a, path, p);
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
template <typename Concrete, typename T, ArrayIndex N>
class AbstractArray : public BaseArray
{
public:
  AbstractArray(const std::array<ArrayIndex,N>& dimensions)
    : m_dims(dimensions)
  {
  }

  Concrete shallow_copy() const
  {
    return Concrete(*static_cast<const Concrete*>(this));
  }

  /**
   * Dimensions of the array.
   */
  const std::array<ArrayDimension,N>& dimensions() const
  {
    return m_dims;
  }

  /**
   * Return the number of dimensions in the array.
   */
  constexpr ArrayDimension ndim() const
  {
    return N;
  }

  /**
   * Return a given dimension.
   */
  ArrayDimension dim(ArrayIndex i) const
  {
    return m_dims[i];
  }

  /**
   * Number of elements in the array.
   */
  ArrayIndex size() const
  {
    return std::accumulate(m_dims.cbegin(), m_dims.cend(), 1,
        [] (ArrayIndex a, ArrayIndex b) { return a * b; });
  }

  T operator()(const std::array<ArrayIndex,N>& path) const
  {
    return static_cast<const Concrete*>(this)->operator()(path);
  }

  /**
   * Apply a function to all the elements in the array.
   * \param f is a callable taking a path and an element, such as
   *        a std::function<void(Path&,T&)>.
   */
  template <typename UnaryOperation>
  void map(UnaryOperation f)
  {
    Coordinates<N> path;
    for_looper<UnaryOperation,0,Concrete,T,N>(*this, path, f);
  }

  /**
   * Apply a function to all the elements in the array.
   * \param f is a callable taking a path and an element, such as
   *        a std::function<void(Path&, const& T)>.
   */
  template <typename ConstMapOperation>
  void map(ConstMapOperation f) const
  {
    Coordinates<N> path;
    const_for_looper<ConstMapOperation,0,Concrete,T,N>(*this, path, f);
  }

protected:
  /// Storage for the array dimensions.
  std::array<ArrayIndex,N> m_dims;
};

/**
 * Sum all the elements of an array.
 * \warning This may overflow.
 */
template <typename Concrete, typename T, ArrayIndex N>
T sum(const AbstractArray<Concrete,T,N>& a) {
  T total = 0;
  a.map([&](auto& path, auto val) {
      (void) path;
      total += val;
    });
  return total;
}

  /**
   * Maximum value in the array.
   */
  template <typename Concrete, typename T, ArrayIndex N>
  T max(const AbstractArray<Concrete,T,N>& a) {
    Coordinates<N> first;
    first.fill(0);
    T res = a(first);
    a.map([&res,&a](auto& path, auto val) {
	(void) path;
	res = val > res ? val : res;
      });
    return res;
  }

/**
 * Check if any element of an array of booleans is false.
 * \see all()
 */
template <typename Concrete, ArrayIndex N>
bool any(const AbstractArray<Concrete,bool,N>& a)
{
  std::array<ArrayIndex,N> path;
  auto p = [](bool val) { return val; };
  return breakable_for_looper<decltype(p),0,Concrete,bool,N>(a, path, p);
}

/**
 * Check that all elements of an array of booleans are true.
 * \see any()
 */
template <typename Concrete, ArrayIndex N>
bool all(const AbstractArray<Concrete,bool,N>& a)
{
  std::array<ArrayIndex,N> path;
  auto p = [](bool val) { return !val; };
  return !breakable_for_looper<decltype(p),0,Concrete,bool,N>(a, path, p);
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

} // namespace cuiloa

// Local Variables:
// mode: c++
// End:
