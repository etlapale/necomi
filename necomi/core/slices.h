// necomi/core/slices.h – Slicing data types
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <array>
#include "../traits/generic.h"

namespace necomi
{
template <typename dim_type, dim_type N>
class Slice
{
public:
  using dims_type = std::array<dim_type,N>;

  Slice(const dims_type& start,
	const dims_type& size,
	const dims_type& strides)
    : m_start(start)
    , m_size(size)
    , m_strides(strides)
  {
  }

  template <dim_type DepN=N,
	    typename std::enable_if_t<DepN==1>* = nullptr>
  Slice(dim_type start, dim_type size, dim_type strides=1)
    : m_start{{start}} , m_size{{size}} , m_strides{{strides}}
  {}

  Slice(const std::array<std::array<dim_type,3>,N>& args)
  {
    for (dim_type i = 0; i < N; i++) {
      m_start[i] = args[i][0];
      m_size[i] = args[i][1] == 0 ? 1 : args[i][1];
      m_strides[i] = args[i][2] == 0 ? 1 : args[i][2];
    }
  }

  template <typename T,
	    std::enable_if_t<is_promotable<T,dim_type>::value>* = nullptr>
  Slice(const Slice<T,N>& s)
  {
    std::copy_n(s.start().cbegin(), N, m_start.begin());
    std::copy_n(s.size().cbegin(), N, m_size.begin());
    std::copy_n(s.strides().cbegin(), N, m_strides.begin());
  }

  const dims_type& start() const
  { return m_start; }

  const dims_type& size() const
  { return m_size; }

  const dims_type& strides() const
  { return m_strides; }

protected:
  dims_type m_start;
  dims_type m_size;
  dims_type m_strides;
};

template <typename dim_type, dim_type N>
Slice<dim_type,N+1> operator,(const Slice<dim_type,N>& a,
			      const Slice<dim_type,1>& b)
{
  std::array<std::array<dim_type,3>,N+1> args;
  for (dim_type i = 0; i < N; i++) {
    args[i][0] = a.start()[i];
    args[i][1] = a.size()[i];
    args[i][2] = a.strides()[i];
  }
  args[N][0] = b.start()[0];
  args[N][1] = b.size()[0];
  args[N][2] = b.strides()[0];
  return Slice<dim_type,N+1>(args);
}

template <typename dim_type=std::size_t,
	  std::enable_if_t<! is_array<dim_type>::value>* = nullptr>
Slice<dim_type,1>
slice(dim_type start, dim_type size, dim_type stride=1)
{
  return Slice<dim_type,1>(start, size, stride);
}
  
} // namespace necomi

// Local Variables:
// mode: c++
// End:
