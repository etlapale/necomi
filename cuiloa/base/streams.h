// cuiloa/base/streams.h – Pretty printers for cuiloa data types
//
// Copyright © 2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <iterator>
#include <ostream>

#include "traits.h"

namespace cuiloa
{
/**
 * Print array coordinates or dimensions on an output stream.
 */
template <std::size_t N>
std::ostream& operator<<(std::ostream& os,
			 const std::array<std::size_t,N>& a)
{
  os << "[";
  std::copy_n(a.cbegin(), N-1,
	    std::ostream_iterator<std::size_t>(os, ", "));
  if (N > 0)
    os << *(a.cend() - 1);
  return os << "]";
}


template <typename Array,
	  typename std::enable_if_t<Array::ndim == 0>* = nullptr>
std::ostream& operator<<(std::ostream& os, const Array& a)
{
  return os << a();
}

template <typename Array,
	  typename std::enable_if_t<(Array::ndim >= 1)>* = nullptr>
std::ostream& operator<<(std::ostream& os, const Array& a)
{
  os << "[";
  for (auto i = 0UL; i < a.dim(0)-1; i++)
    os << cuiloa::delayed::slice(a,i) << ", ";
  if (a.dim(0) > 0)
    os << cuiloa::delayed::slice(a,a.dim(0)-1);
  return os << "]";
}

} // namespace cuiloa

// namespace cuiloa
// Local Variables:
// mode: c++
// End:

