// necomi/arrays/stridedarray.h – In memory shared strided array
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <memory>
#include <sstream>

#include "../core/iterators.h"
#include "../core/loops.h"
#include "../core/shape.h"
#include "../core/slices.h"
#include "../core/strides.h"

#include "../codecs/streams.h"

#include "../traits/arrays.h"

#include "dimarray.h"

namespace necomi {

/**
 * Multi-dimensional arrays supporting non-contiguous shared data.
 */
template <typename T, std::size_t N>
class StridedArray
    // Parent helper class handling dimensions
  : public DimArray<std::size_t, N>
{
public:
  using dim_type = std::size_t;
  using dims_type = std::array<dim_type, N>;
  using dtype = T;
  enum { ndim = N };

  // Facilitate access to classes from the same template.
  template <typename U, dim_type M> friend class Array;

  /// Short name for the parent class
  typedef DimArray<std::size_t, N> Parent;

public:
  /**
   * Create a new multi-dimensional array with uninitialized elements.
   */
  template <typename ...Dims,
	    typename std::enable_if_t<sizeof...(Dims) == N && all_convertible<Dims..., dim_type>::value>* = nullptr>
  StridedArray(Dims ...dims)
    : StridedArray(dims_type{static_cast<dim_type>(dims)...})
  {}

  /**
   * Create a new multi-dimensional array with uninitialized elements.
   */
  StridedArray(const dims_type& dims)
    : Parent(dims)
    , m_strides(default_strides(dims))
    , m_shared_data(new T[size(*this)], [](T* p){ delete [] p; })
    , m_data(m_shared_data.get())
  {}

  /**
   * Create a shared view of the given array.
   */
  StridedArray(const StridedArray<T,N>& src)
    : Parent(src.m_dims)
    , m_strides(src.m_strides)
    , m_shared_data(src.m_shared_data)
    , m_data(src.m_data)
  {}

  /**
   * Construct an array from already existing data.
   * The given data is never destroyed.
   */
  template <typename ...Dims, typename Deleter,
	    typename std::enable_if<sizeof...(Dims) == N && all_convertible<Dims..., dim_type>(),int>::type = 0>
  StridedArray(T* data, Deleter deleter, Dims ...dims)
    : StridedArray(data, dims_type{static_cast<dim_type>(dims)...}, deleter)
  {}

  /**
   * Construct an array from already existing data.
   */
  template <typename Deleter>
  StridedArray(T* data, const dims_type& dims, Deleter deleter)
    : Parent(dims)
    , m_strides(default_strides(dims))
    , m_shared_data(data, deleter)
    , m_data(data)
  {
  }

  StridedArray(std::shared_ptr<T> shared_data,
	T* data,
	const dims_type& strides,
	const dims_type& dims)
    : Parent(dims)
    , m_strides(strides)
    , m_shared_data(shared_data)
    , m_data(data)
  {
  }

  template <typename Array,
	    std::enable_if_t<is_indexable<Array>::value
			     && Array::ndim == N
			     && is_promotable<typename Array::dtype,T>::value>* = nullptr>
  StridedArray(const Array& a)
    : StridedArray(a.dims())
  {
    this->operator=(a);
  }

  template <typename Array,
	    std::enable_if_t<has_dtype<Array>::value
			     && ! has_ndim<Array>::value
			     && has_dims<Array>::value
			     && is_promotable<typename Array::dtype,T>::value>* = nullptr>
  StridedArray(const Array& a)
    : StridedArray(to_array<dim_type, ndim>(a.dims()))
  {
    this->operator=(a);
  }

  const dims_type& strides() const
  { return m_strides; }

  /**
   * Checks whether the array is contiguous.
   */
  bool contiguous() const
  {
    if (N == 0)
      return true;

    dim_type prev = 1;
    for (ssize_t i = N - 1; i >= 0; i--) {
      if (m_strides[i] != prev)
	return false;
      prev *= this->m_dims[i];
    }
    return true;
  }

  /**
   * Return a view on a slice of the array.
   * Template argument DepN is a trick to allow std::enable_if_t and
   * disable the function for N=0. You should ignore it.
   */
  template <dim_type DepN = N,
	    typename std::enable_if_t<DepN!=0>* = nullptr>
  StridedArray<T,DepN-1> operator[](dim_type index) const
  {
#ifndef NECOMI_NO_BOUND_CHECKS
    // Check slicing index
    if (index > this->m_dims[0])
      throw std::out_of_range("invalid slicing index");
#endif

    // New dimensions
    std::array<dim_type,N-1> dims;
    std::copy(std::next(this->m_dims.cbegin()), this->m_dims.cend(), dims.begin());
    // New strides
    std::array<dim_type,N-1> strides;
    std::copy(std::next(m_strides.cbegin()), m_strides.cend(), strides.begin());

    StridedArray<T,N-1> a(m_shared_data, &(m_data[index * m_strides[0]]), strides, dims);
    
    return a;
  }

  T& operator()(const dims_type& coords)
  { return m_data[strided_index(*this, coords)];}

  const T& operator()(const dims_type& coords) const
  { return m_data[strided_index(*this, coords)]; }
    
  template <typename ...Coords,
	    std::enable_if_t<sizeof...(Coords) == N && all_convertible<Coords...,dim_type>::value>* = nullptr>
    T& operator()(Coords... coords)
  {
    // TODO check indices for out of bounds
    return m_data[strided_index(*this, coords...)];
  }
    
  template <typename ...Coords,
	    std::enable_if_t<sizeof...(Coords) == N && all_convertible<Coords...,dim_type>::value>* = nullptr>
    const T& operator()(Coords... coords) const
  {
    // TODO check indices for out of bounds
    return m_data[strided_index(*this, coords...)];
  }

  /**
   * Return a restricted view on the array.
   */
  StridedArray<T,N> slice(const Slice<dim_type, N>& s) const
  {
    StridedArray<T,N> a(*this);
#ifndef NECOMI_NO_BOUND_CHECKS
    for (dim_type i = 0; i < N; i++) {
      // Check that starting point is valid
      if (s.start()[i] >= this->m_dims[i])
	throw std::out_of_range("invalid starting point for slicing");
      // Check that resulting size is valid
      if (s.start()[i] + s.size()[i] > this->m_dims[i])
	throw std::out_of_range("slicing view exceeds array dimensions");
    }
#endif
    // Starting point
    a.m_data = &a(s.start());
    // Dimensions
    // TODO: bound checks
    a.m_dims = s.size();
    // Strides
    std::transform(s.strides().cbegin(), s.strides().cend(),
		   m_strides.cbegin(), a.m_strides.begin(), 
		   [](auto a, auto b) { return a * b; });
    return a;
  }
    
  /**
   * Fix a dimension in the array.
   * Returns a new array sharing the same elements but with one
   * dimension fixed, with its size set to 1. When
   * \c a is an array of size [D0,…,DN], then
   * \c slice_for_dim(a,i,x) will be of size [D0,…,D(i-1),1,…DN].
   */
  StridedArray<T,N> slice_for_dim(dim_type dim, dim_type val) const
  {
    dims_type start, size, strides;

    start.fill(0);
    std::copy(this->m_dims.cbegin(), this->m_dims.cend(), size.begin());
    strides.fill(1);
      
    start[dim] = val;
    size[dim] = 1;
      
    return this->slice(Slice<dim_type, N>(start, size, strides));
  }

  StridedArray<T,N> operator()(const Slice<dim_type, N>& s) const
  { return this->slice(s); }

  /// Return a raw pointer to the data associated with the array.
  T* data()
  { return m_data; }

  /**
   * Return the immutable data associated with the array.
   */
  const T* data() const
  { return m_data; }

  /**
   * Return a new shared pointer to the data.
   */
  std::shared_ptr<T> shared_data() const
  {
    return m_shared_data;
  }

  /**
   * Apply a function to all the elements in the array.
   * \param f is a callable taking a path and an element, such as
   *        a std::function<void(Path&,T&)>.
   */
  template <typename UnaryOperation>
  void map(UnaryOperation f)
  {
    dims_type coords;
    for_looper<StridedArray<T,N>,0,UnaryOperation>(*this, coords, f);
  }
    
  /**
   * Apply a function to all the elements in the array.
   * \param f is a callable taking a path and an element, such as
   *        a std::function<void(Path&, const& T)>.
   */
  template <typename ConstMapOperation>
  void map(ConstMapOperation f) const
  {
    dims_type coords;
    const_for_looper<StridedArray<T,N>,0,ConstMapOperation>(*this, coords, f);
  }

  /**
   * Fill an array with an abstract array.
   */
  template <typename Array,
	    std::enable_if_t<is_indexable<Array>::value
			     && Array::ndim == N
			     && is_promotable<typename Array::dtype,T>::value>* = nullptr>
  void operator=(const Array& a)
  {
#ifndef NECOMI_NO_BOUND_CHECKS
    // Make sure the dimensions of a and b are the same
    if (this->dims() != a.dims()) {
      std::ostringstream msg;
      msg << "cannot copy from indexable array of different dimensions (";
      copy_dims(this->dims(), msg) << " != ";
      copy_dims(a.dims(), msg) << ")";
      throw std::length_error(msg.str());
    }
#endif
    this->map([&a](auto& path, auto& val) {
	val = a(path);
      });
  }

  template <typename Array,
	    std::enable_if_t<has_dtype<Array>::value
			     && ! has_ndim<Array>::value
			     && has_dims<Array>::value
			     && is_promotable<typename Array::dtype,T>::value>* = nullptr>
  void operator=(const Array& a)
  {
#ifndef NECOMI_NO_BOUND_CHECKS
    // Make sure the dimensions of a and b are the same
    if (ndim != a.ndim())
      throw std::length_error("cannot copy from variadic array of different dimensionality");
    if (! std::equal(this->dims().cbegin(), this->dims().cend(),
		     a.dims().cbegin()))
      throw std::length_error("cannot copy from variadic array of different dimensions");
#endif
    this->map([&a](const auto& coords, auto& val) {
	typename Array::dims_type acoords(coords.cbegin(), coords.cend());
	val = a(acoords);
      });
  }

  void operator=(const StridedArray<T,N>& a)
  {
    this->operator=<StridedArray<T,N>>(a);
  }

  void operator=(const T& value)
  {
    this->map([&value](const auto&, auto& val){ val = value; });
  }

  /**
   * Fill an entire array with a single value.
   */
  void fill(const T& val)
  {
    this->operator=(val);
  }

  /**
   * Construct a copy of an array.
   * All the elements are put in a contiguous region of memory
   * newly allocated with, initially, no other view on it.
   */
  StridedArray<T,N> copy() const
  {
    StridedArray<T,N> a(this->m_dims);
    this->map([&a](auto& path, auto&& val) {
	T v = val;
	a(path) = v;
      });
    return a;
  }


  // Iterator interface

  ArrayIterator<StridedArray<T,N>> begin()
  {
    return ArrayIterator<StridedArray<T,N>>(*this);
  }

  ArrayIterator<StridedArray<T,N>> end()
  {
    dims_type coords = this->dims();
    for (auto i = 0UL; i < ndim - 1; i++)
      coords[i]--;
    return ArrayIterator<StridedArray<T,N>>(*this, coords);
  }
  
protected:
  dims_type m_strides;
  std::shared_ptr<T> m_shared_data;
  T* m_data;
};

/**
 * Convert an abstract array to an immediate one with element casting.
 *
 * A new array with copied or casted elements is returned,
 * even if the original array already was an immediate with same
 * element type.
 */
template <typename U, typename From, typename T=typename From::dtype,
	  typename std::enable_if_t<std::is_convertible<T,U>::value>* = nullptr>
StridedArray<U, From::ndim> strided_array(const From& a)
{
  StridedArray<U, From::ndim> res(a.dims());
  res.map([&a](auto& coords, auto& val) {
      val = static_cast<U>(a(coords));
    });
  return res;
}

/**
 * Convert an abstract array to an immediate one with same element type.
 *
 * A new array with copied or casted elements is always returned.
 */
template <typename From, typename T=typename From::dtype>
StridedArray<T, From::ndim> strided_array(const From& a)
{
  return strided_array<T,From>(a);
}

template <typename From, typename T=typename From::dtype>
StridedArray<T, From::ndim> strided(const From& a)
{
  return strided_array<T,From>(a);
}

template <typename T>
StridedArray<T,1> litarray(const std::initializer_list<T>& lst)
{
  StridedArray<T,1> a(lst.size());
  std::copy_n(lst.begin(), lst.size(), a.data());
  return a;
}

template <typename ...T>
auto litarray(T... values)
{
  using U = typename std::common_type<T...>::type;
  std::initializer_list<U> lst{static_cast<U>(values)...};
  return litarray(lst);
}


} // namespace necomi

// Local Variables:
// mode: c++
// End:
