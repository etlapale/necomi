// necomi/funcs/random.h – Pseudo random number generation
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <random>

#include "../base/array.h"

namespace necomi
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
template <typename T, ArrayIndex N, typename PRNG>
Array<T,N> normal(const T& mean, const T& deviation,
		  const std::array<ArrayIndex,N>& dims,
		  PRNG& prng)
{
  std::normal_distribution<T> dist(mean, deviation);
  Array<T,N> a(dims);

  a.map([&dist,&prng](auto& path, auto& val) {
      (void) path;
      val = dist(prng);
    });

  return a;
}

template <typename T, ArrayIndex N, typename PRNG>
Array<T,N> normal(const std::array<ArrayIndex,N>& dims, PRNG& prng)
{
  return normal<T,N,PRNG>(0, 1, dims, prng);
}


/**
 * Generate a one dimensional array filled with random numbers
 * following a normal distribution.
 */
template <typename T, typename PRNG>
Array<T,1> normal(const T& mean, const T& deviation,
		  ArrayIndex size, PRNG& prng)
{
  return normal<T,1>(mean, deviation, {size}, prng);
}

template <typename T=double, typename PRNG>
Array<T,1> normal(ArrayIndex size, PRNG& prng)
{
  return normal<T,1>(0, 1, {size}, prng);
}

/**
 * Generate a one dimensional array filled with random floating point
 * numbers following a uniform distribution.
 */
template <typename T, ArrayIndex N, typename PRNG>
std::enable_if_t<std::is_floating_point<T>::value,Array<T,N>>
uniform(const T& min, const T& max,
	const std::array<ArrayIndex,N>& dims,
	PRNG& prng)
{
  std::uniform_real_distribution<T> dist(min, max);
  Array<T,N> a(dims);

  a.map([&dist,&prng](auto& path, auto& val) {
      (void) path;
      val = dist(prng);
    });

  return a;
}

template <typename T, typename PRNG>
std::enable_if_t<std::is_floating_point<T>::value,Array<T,1>>
uniform(const T& min, const T& max,
	ArrayDimension size, PRNG& prng)
{
  return uniform<T,1>(min, max, {size}, prng);
}

} // namespace necomi

// Local Variables:
// mode: c++
// End:
