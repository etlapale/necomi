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

#include <random>

#include "array.h"

namespace cuiloa
{

/**
 * Represent an array expression.
 * Make sure to register each dependency with add_reference.
 */
template <typename Expr, typename T, ArrayIndex N>
class DelayedArray
{
public:
  DelayedArray(const std::array<ArrayIndex,N>& dims, Expr e)
    : m_dims(dims)
    , m_e(e)
  {
  }

  const std::array<ArrayIndex,N>& dimensions() const
  { return m_dims; }

  ArrayIndex size() const
  {
    return std::accumulate(m_dims.cbegin(), m_dims.cend(), 1,
        [] (ArrayIndex a, ArrayIndex b) { return a * b; });
  }

  /**
   * Return the value of a single element.
   */
  template <typename ...Indices>
  T operator()(Indices... indices) const
  {
    static_assert(sizeof...(Indices) == N, "improper indices arity");
    static_assert(all_indices<Indices...>(), "invalid indices type");

    std::array<ArrayIndex,N> idx {{static_cast<ArrayIndex>(indices)...}};
    return this->operator()(idx);
  }

  /**
   * Return the value of a single element.
   */
  T operator()(const std::array<ArrayIndex,N>& path) const
  {
    return m_e(path);
  }

  /**
   * Add a reference to the given array.
   */
  template <typename U, ArrayIndex M>
  void add_reference(const Array<U,M>& a)
  {
    auto b = new Array<U,M>(a);
    m_refs.push_back(std::shared_ptr<BaseArray>(b));
  }
protected:
  std::array<ArrayIndex,N> m_dims;
  Expr m_e;
  std::vector<std::shared_ptr<BaseArray>> m_refs;
};

/**
 * DelayedArray factory with template forwarding.
 * Allows the deducting the type of Expr.
 */
template <typename T, ArrayIndex N, typename Expr>
DelayedArray<Expr,T,N>
make_delayed(const std::array<ArrayIndex,N>& dims, Expr e)
{
  return DelayedArray<Expr,T,N>(dims, e);
}

/**
 * Namespace to work with DelayedArrays.
 */
namespace delayed
{
  template <typename T, ArrayIndex N>
  auto operator*(const Array<T,N>& a, const Array<T,N>& b)
  {
#ifndef CUILOA_NO_BOUND_CHECKS
    // Make sure the dimensions of a and b are the same
    if (a.dimensions() != b.dimensions())
      throw std::length_error("cannot multiply arrays of different dimensions");
#endif
    
    auto dims = a.dimensions();
    auto res = make_delayed<T,N>(dims, [a,b](auto& path) {
	return a(path) * b(path);
      });

    res.add_reference(a);
    res.add_reference(b);

    return res;
  }

} // namespace delayed
} // namespace cuiloa


// Local Variables:
// mode: c++
// End:
