// necomi/base/delayed.h – Delayed arrays
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <random>

//#include "array.h"
//#include "concepts.h"

namespace necomi {

template <typename Array, typename Function,
	  std::enable_if_t<is_indexable<Array>::value>* = nullptr>
auto xmap(const Array& a, Function&& f)
{
  return make_delayed<typename Array::dtype, Array::ndim>(a.dims(), std::move(f));
}


/**
 * Namespace to work with DelayedArrays.
 */
namespace delayed
{

/**
 * Compare two arrays element-wise.
 * This operator is disabled if the shapes do not match, to allow for
 * shape broadcasting by other operators.
 */
template <typename Array1, typename Array2,
	  typename std::enable_if_t<is_array<Array1>::value
				    && is_array<Array2>::value>* = nullptr,
	  typename std::enable_if_t<Array1::ndim == Array2::ndim>* = nullptr>
auto operator==(const Array1& a, const Array2& b)
{
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions of a and b are the same
  if (a.dims() != b.dims())
    throw std::length_error("cannot compare arrays of different dimensions");
#endif
  return make_delayed<bool,Array1::ndim>(a.dims(),
					 [a,b] (const auto& coords) {
					   return a(coords) == b(coords);
					 });
}

template <typename Array1, typename Array2,
	  typename std::enable_if_t<is_array<Array1>::value &&
				    is_array<Array2>::value &&
				    Array1::ndim==Array2::ndim>* = nullptr>
auto operator!=(const Array1& a, const Array2& b)
{
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions of a and b are the same
  if (a.dims() != b.dims())
    throw std::length_error("cannot compare arrays of different dimensions");
#endif
  return make_delayed<bool,Array1::ndim>(a.dims(),
					 [a,b] (const auto& coords) {
					   return a(coords) != b(coords);
					 });
}

template <typename Array1, typename Array2,
	  typename std::enable_if_t<Array1::ndim==Array2::ndim>* = nullptr>
auto operator*(const Array1& a, const Array2& b)
{
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions of a and b are the same
  if (a.dims() != b.dims())
    throw std::length_error("cannot multiply arrays of different dimensions");
#endif
  using C = typename std::common_type<typename Array1::dtype,
				      typename Array2::dtype>::type;
  return make_delayed<C,Array1::ndim>(a.dims(),
				      [a,b] (const auto& x) { return a(x) * b(x); });
}

template <typename U, typename Array,
	  std::enable_if_t<is_indexable<Array>::value
			   && ! is_indexable<U>::value>* = nullptr>
auto operator*(const Array& a, U value)
{
  using C = typename std::common_type<typename Array::dtype, U>::type;
  return make_delayed<C,Array::ndim>(a.dims(),
				     [a,value] (const auto& x)
				     { return a(x)*value; });
}

template <typename U, typename Array,
	  std::enable_if_t<is_indexable<Array>::value
			   && ! is_indexable<U>::value>* = nullptr>
auto operator*(U value, const Array& a)
{
  using C = typename std::common_type<typename Array::dtype, U>::type;
  return make_delayed<C,Array::ndim>(a.dims(),
				     [a,value] (const auto& x)
				     { return value*a(x); });
}

template <typename Array1, typename Array2,
	  typename std::enable_if_t<Array1::ndim==Array2::ndim>* = nullptr>
auto operator/(const Array1& a, const Array2& b)
{
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions of a and b are the same
  if (a.dims() != b.dims())
    throw std::length_error("cannot divide arrays of different dimensions");
#endif
  using C = typename std::common_type<typename Array1::dtype,
				      typename Array2::dtype>::type;
  return make_delayed<C,Array1::ndim>(a.dims(),
				      [a,b] (const auto& x) { return a(x) / b(x); });
}

template <typename Array, typename U,
	  std::enable_if_t<is_indexable<Array>::value
			   && ! is_array<U>::valu>* = nullptr>
auto operator/(U value, const Array& a)
{
  using C = typename std::common_type<typename Array::dtype, U>::type;
  return make_delayed<C,Array::ndim>(a.dims(),
				     [a,value] (const auto& x)
				     { return value/a(x); });
}

template <typename Array, typename U,
	  std::enable_if_t<is_indexable<Array>::value
			   && ! is_array<U>::value>* = nullptr>
auto operator/(const Array& a, U value)
{
  using C = typename std::common_type<typename Array::dtype, U>::type;
  return make_delayed<C,Array::ndim>(a.dims(),
				     [a,value] (const auto& x)
				     { return a(x)/value; });
}

template <typename Array1, typename Array2,
	  typename std::enable_if_t<Array1::ndim==Array2::ndim>* = nullptr>
auto operator-(const Array1& a, const Array2& b)
{
  using C = typename std::common_type<typename Array1::dtype,
				      typename Array2::dtype>::type;
  
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions of a and b are the same
  if (a.dims() != b.dims())
    throw std::length_error("cannot sum arrays of different dimensions");
#endif
  return make_delayed<C, Array1::ndim>(a.dims(),
				       [a,b](const auto& coords) {
					 return a(coords) - b(coords);
				       });
}

template <typename Array, typename U,
	  std::enable_if_t<is_indexable<Array>::value
			   && ! is_array<U>::value>* = nullptr>
auto operator-(U value, const Array& a)
{
  using C = typename std::common_type<typename Array::dtype, U>::type;
  return make_delayed<C,Array::ndim>(a.dims(),
				     [a,value] (const auto& x)
				     { return value - a(x); });
}

template <typename Array, typename U,
	  std::enable_if_t<is_indexable<Array>::value
			   && ! is_array<U>::value>* = nullptr>
auto operator-(const Array& a, U value)
{
  using C = typename std::common_type<typename Array::dtype, U>::type;
  return make_delayed<C,Array::ndim>(a.dims(),
				     [a,value] (const auto& x)
				     { return a(x) - value; });
}

template <typename Array1, typename Array2,
	  typename std::enable_if_t<Array1::ndim==Array2::ndim>* = nullptr>
auto operator+(const Array1& a, const Array2& b)
{
  using C = typename std::common_type<typename Array1::dtype,
				      typename Array2::dtype>::type;
  
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions of a and b are the same
  if (a.dims() != b.dims())
    throw std::length_error("cannot sum arrays of different dimensions");
#endif
  return make_delayed<C, Array1::ndim>(a.dims(),
				       [a,b](const auto& coords) {
					 return a(coords) + b(coords);
				       });
}

template <typename T, typename Array,
	  typename C = typename std::common_type<typename Array::dtype>::type,
	  std::enable_if_t<is_indexable<Array>::value
			   && ! is_array<T>::value>* = nullptr>
auto operator+(const T& value, const Array& a)
{
  return make_delayed<C, Array::ndim>(a.dims(), [value,a](const auto& coords) { return value + a(coords); });
}

template <typename T, typename Array,
	  typename C = typename std::common_type<typename Array::dtype>::type,
	  std::enable_if_t<is_indexable<Array>::value
			   && ! is_array<T>::value>* = nullptr>
auto operator+(const Array& a, const T& value)
{
  return make_delayed<C, Array::ndim>(a.dims(), [value,a](const auto& coords) { return a(coords) + value; });
}

template <typename Array, typename T,
	  std::enable_if_t<is_indexable<Array>::value
			   && ! is_array<T>::value>* = nullptr>
auto operator>(const Array& a, const T& val)
{
  return make_delayed<bool,Array::ndim>(a.dims(), [a,val](auto& path) {
      return a(path) > val;
    });
}

template <typename Array, typename T,
	  std::enable_if_t<is_indexable<Array>::value
			   && ! is_array<T>::value>* = nullptr>
auto operator<(const Array& a, const T& val)
{
  return make_delayed<bool,Array::ndim>(a.dims(), [a,val](auto& path) {
      return a(path) < val;
    });
}

  template <typename T, ArrayIndex N>
  auto operator>(const Array<T,N>& a, const Array<T,N>& b)
  {
#ifndef NECOMI_NO_BOUND_CHECKS
    // Make sure the dimensions of a and b are the same
    if (a.dims() != b.dims())
      throw std::length_error("cannot sum arrays of different dimensions");
#endif
    auto fun = [a,b](auto& path) {
        return a(path) > b(path);
      };
    return DelayedArray<bool,N,decltype(fun)>(a.dims(), fun);
  }

  template <typename T, ArrayIndex N>
  auto operator<(const Array<T,N>& a, const Array<T,N>& b)
  {
#ifndef NECOMI_NO_BOUND_CHECKS
    // Make sure the dimensions of a and b are the same
    if (a.dims() != b.dims())
      throw std::length_error("cannot sum arrays of different dimensions");
#endif
    auto fun = [a,b](auto& path) {
        return a(path) < b(path);
      };
    return DelayedArray<bool,N,decltype(fun)>(a.dims(), fun);
  }


template <std::size_t M, typename Array>
auto reshape(const Array& a, const Dimensions<M>& d)
{
#ifndef NECOMI_NO_BOUND_CHECKS
    // Make sure the input and output array sizes are the same
    auto out_size = std::accumulate(d.cbegin(), d.cend(),
				    static_cast<ArrayDimension>(1),
        [] (ArrayIndex a, ArrayIndex b) { return a * b; });
    if (out_size != size(a))
      throw std::length_error("invalid dimensions for reshaped array");
#endif
    auto old_strides = default_strides(a.dims());
    auto new_strides = default_strides(d);
    return make_delayed<typename Array::dtype,M>(d,
			     [a,old_strides,new_strides]
			     (auto& path)
	       { auto idx = std::inner_product(path.cbegin(), path.cend(),
					       new_strides.cbegin(), 0);
		 auto old_path = index_to_path(idx, old_strides);
		 return a(old_path); });
}

template <typename Array, typename ...Dimensions,
	  typename std::enable_if_t<all_indices<Dimensions...>::value>* = nullptr>
auto reshape(const Array& a, Dimensions... dims)
{
  necomi::Dimensions<sizeof...(Dimensions)> d =
    {static_cast<std::size_t>(dims)...};
  return reshape(a, d);
}

/**
 * Shift elements on a given axis.
 */
template <typename Array>
auto roll(const Array& a, ArrayIndex shift, ArrayIndex dim)
{
#ifndef NECOMI_NO_BOUND_CHECKS
  if (dim >= Array::ndim)
    throw std::out_of_range("invalid rolling dimension");
#endif
  auto sz = a.dims()[dim];
  return make_delayed<typename Array::dtype, Array::ndim>(a.dims(),
			   [a,sz,dim,shift]
			   (auto path) {
			     path[dim] = (path[dim] + sz - shift) % sz;
			     return a(path);
			   });
}

template <typename Array,
	  std::enable_if_t<Array::ndim==1>* = nullptr>
auto roll(const Array& a, ArrayIndex shift)
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
  using T = typename std::common_type<typename Array1::dtype,
				      typename Array2::dtype>::type;
  return make_delayed<T,Array1::ndim+1>(append_coordinate(a.dims(), 2),
			     [a,b]
			     (auto& coords) {
					  auto c = remove_coordinate(coords, Array1::ndim);
			       return coords[Array1::ndim] == 0 ? a(c) : b(c);
			     });
}
  
template <typename Array, typename T=typename Array::dtype>
auto shifted(const Array& a,
	     std::array<T,Array::ndim> offset,
	     T default_value = 0)
{
  return make_delayed<T,Array::ndim>(a.dims(),
	[a=a,offset=std::move(offset),
         default_value=std::move(default_value)]
	(const auto& coords) {
				       Coordinates<Array::ndim> cx;
	  for (ArrayIndex i = 0; i < Array::ndim; i++) {
	    // Check for negative resulting coordinates
	    if (offset[i] < 0
		&& static_cast<ArrayDimension>(-offset[i]) > coords[i])
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
		
  using T = typename std::common_type<typename Array::dtype,
				      typename Arrays::dtype...>::type;

  return make_delayed<T,Array::ndim+1>(prepend_coordinate(a.dims(), sizeof...(Arrays)+1), [a,as...] (const auto& coords) {
      auto c = remove_coordinate(coords, 0);
      return choose_array<0,Array,Arrays...>::at(coords[0], c, a, as...);
    });							   
}

/**
 * Concatenate arrays.
 */
template <typename Array1, typename Array2>
auto concat(const Array1& a, const Array2& b)
{
  static_assert(same_dimensionality<Array1, Array2>::value,
		"concatenated arrays must have the same number of dimensions");
  auto d = 0UL;	// Dimension along which to concatenate
#ifndef NECOMI_NO_BOUND_CHECKS
  if (! almost_same_dimensions(d, a, b))
    throw std::length_error("concatenated arrays must have almost the same dimensions");
#endif
  
  using T = typename std::common_type<typename Array1::dtype,
				      typename Array2::dtype>::type;

  return make_delayed<T,Array1::ndim>(change_coordinate(a.dims(), d, a.dim(d) + b.dim(d)), [d,a,b](const auto& coords) {
      auto i = coords[d];
      if (i < a.dim(d))
	return a(change_coordinate(coords, d, i));
      else
	return b(change_coordinate(coords, d, i-a.dim(d)));
    });
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
  return make_delayed<typename Array::dtype,Array::ndim-1>(remove_coordinate(a.dims(), 0), [a,i] (const auto& coords) {
      auto c = prepend_coordinate(coords, i);
      return a(c);
    });
}

} // namespace delayed
  
  //////////////////////////////////////////////////////////////////////////
  
template <typename Array>
auto fix_dimension(const Array& a, ArrayIndex dim, ArrayIndex val)
{
  return make_delayed<typename Array::dtype,Array::ndim-1>(remove_coordinate(a.dims(), dim),
			     [a,dim,val]
			     (const auto& coords) {
			       return a(add_coordinate(coords, dim, val));
			     });
}

} // namespace necomi

// Local Variables:
// mode: c++
// End:
