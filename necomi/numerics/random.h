// necomi/numerics/random.h – Pseudo random number generation
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <random>
#include <type_traits>

#include "../arrays/stridedarray.h"

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

  explicit RandomDevSeedSequence(std::initializer_list<std::uint32_t> il)
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
template <typename T, typename U, std::size_t N, typename PRNG,
	  typename V = typename std::common_type<T,U>::type,
	  typename W = typename std::conditional<std::is_floating_point<V>::value, V, double>::type>
StridedArray<W,N> normal(const T& mean, const U& deviation,
			 const std::array<std::size_t,N>& dims,
			 PRNG& prng)
{
  std::normal_distribution<W> dist(mean, deviation);
    StridedArray<W,N> a(dims);
  return transform(a, [&dist,&prng](auto){ return dist(prng); });
}


// TODO

/*
template <typename T=double, ArrayIndex N, typename PRNG>
constexpr auto normal(std::initializer_list<std::size_t> dims, PRNG& prng)
{
  Dimensions<dims.size()> d(dims);
  return normal<T,dims.size(),PRNG>(0, 1, d, prng);
}
*/


template <typename T=double, std::size_t N, typename PRNG>
auto normal(const std::array<std::size_t,N>& dims, PRNG& prng)
{
  return normal<T>(0, 1, dims, prng);
}


/**
 * Generate a one dimensional array filled with random numbers
 * following a normal distribution.
 */
template <typename T, typename U, typename PRNG>
auto normal(const T& mean, const U& deviation, std::size_t size, PRNG& prng)
{
  return normal<T,U,1>(mean, deviation, {size}, prng);
}

template <typename T=double, typename PRNG>
StridedArray<T,1> normal(std::size_t size, PRNG& prng)
{
  return normal<T,T,1>(0, 1, {size}, prng);
}

/**
 * Generate a one dimensional array filled with random floating point
 * numbers following a uniform distribution.
 */
template <typename T, std::size_t N, typename PRNG,
	  typename std::enable_if_t<std::is_floating_point<T>::value>* = nullptr>
StridedArray<T,N> uniform(const T& min, const T& max,
			  const std::array<std::size_t,N>& dims,
			  PRNG& prng)
{
  std::uniform_real_distribution<T> dist(min, max);
  StridedArray<T,N> a(dims);

  a.map([&dist,&prng](const auto&, auto& val) {
      val = dist(prng);
    });

  return a;
}

template <typename T, std::size_t N, typename PRNG,
	  typename std::enable_if_t<std::is_integral<T>::value>* = nullptr>
StridedArray<T,N> uniform(const T& min, const T& max,
			  const std::array<std::size_t,N>& dims,
			  PRNG& prng)
{
  std::uniform_int_distribution<T> dist(min, max);
  StridedArray<T,N> a(dims);

  a.map([&dist,&prng](const auto&, auto& val) {
      val = dist(prng);
    });

  return a;
}

template <typename T, typename PRNG,
	  typename std::enable_if_t<std::is_floating_point<T>::value
				    || std::is_integral<T>::value>* = nullptr>
StridedArray<T,1> uniform(const T& min, const T& max,
			  std::size_t size, PRNG& prng)
{
  return uniform<T,1>(min, max, {size}, prng);
}



/**
 * Generate a one dimensional array filled with random floating point
 * numbers following a uniform distribution.
 */
template <typename T, std::size_t N, typename PRNG,
	  typename std::enable_if_t<std::is_floating_point<T>::value>* = nullptr>
StridedArray<T,N> betarnd(const T& alpha, const T& beta,
			  const std::array<std::size_t,N>& dims,
			  PRNG& prng)
{
  std::gamma_distribution<T> xd(alpha, 1);
  std::gamma_distribution<T> yd(beta, 1);
  StridedArray<T,N> a(dims);

  a.map([&xd,&yd,&prng](const auto&, auto& val) {
      auto x = xd(prng);
      val = x / (x + yd(prng));
    });

  return a;
}

template <typename T, typename PRNG,
	  typename std::enable_if_t<std::is_floating_point<T>::value>* = nullptr>
StridedArray<T,1> betarnd(const T& alpha, const T& beta,
			  std::size_t size, PRNG& prng)
{
  return betarnd<T,1>(alpha, beta, {size}, prng);
}




} // namespace necomi

// Local Variables:
// mode: c++
// End:
