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

#include "../base/delayed.h"

namespace cuiloa
{

  /**
   * Get the cosinus of each element interpreted as a angle in radians.
   */
  template <typename T, ArrayDimension N, typename Concrete,
	    std::enable_if_t<std::is_floating_point<T>::value>* = nullptr>
  auto cos(const AbstractArray<Concrete,T,N>& a)
  {
    return make_delayed<T,N>(a.dimensions(),
			     [a=a.shallow_copy()] (auto& coords) {
			       return std::cos(a(coords));
			     });
  }

  /**
   * Get the cosinus of each element interpreted as a angle in radians.
   */
  template <typename T, ArrayDimension N, typename Concrete,
	    std::enable_if_t<std::is_floating_point<T>::value>* = nullptr>
  auto sin(const AbstractArray<Concrete,T,N>& a)
  {
    return make_delayed<T,N>(a.dimensions(),
			     [a=a.shallow_copy()] (auto& coords) {
			       return std::sin(a(coords));
			     });
  }

  template <typename T,
	    std::enable_if_t<std::is_floating_point<T>::value>* = nullptr>
  constexpr T radians(T angle)
  {
    return angle *  M_PI / 180.;
  }

  /**
   * Convert each element interpreted as a angle in degrees into radians.
   */
  template <typename T, ArrayDimension N, typename Concrete,
	    std::enable_if_t<std::is_floating_point<T>::value>* = nullptr>
  auto radians(const AbstractArray<Concrete,T,N>& a)
  {
    return make_delayed<T,N>(a.dimensions(),
			     [a=a.shallow_copy()] (auto& coords) {
			       return radians(a(coords));
			     });
  }
  
  template <typename T,
	    std::enable_if_t<std::is_floating_point<T>::value>* = nullptr>
  constexpr T degrees(T angle)
  {
    return angle * 180. / M_PI;
  }

  /**
   * Convert each element interpreted as a angle in radians into degrees.
   */
  template <typename T, ArrayDimension N, typename Concrete,
	    std::enable_if_t<std::is_floating_point<T>::value>* = nullptr>
  auto degrees(const AbstractArray<Concrete,T,N>& a)
  {
    return make_delayed<T,N>(a.dimensions(),
			     [a=a.shallow_copy()] (auto& coords) {
			       return degrees(a(coords));
			     });
  }

} // namespace cuiloa

// Local Variables:
// mode: c++
// End:
