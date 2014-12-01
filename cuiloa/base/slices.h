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

#include "basearray.h"

namespace cuiloa
{
  template <ArrayIndex N>
  class Slice
  {
  public:
    Slice(std::array<ArrayIndex,N> start,
	  std::array<ArrayIndex,N> size,
	  std::array<ArrayIndex,N> strides)
      : m_start(std::move(start))
      , m_size(std::move(size))
      , m_strides(std::move(strides))
    {
    }

    template <ArrayIndex DepN=N,
	      typename std::enable_if<DepN==1>::type* = nullptr>
    Slice(ArrayIndex start, ArrayIndex size, ArrayIndex strides=1)
      : m_start{{start}} , m_size{{size}} , m_strides{{strides}}
    {}

    Slice(const std::array<std::array<ArrayIndex,3>,N>& args)
    {
      for (ArrayIndex i = 0; i < N; i++) {
	m_start[i] = args[i][0];
	m_size[i] = args[i][1] == 0 ? 1 : args[i][1];
	m_strides[i] = args[i][2] == 0 ? 1 : args[i][2];
      }
    }

    const std::array<ArrayIndex,N>& start() const
    { return m_start; }

    const std::array<ArrayIndex,N>& size() const
    { return m_size; }

    const std::array<ArrayIndex,N>& strides() const
    { return m_strides; }

  protected:
    std::array<ArrayIndex,N> m_start;
    std::array<ArrayIndex,N> m_size;
    std::array<ArrayIndex,N> m_strides;
  };

  template <ArrayIndex N>
  Slice<N+1> operator,(const Slice<N>& a, const Slice<1>& b)
  {
    std::array<std::array<ArrayIndex,3>,N+1> args;
    for (ArrayIndex i = 0; i < N; i++) {
      args[i][0] = a.start()[i];
      args[i][1] = a.size()[i];
      args[i][2] = a.strides()[i];
    }
    args[N][0] = b.start()[0];
    args[N][1] = b.size()[0];
    args[N][2] = b.strides()[0];
    return Slice<N+1>(args);
  }

  inline Slice<1>
  slice(ArrayIndex start, ArrayIndex size, ArrayIndex stride=1)
  {
    return Slice<1>(start, size, stride);
  }
  
} // namespace cuiloa

// Local Variables:
// mode: c++
// End:
