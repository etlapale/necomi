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
 * Implements the SeedSequence concept for std::random_device.
 */
struct RandomDevSeedSequence
{
  typedef std::uint32_t result_type;
  RandomDevSeedSequence() {}

  template <typename InputIterator>
  RandomDevSeedSequence(InputIterator ia, InputIterator ib)
  {
    (void) ia; (void) ib;
  }

  RandomDevSeedSequence(std::initializer_list<std::uint32_t> il)
  {
    (void) il;
  }

  template <typename InputIterator>
  void generate(InputIterator ia, InputIterator ib) {
    for (; ia != ib; ++ia)
      *ia = m_dist(m_dev);
  }

  size_t size() { return 0; }

  template <typename OutputIterator>
  void param(OutputIterator ob)
  {
    (void) ob;
  }
protected:
  std::random_device m_dev;
  std::uniform_int_distribution<std::uint32_t> m_dist;
};


/**
 * Generate a one dimensional array filled with random numbers
 * following a normal distribution.
 */
template <typename T, typename PRNG>
Array<T,1> normal_distribution(const T& mean, const T& deviation,
			       ArrayIndex size, PRNG& prng)
{
  std::normal_distribution<T> dist(mean, deviation);
  Array<T,1> a(size);

  a.map([&dist,&prng](auto& path, auto& val) {
      (void) path;
      val = dist(prng);
    });

  return a;
}

} // namespace cuiloa

// Local Variables:
// mode: c++
// End:
