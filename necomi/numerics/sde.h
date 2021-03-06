// necomi/numerics/sde.h – Elementary SDE solver
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include "../arrays/stridedarray.h"

namespace necomi {

/**
 * Euler-Maruyama integration of stochastic differential equations.
 * States are represented by immediate N-dimensional arrays, and
 * the diffusion term can include an arbitrary number of noises.
 */
template <typename T, std::size_t N,
	  typename Drift, typename Diffusion, typename PRNG>
class EulerMaruyama
{
  // Make sure Drift and Diffusion objects have the correct return type.
  static_assert(std::is_convertible<decltype((std::declval<Drift>())(std::declval<necomi::StridedArray<T,N>>())),necomi::StridedArray<T,N>>::value,
		"invalid Drift type for the EM SDE solver");
  static_assert(std::is_convertible<decltype((std::declval<Diffusion>())(std::declval<necomi::StridedArray<T,N>>())),necomi::StridedArray<T,N+1>>::value,
		"invalid Diffusion type for the EM SDE solver");
public:
  /**
   * Both drift and diffusion must be function objects taking the
   * current state as argument, a const necomi::StridedArray<T,N> instance,
   * of dimensions given by \c dimensions [d1×…×dN].
   * The drift must return an array of the same dimensions, [d1×…×dN],
   * while the diffusion must return an array of adding an extra
   * dimension corresponding to the noises, [d1×…×dN×dnum_noises].
   */
  EulerMaruyama(const std::array<std::size_t,N>& dimensions, std::size_t num_noises,
                Drift drift, Diffusion diffusion, PRNG& prng)
    : m_drift(drift), m_diffusion(diffusion),
      m_t(0), m_X(dimensions),
      m_num_noises(num_noises),
      m_prng(prng)
  {
    m_X.fill(0);
  }

  void step(T dt)
  {
    using namespace broadcasting;

    auto Winc = std::sqrt(dt)*normal(m_num_noises, m_prng);
    const StridedArray<T,N>& X = m_X;
    m_X += dt*m_drift(X) + sum(m_diffusion(X)*Winc, N-1);

    m_t += dt;
  }

  T t() const
  { return m_t; }

  StridedArray<T,N>& X()
  { return m_X; }

  const StridedArray<T,N>& X() const
  { return m_X; }

protected:
  Drift m_drift;
  Diffusion m_diffusion;
  /** Current time. */
  T m_t;
  /** Current state. */
  StridedArray<T,N> m_X;
  /** Number of Wiener process noises. */
  std::size_t m_num_noises;
  /** Pseudo random number generator */
  PRNG& m_prng;
};

/**
 * Wrapper to the EulerMaruyama constructor.
 * Allows to skip explicit template arguments.
 */
template <typename T, std::size_t N,
	  typename Drift, typename Diffusion, typename PRNG>
auto euler_maruyama(const std::array<std::size_t,N>& dims, std::size_t num_noises,
		    Drift drift, Diffusion diffusion,
		    PRNG& prng)
{
  return EulerMaruyama<T,N,Drift,Diffusion,PRNG>(dims, num_noises,
                                                 drift, diffusion, prng);
}

} // namespace necomi

// Local Variables:
// mode: c++
// End:
