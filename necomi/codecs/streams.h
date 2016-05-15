// necomi/codecs/streams.h – Pretty printers for necomi data types
//
// Copyright © 2016 Émilien Tlapale
// Copyright © 2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <iterator>
#include <ostream>

#include "../delayed/transforms.h"
#include "../traits/arrays.h"

namespace necomi {
namespace streams {

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
	  typename std::enable_if_t<Array::ndim() == 0>* = nullptr>
std::ostream& operator<<(std::ostream& os, const Array& a)
{
  return os << a();
}

template <typename Array,
	  typename std::enable_if_t<(Array::ndim() >= 1)>* = nullptr>
std::ostream& operator<<(std::ostream& os, const Array& a)
{
  os << "[";
  for (auto i = 0UL; i < a.dim(0)-1; i++)
    os << necomi::slice(a,i) << ", ";
  if (a.dim(0) > 0)
    os << necomi::slice(a,a.dim(0)-1);
  return os << "]";
}
} // namespace streams


template <typename Dims, typename OutputIt>
OutputIt& copy_dims(const Dims& dims, OutputIt& d_first)
{
  std::copy(dims.cbegin(), dims.cend()-1,
	    std::ostream_iterator<typename Dims::value_type>(d_first, "×"));
  d_first << *(dims.cend()-1);
  return d_first;
}

} // namespace necomi

// Local Variables:
// mode: c++
// End:

