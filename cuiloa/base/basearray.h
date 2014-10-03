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
 * Standard type to denote coordinate indices or dimensions.
 */
typedef unsigned int ArrayIndex;


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
           std::array<ArrayIndex,N>& path,
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
           std::array<ArrayIndex,N>& path,
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
		 std::array<ArrayIndex,N>& path,
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
		 std::array<ArrayIndex,N>& path,
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
		     std::array<ArrayIndex,N>& path,
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
		     std::array<ArrayIndex,N>& path,
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

  /**
   * Dimensions of the array.
   */
  const std::array<ArrayIndex,N>& dimensions() const
  {
    return m_dims;
  }
  
  /**
   * Number of elements in the array.
   */
  ArrayIndex size() const
  {
    return std::accumulate(m_dims.cbegin(), m_dims.cend(), 1,
        [] (ArrayIndex a, ArrayIndex b) { return a * b; });
  }

  /**
   * Apply a function to all the elements in the array.
   * \param f is a callable taking a path and an element, such as
   *        a std::function<void(Path&,T&)>.
   */
  template <typename UnaryOperation>
  void map(UnaryOperation f)
  {
    std::array<ArrayIndex,N> path;
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
    std::array<ArrayIndex,N> path;
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

} // namespace cuiloa

// Local Variables:
// mode: c++
// End:
