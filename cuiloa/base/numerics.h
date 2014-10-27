/* Copyright © 2014 University of California
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <algorithm>

#include "array.h"

namespace cuiloa
{
  /**
   * Discretization as nearest 1D element interpolation.
   */
  template <typename T, typename Func>
  class Discretization
  {
  public:
    Discretization(T min, T max, cuiloa::ArrayDimension size, Func func)
      : m_min(min)
      , m_max(max)
      , m_data(func(cuiloa::delayed::linspace<T>(min, max, size, true)))
    {}
    T operator()(T val) const
    {
      auto idx = static_cast<cuiloa::ArrayIndex>(0.5 + (val - m_min)/(m_max - m_min) * (m_data.size() - 1));
      //auto rect = std::max(static_cast<ArrayIndex>(0),
      //std::min(idx, m_data.size()));
      return m_data(idx);
    }
  protected:
    T m_min;
    T m_max;
    cuiloa::Array<T,1> m_data;
  };
  
  template <typename T, typename Func>
  auto discretization(T min, T max, cuiloa::ArrayDimension size, Func func)
  {
    return Discretization<T,Func>(min, max, size, func);
  }
  
  template <typename T>
  auto discretization(T min, T max, cuiloa::ArrayDimension size)
  {
    return discretization(min, max, size, [](auto&& a){ return a; });
  }
  
  enum class InterpolationMethod
  {
    NearestNeighbor,
    Linear
  };
  
  /**
   * Nearest-neighbor interpolation.
   */
  template <InterpolationMethod method, typename U=double,
            typename T, typename Concrete,
	    typename std::enable_if_t<
	      std::is_arithmetic<U>::value &&
	      method==InterpolationMethod::NearestNeighbor>* = nullptr>
  auto interpolation(cuiloa::AbstractArray<Concrete,T,1>& a)
  {
    return [a=a.shallow_copy()](U x) {
      auto x0 = static_cast<cuiloa::ArrayIndex>(0.5 + x);
      return a(x0);
    };
  }
  
  /**
   * Linear interpolation.
   */
  template <InterpolationMethod method, typename U=double,
            typename T, typename Concrete,
	    typename std::enable_if_t<
	      std::is_arithmetic<U>::value &&
	      method==InterpolationMethod::Linear>* = nullptr>
  auto interpolation(cuiloa::AbstractArray<Concrete,T,1>& a)
  {
    return [a=a.shallow_copy()](U x) {
      auto x0 = static_cast<cuiloa::ArrayIndex>(x);
      auto y0 = a(x0);
      auto y1 = a(x0 + 1);

      return y0 + (y1 - y0)*(x - x0);
    };
  }
 
  /**
   * Return a rescaling function mapping values in [imin,imax]
   * into values in [omin,omax].
   */
  template <typename T>
  auto rescale(T imin, T imax, T omin, T omax)
  {
    return [imin,imax,omin,omax](T x) {
      return (x - imin)*(omax-omin)/(imax-imin)+omin;
    };
  }
}

// Local Variables:
// mode: c++
// End:
