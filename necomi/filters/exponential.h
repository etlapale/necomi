// necomi/filters/exponential.h – Recursive exponential filters
//
// Copyright © 2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <array>
#include <cstdlib>
#include <numeric>
#include <type_traits>
#include <vector>

#ifdef HAVE_BOOST
#include <boost/math/special_functions/binomial.hpp>
#endif

#include "../convert/stl.h"

namespace necomi
{

template <typename T, std::size_t N>
class RecursiveFilter
{
  static_assert(std::is_floating_point<T>::value,
		"recursive filtering requires floating point values");
  
public:
  RecursiveFilter(const std::vector<T>& a,
		  const std::vector<T>& b,
		  const std::array<std::size_t,N>& dims)
    : m_a(a), m_b(from_vector(b))
    , m_last_inputs(prepend_coordinate(dims, b.size()))
    , m_last_outputs(prepend_coordinate(dims, a.size()-1))
    , m_in_pos(0), m_out_pos(0)
  {
    m_last_inputs = 0;
    m_last_outputs = 0;
  };

  const std::vector<T> a() const
  { return m_a; }
  
  const std::vector<T> b() const
  { return to_vector(m_b); }

  template <typename Input,
	    std::enable_if_t<is_indexable<Input>::value>* = nullptr>
  const necomi::StridedArray<T,N> feed(const Input& input)
  {
    static_assert(Input::ndim == N, "invalid input array dimensionality");
    
#ifndef NECOMI_NO_BOUND_CHECKS
    if (input.dims() != remove_coordinate(m_last_inputs.dims(), 0))
      throw std::length_error("input array dimensions incompatible with declared ones");
#endif
  
    // Save the input
    m_in_pos = (m_in_pos + m_last_inputs.dim(0) - 1) % m_last_inputs.dim(0);
    m_last_inputs[m_in_pos] = input;
    
    // Compute A·Y
    auto a_y = strided_array(zeros_like(input));
    for (auto i = 0UL; i < m_last_outputs.dim(0); i++)
      a_y += m_a[i+1]
	* m_last_outputs[(i + m_out_pos) % m_last_outputs.dim(0)];
    // Compute B·X
    auto b_x = sum(widen_right(m_last_inputs.dims(), m_b)
		   * roll(m_last_inputs, m_in_pos, 0UL), 0);

    // Compute and save the output
    m_out_pos = (m_out_pos + m_last_outputs.dim(0) - 1) % m_last_outputs.dim(0);
    m_last_outputs[m_out_pos] = (b_x - a_y) / m_a[0];

    return m_last_outputs[m_out_pos];
  }

  /**
   * Convenience overload for scalar values.
   */
  template <typename U,
	    std::enable_if_t<std::is_convertible<U,T>::value
			     && N == 0>* = nullptr>
  T feed(const U& input)
  {
    necomi::StridedArray<T,0> in;
    in = input;
    return feed(in)();
  }
  
private:
  /// Coefficient applied to the last outputs.
  std::vector<T> m_a;
  /// Coefficient applied to the last inputs.
  //std::vector<T> m_b;
  StridedArray<T,1> m_b;
  /// Copy of the last inputs.
  necomi::StridedArray<T,N+1> m_last_inputs;
  /// Last outputs.
  necomi::StridedArray<T,N+1> m_last_outputs;;
  /// Position in the circular array of last inputs.
  std::size_t m_in_pos;
  /// Position in the circular array of last outputs.
  std::size_t m_out_pos;
};


#ifdef HAVE_BOOST

/**
 *
 */
template <typename T, std::size_t N>
auto exp_cascade(std::size_t order, T tau,
		 const std::array<std::size_t,N>& dims)
{
  static_assert(std::is_floating_point<T>::value,
		"exp_cascade requires a floating point argument");
  
  std::vector<T> a;
  for (auto i = 0UL; i < order + 2; i++)
    a.push_back((i%2 ? -1 : 1)
		* std::exp(- static_cast<T>(i) * order / tau)
		* boost::math::binomial_coefficient<T>(order+1, i));

  std::vector<T> b { std::accumulate(a.cbegin(), a.cend(), 0.0) };

  return RecursiveFilter<T,N>(a, b, dims);
}

/// Convenience wrapper for scalars.
template <typename T>
auto exp_cascade(std::size_t order, T tau)
{
  return exp_cascade<T,0>(order, tau, {});
}

#endif // HAVE_BOOST

} // namespace necomi

// Local Variables:
// mode: c++
// End:
