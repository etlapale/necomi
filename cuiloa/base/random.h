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

template <typename T, ArrayIndex N, typename PRNG>
Array<T,N> normal_distribution(const T& mean, const T& deviation,
			       PRNG& prng)
{
  std::normal_distribution<T> dist(mean, deviation);
  Array<T,N> a;

  a.map([&dist,&prng](auto& path, auto& val) {
      (void) path;
      val = dist(prng);
    });

  return a;
}

} // namespace cuiloa
