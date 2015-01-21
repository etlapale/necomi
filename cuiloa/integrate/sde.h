/* Copyright © 2014–2015 University of California
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

#include "../base/array.h"

namespace cuiloa {

  /**
   * Euler-Maruyama integration of stochastic differential equations.
   * States are represented by immediate N-dimensional arrays, and
   * the diffusion term can include an arbitrary number of noises.
   */
  template <typename T, ArrayDimension N,
	    typename Drift, typename Diffusion, typename PRNG>
  class EulerMaruyama
  {
  public:
    EulerMaruyama(const Dimensions<N>& dimensions, ArrayIndex num_noises,
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
      using namespace delayed;
      using namespace delayed::broadcasting;

      auto Winc = std::sqrt(dt)*normal(m_num_noises, m_prng);
      m_X += dt*m_drift(m_X) + sum(m_diffusion(m_X)*Winc, 1);

      m_t += dt;
    }

    T t() const
    { return m_t; }

    Array<T,N>& X()
    { return m_X; }

    const Array<T,N>& X() const
    { return m_X; }

  protected:
    Drift m_drift;
    Diffusion m_diffusion;
    T m_t;
    Array<T,N> m_X;
    ArrayDimension m_num_noises;
    PRNG& m_prng;
  };

  template <typename T, ArrayDimension N,
	    typename Drift, typename Diffusion, typename PRNG>
  auto euler_maruyama(const Dimensions<N>& dims, ArrayIndex num_noises,
		      Drift drift, Diffusion diffusion,
		      PRNG& prng)
  {
    return EulerMaruyama<T,N,Drift,Diffusion,PRNG>(dims, num_noises,
						   drift, diffusion, prng);
  }

}

// Local Variables:
// mode: c++
// End:
