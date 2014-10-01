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
protected:
  std::array<ArrayIndex,N> m_dims;
  Expr m_e;
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

namespace delayed
{
  template <typename T, ArrayIndex N>
  auto operator*(const Array<T,N>& a, const Array<T,N>& b)
  {
    auto dims = a.dimensions();
    return make_delayed<T,N>(dims, [&a,&b](auto& path) {
	return a(path) * b(path);
      });
  }

} // namespace delayed
} // namespace cuiloa


// Local Variables:
// mode: c++
// End:
