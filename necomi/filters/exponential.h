// necomi/filters/exponential.h – Recursive exponential filters
//
// Copyright © 2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <cstdlib>
#include <numeric>
#include <type_traits>
#include <vector>

#ifdef HAVE_BOOST
#include <boost/circular_buffer.hpp>
#include <boost/math/special_functions/binomial.hpp>

namespace necomi
{

template <typename T>
class RecursiveFilter
{
public:
  
  RecursiveFilter(const std::vector<T>& a,
		  const std::vector<T>& b)
    : m_a(a), m_b(b)
    , m_last_inputs(a.size(), 0), m_last_outputs(b.size(), 0)
  {};

  const std::vector<T> a() const
  { return m_a; }
  
  const std::vector<T> b() const
  { return m_b; }

  const boost::circular_buffer<T> last_inputs() const
  { return m_last_inputs; }

  const boost::circular_buffer<T> last_outputs() const
  { return m_last_outputs; }

  void feed(const T& input)
  {
    // Save the input
    //m_last_inputs.
    
    // Compute A·Y
    
  }
  
private:
  std::vector<T> m_a;
  std::vector<T> m_b;
  boost::circular_buffer<T> m_last_inputs;
  boost::circular_buffer<T> m_last_outputs;
};


/**
 *
 */
template <typename T>
auto exp_cascade(std::size_t order, T tau)
{
  static_assert(std::is_floating_point<T>::value,
		"exp_cascade requires a floating point argument");
  
  std::vector<T> a;
  for (auto i = 0UL; i < order + 2; i++)
    a.push_back((i%2 ? -1 : 1)
		* std::exp(- static_cast<T>(i) * order / tau)
		* boost::math::binomial_coefficient<T>(order+1, i));

  std::vector<T> b { std::accumulate(a.cbegin(), a.cend(), 0.0) };

  return RecursiveFilter<T>(a,b);
}

#endif // HAVE_BOOST

} // namespace necomi

// Local Variables:
// mode: c++
// End:
