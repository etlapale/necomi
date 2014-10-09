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
    template <ArrayIndex DepN=N,
	      typename std::enable_if<DepN==1>::type* = nullptr>
    Slice(ArrayIndex start, ArrayIndex end, ArrayIndex strides=1)
      : m_start{{start}} , m_end{{end}} , m_strides{{strides}}
    {}

    /*Slice(const std::array<ArrayIndex,N>& start,
	  const std::array<ArrayIndex,N>& end)
      : m_start(start), m_end(end)
    {
      m_strides.fill(1);
    }

    Slice(const std::array<ArrayIndex,N>& start,
	  const std::array<ArrayIndex,N>& end,
	  const std::array<ArrayIndex,N>& strides)
      : m_start(start), m_end(end), m_strides(strides)
      {}*/

    //template <typename ...Slices>
    //Slice(std::initializer_list<std::array<ArrayIndex,N>> slices)
    //{
      //std::array<ArrayIndex,N> start = {slices
      //std::array<ArrayIndex,2*N> a{{slices...}};

      //std::vector<Slices...> args = {slices...};

      /*std::size_t i = 0;
      auto fun = [&i]() {
	std::cout << i << std::endl;
	i++;
	};*/
      //std::initializer_list<int>{(fun(slices),0),...};

      //throw std::runtime_error("free form NYI");
    //}

    Slice(const std::array<std::array<ArrayIndex,3>,N>& args)
    {
      for (ArrayIndex i = 0; i < N; i++) {
	m_start[i] = args[i][0];
	m_end[i] = args[i][1] == 0 ? m_start[i] : args[i][1];
	m_strides[i] = args[i][2] == 0 ? 1 : args[i][2];
      }
    }

    const std::array<ArrayIndex,N>& start() const
    { return m_start; }

    const std::array<ArrayIndex,N>& end() const
    { return m_end; }

    const std::array<ArrayIndex,N>& strides() const
    { return m_strides; }

  protected:
    std::array<ArrayIndex,N> m_start;
    std::array<ArrayIndex,N> m_end;
    std::array<ArrayIndex,N> m_strides;
  };

  template <ArrayIndex N>
  Slice<N+1> operator,(const Slice<N>& a, const Slice<1>& b)
  {
    std::array<std::array<ArrayIndex,3>,N+1> args;
    for (ArrayIndex i = 0; i < N; i++) {
      args[i][0] = a.start()[i];
      args[i][1] = a.end()[i];
      args[i][2] = a.strides()[i];
    }
    args[N][0] = b.start()[0];
    args[N][1] = b.end()[0];
    args[N][2] = b.strides()[0];
    return Slice<N+1>(args);
  }

  inline Slice<1>
  slice(ArrayIndex start, ArrayIndex end, ArrayIndex stride=1)
  {
    return Slice<1>(start, end, stride);
  }
} // namespace cuiloa

// Local Variables:
// mode: c++
// End:
