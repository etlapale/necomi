// necomi/core/strides.h – Strides computations
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <array>
#include <numeric>
#include <type_traits>

namespace necomi
{

/**
 * Compute the default strides for the given dimensions.
 */
template <typename DimsType>
DimsType default_strides(const DimsType& dims)
{
  auto N = dims.size();
  DimsType strides(dims);	// Nop copy to set the size
  if (N > 0) {
    auto prev = strides[N - 1] = 1;
    for (long i = N - 2; i >= 0; i--)
      prev = strides[i] = dims[i + 1] * prev;
  }
  return strides;
}

/// Convert element coordinates in address offset.
template <typename Array,
	  typename dims_type = typename Array::dims_type,
	  std::enable_if_t<has_strides<Array>::value
			   && std::is_same<dims_type, typename Array::dims_type>::value>* = nullptr>
std::size_t strided_index(const Array& a, const dims_type& coords)
{
  // TODO static/dynamic check on a.ndim
  return std::inner_product(coords.cbegin(), coords.cend(),
			    a.strides().cbegin(), 0);
}


/// Convert element coordinates in address offset.
template <typename dims_type,
	  std::enable_if_t<! has_strides<dims_type>::value>* = nullptr>
std::size_t strided_index(const dims_type& dims, const dims_type& coords)
{
  return std::inner_product(coords.cbegin(), coords.cend(),
			    default_strides(dims).cbegin(), 0);
}

template <typename Array, typename ...Coords,
	  typename dim_type = typename Array::dim_type,
	  std::enable_if_t<all_convertible<Coords..., dim_type>::value>* = nullptr>
std::size_t strided_index(const Array& a, Coords... coords)
{
  // TODO static/dynamic check on a.ndim
  using dims_type = typename Array::dims_type;
  return strided_index(a, dims_type{static_cast<dim_type>(coords)...});
}

/**
 * Convert a scalar index into a multi-dimensional indexing path.
 * \param strides The strides of the underlying array.
 * \see default_strides to get strides for some dimensions.
 */
template <typename dims_type,
	  typename dim_type = typename dims_type::value_type>
dims_type strided_index_to_coords(dim_type idx, const dims_type& strides)
{
  dims_type res;
  for (auto i = 0UL; i < strides.size(); i++) {
    res[i] = idx / strides[i];
    idx %= strides[i];
  }
  return res;
}

} // namespace necomi

// Local Variables:
// mode: c++
// End:
