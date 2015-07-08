// necomi/arrays/dimarray.h – Abstract helper class for array dimensions
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

namespace necomi {

/// Base class helper for fixed-ndim std::size_t dimensions arrays.
template <typename DimType, DimType N>
class DimArray
{
public:
  using dim_type = DimType;
  using dims_type = std::array<dim_type, N>;
  
  explicit DimArray(const dims_type& dims)
    : m_dims(dims)
  {}

  const dims_type& dims() const
  { return m_dims; }

  dim_type dim(dim_type i) const
  { return m_dims[i]; }
  
protected:
  /// Storage for the array dimensions.
  dims_type m_dims;
};

} // namespace necomi

// Local Variables:
// mode: c++
// End:
