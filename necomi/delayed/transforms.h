// necomi/delayed/transforms.h – Array reshaping and reordering
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include "../arrays/delayed.h"

namespace necomi {

template <std::size_t M, typename Array>
auto reshape(const Array& a, const std::array<std::size_t,M>& d)
{
  using dim_type = typename Array::dim_type;
  
#ifndef NECOMI_NO_BOUND_CHECKS
    // Make sure the input and output array sizes are the same
    auto out_size = std::accumulate(d.cbegin(), d.cend(),
				    static_cast<dim_type>(1),
        [] (dim_type a, dim_type b) { return a * b; });
    if (out_size != size(a))
      throw std::length_error("invalid dimensions for reshaped array");
#endif
    auto old_strides = default_strides(a.dims());
    auto new_strides = default_strides(d);
    return make_delayed(d, [a,old_strides,new_strides](const auto& path) {
	auto idx = std::inner_product(path.cbegin(), path.cend(),
				      new_strides.cbegin(), 0);
	auto old_coords = strided_index_to_coords(idx, old_strides);
	return a(old_coords);
      });
}

template <typename Array, typename ...Dimensions,
	  typename dim_type = typename Array::dim_type,
	  typename std::enable_if_t<all_convertible<Dimensions...,dim_type>::value>* = nullptr>
auto reshape(const Array& a, Dimensions... dims)
{
  std::array<dim_type,sizeof...(Dimensions)> d =
    {static_cast<dim_type>(dims)...};
  return reshape(a, d);
}

/**
 * Shift elements on a given axis.
 */
template <typename Array, typename dim_type = typename Array::dim_type>
auto roll(const Array& a, dim_type shift, dim_type dim)
{
#ifndef NECOMI_NO_BOUND_CHECKS
  if (dim >= Array::ndim)
    throw std::out_of_range("invalid rolling dimension");
#endif
  auto sz = a.dims()[dim];
  return make_delayed(a.dims(), [a,sz,dim,shift] (auto coords) {
      coords[dim] = (coords[dim] + sz - shift) % sz;
      return a(coords);
    });
}

template <typename Array, typename dim_type = typename Array::dim_type,
	  std::enable_if_t<Array::ndim==1>* = nullptr>
auto roll(const Array& a, dim_type shift)
{
  return roll<Array>(a, shift, 0);
}

// TODO: remove (stack() special case)
template <typename Array1, typename Array2,
	  std::enable_if_t<Array1::ndim == Array2::ndim>* = nullptr>
auto zip(const Array1& a, const Array2& b)
{
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions of a and b are the same
  if (a.dims() != b.dims())
    throw std::length_error("cannot zip arrays of different dimensions");
#endif
  return make_delayed(append_coordinate(a.dims(), 2),
		      [a,b](const auto& coords) {
			auto c = remove_coordinate(coords, Array1::ndim);
			return coords[Array1::ndim] == 0 ? a(c) : b(c);
		      });
}
  
template <typename Array, typename T=typename Array::dtype,
	  typename dims_type = typename Array::dims_type>
auto shifted(const Array& a, std::array<ssize_t,Array::ndim> offset, T default_value = 0)
{
  using dim_type = typename Array::dim_type;
  
  return make_delayed(a.dims(),
	[a=a,offset=std::move(offset),
         default_value=std::move(default_value)]
	(const auto& coords) {
	  dims_type cx;
	  for (dim_type i = 0; i < Array::ndim; i++) {
	    // Check for negative resulting coordinates
	    if (offset[i] < 0
		&& static_cast<dim_type>(-offset[i]) > coords[i])
	      return default_value;
	    cx[i] = coords[i] + offset[i];
	    // Check for out of range resulting coordinates
	    if (cx[i] >= a.dim(i))
	      return default_value;
	  }
	  return a(cx);
	});
}


/**
 * Stack several indexable arrays into a single delayed one.  The
 * first dimension of the returned array will select one of the
 * orginal arrays, and the rest of the coordinates will index into them.
 */
template <typename Array, typename ...Arrays>
auto stack(const Array& a, const Arrays&... as)
{
  static_assert(same_dimensionality<Array,Arrays...>::value,
		"stacked arrays must have the same number of dimensions");
#ifndef NECOMI_NO_BOUND_CHECKS
  if (! same_dimensions(a, as...))
    throw std::length_error("stacked arrays must have the same dimensions");
#endif
		
  return make_delayed(prepend_coordinate(a.dims(), sizeof...(Arrays)+1),
		      [a,as...](const auto& coords) {
      auto c = remove_coordinate(coords, 0);
      return choose_array<0,Array,Arrays...>::at(coords[0], c, a, as...);
    });							   
}

/**
 * Concatenate arrays.
 */
template <typename Array, typename ...Arrays,
	  std::enable_if_t<is_indexable<Array>::value>* = nullptr>
auto concat(std::size_t d, const Array& a, const Arrays&... as)
{
  static_assert(same_dimensionality<Array, Arrays...>::value,
		"concatenated arrays must have the same dimensionality");
#ifndef NECOMI_NO_BOUND_CHECKS
  if (! almost_same_dimensions(d, a, as...))
    throw std::length_error("concatenated arrays must have almost the same dimensions");
#endif

  std::array<std::size_t, 1+sizeof...(Arrays)> ns{a.dim(d), as.dim(d)...};
  // Total size of the dimension along which to concatenate
  auto n = std::accumulate(ns.cbegin(), ns.cend(), 0);
  // Cumulative array to find the correct array for a given coordinate
  std::array<std::size_t, 1+sizeof...(Arrays)> cumsum;
  std::partial_sum(ns.cbegin(), ns.cend(), cumsum.begin());
  
  return make_delayed(change_coordinate(a.dims(), d, n),
		      [d,cumsum,a,as...](const auto& coords) {
			auto i = coords[d];
			// Search in which array the element is
			std::size_t j;
			for (j = 0; j < cumsum.size(); j++)
			  if (i < cumsum[j])
			    break;
			
			// Get element coordinates in the subarray
			auto c = change_coordinate(coords, d, i - (j > 0 ? cumsum[j-1] : 0));
			
			// TODO: throw except on out of range
			// Return the element
			return choose_array<0,Array,Arrays...>::at(j, c, a, as...);
		      });
}

template <typename Array, typename ...Arrays,
	  typename std::enable_if_t<is_indexable<Array>::value>* = nullptr>
auto concat(const Array& a, const Arrays&... as)
{
  return concat(0, a, as...);
}

// TODO: remove, special case of fix_dimension

template <typename Array,
	  std::enable_if_t<is_array<Array>::value>* = nullptr>
auto slice(Array a, std::size_t i)
{
  static_assert(Array::ndim >= 1,
		"only arrays with more than one dimension are sliceable");
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions of a and b are the same
  if (i >= a.dim(0))
    throw std::range_error("slice index is too large");
#endif
  return make_delayed(remove_coordinate(a.dims(), 0),
		      [a,i](const auto& coords) {
			auto c = prepend_coordinate(coords, i);
			return a(c);
		      });
}


template <typename Array,
	  std::enable_if_t<is_modifiable<Array>::value>* = nullptr>
auto fix_dimension(Array& a, std::size_t dim, std::size_t val)
{
  return make_delayed(remove_coordinate(a.dims(), dim),
		      [a,dim,val] (const auto& coords) mutable
		          -> typename Array::dtype &  {
			return a(add_coordinate(coords, dim, val));
		      });
}

template <typename Array>
auto fix_dimension(const Array& a, std::size_t dim, std::size_t val)
{
  return make_delayed(remove_coordinate(a.dims(), dim),
		      [a,dim,val] (const auto& coords) {
			return a(add_coordinate(coords, dim, val));
		      });
}

} // namespace necomi

// Local Variables:
// mode: c++
// End:
