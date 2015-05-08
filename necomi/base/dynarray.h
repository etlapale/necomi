// necomi/base/dynarray.h – Dynamically-sized strided arrays
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <vector>

namespace necomi
{

template <typename T>
class DynArray
{
public:
  using dim_type = std::size_t;
  using dims_type = std::vector<dim_type>;
  using dtype = T;

  template <typename ...Dims,
	    std::enable_if_t<all_convertible<Dims..., dim_type>::value>* = nullptr>
  DynArray(Dims... dims)
    : m_dims{static_cast<dim_type>(dims)...}
  {
  }

  const dims_type& dims() const
  { return m_dims; }
protected:
  dims_type m_dims;
}; // class DynArray

} // namespace necomi

// Local Variables:
// mode: c++
// End:
