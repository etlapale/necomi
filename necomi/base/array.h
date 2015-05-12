// necomi/base/array.h – Immediate arrays
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <algorithm>
#include <exception>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>

#include "basearray.h"
#include "delayed.h"
#include "slices.h"


namespace necomi
{
  /**
   * Multi-dimensional arrays supporting non-contiguous shared data.
   */
  template <typename T, ArrayIndex N>
  class Array : public DimArray<std::size_t, N>
  {
  public:
    using dim_type = std::size_t;
    using dims_type = std::array<dim_type, N>;
    using dtype = T;
    enum { ndim = N };
    
    template <typename U, ArrayIndex M> friend class Array;

    /// Short name for the parent class
    typedef DimArray<std::size_t, N> Parent;

  public:
    /**
     * Create a new multi-dimensional array with uninitialized elements.
     */
    template <typename ...Dim,
	      typename std::enable_if_t<sizeof...(Dim) == N && all_indices<Dim...>()>* = nullptr>
      Array(Dim ...dims)
      : Array(dims_type{static_cast<ArrayIndex>(dims)...})
    {}

    /**
     * Create a new multi-dimensional array with uninitialized elements.
     */
    Array(const dims_type& dims)
      : Parent(dims)
      , m_strides(default_strides(dims))
      , m_shared_data(new T[size(*this)], [](T* p){ delete [] p; })
      , m_data(m_shared_data.get())
    {}

    /**
     * Create a shared view of the given array.
     */
    Array(const Array<T,N>& src)
      : Parent(src.m_dims)
      , m_strides(src.m_strides)
      , m_shared_data(src.m_shared_data)
      , m_data(src.m_data)
    {}

    /**
     * Construct an array from already existing data.
     * The given data is never destroyed.
     */
    template <typename ...Dim, typename Deleter,
	      typename std::enable_if<sizeof...(Dim) == N && all_indices<Dim...>(),int>::type = 0>
      Array(T* data, Deleter deleter, Dim ...dims)
      : Array(data, std::array<ArrayIndex,N>({{static_cast<ArrayIndex>(dims)...}}), deleter)
    {}

    /**
     * Construct an array from already existing data.
     */
    template <typename Deleter>
    Array(T* data,
	  const std::array<ArrayIndex,N>& dims,
	  Deleter deleter)
      : Parent(dims)
      , m_strides(default_strides(dims))
      , m_shared_data(data, deleter)
      , m_data(data)
    {
    }

    Array(std::shared_ptr<T> shared_data,
	  T* data,
	  std::array<ArrayIndex,N> strides,
	  const std::array<ArrayIndex,N>& dims)
      : Parent(dims)
      , m_strides(std::move(strides))
      , m_shared_data(std::move(shared_data))
      , m_data(data)
    {
    }

    /**
     * Create an immediate array initialized from a delayed one.
     */
    template <typename Expr>
    Array(const DelayedArray<T,N,Expr>& a)
      : Array(a.dims())
    {
      this->operator=(a);
    }

    const std::array<ArrayIndex,N>& strides() const
    { return m_strides; }

    /**
     * Checks whether the array is contiguous.
     */
    bool contiguous() const
    {
      if (N == 0)
	return true;

      ArrayIndex prev = 1;
      for (int i = N - 1; i >= 0; i--) {
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
    template <ArrayIndex DepN = N,
	      typename std::enable_if_t<DepN!=0>* = nullptr>
    Array<T,DepN-1> operator[](ArrayIndex index) const
    {
#ifndef NECOMI_NO_BOUND_CHECKS
      // Check slicing index
      if (index > this->m_dims[0])
	throw std::out_of_range("invalid slicing index");
#endif

      // New dimensions
      std::array<ArrayIndex,N-1> dims;
      std::copy(std::next(this->m_dims.cbegin()), this->m_dims.cend(),
		dims.begin());
      // New strides
      std::array<ArrayIndex,N-1> strides;
      std::copy(std::next(m_strides.cbegin()), m_strides.cend(),
		strides.begin());

      Array<T,N-1> a(m_shared_data, &(m_data[index * m_strides[0]]),
		     strides, dims);
      //a.m_data = &(m_data[index * m_strides[0]]);
      //std::copy(std::next(m_strides.cbegin()), m_strides.cend(),
      //	a.m_strides.begin());
    
      return a;
    }

    T& operator()(const dims_type& coords)
    { return m_data[index(*this, coords)];}

    const T& operator()(const dims_type& coords) const
    { return m_data[index(*this, coords)]; }
    
    template <typename ...Coords,
	      std::enable_if_t<sizeof...(Coords) == N && all_convertible<Coords...,dim_type>::value>* = nullptr>
    T& operator()(Coords... coords)
    {
      // TODO check indices for out of bounds
      return m_data[index(*this, coords...)];
    }
    
    template <typename ...Coords,
	      std::enable_if_t<sizeof...(Coords) == N && all_convertible<Coords...,dim_type>::value>* = nullptr>
    const T& operator()(Coords... coords) const
    {
      // TODO check indices for out of bounds
      return m_data[index(*this, coords...)];
    }

    /**
     * Return a restricted view on the array.
     */
    Array<T,N> slice(const Slice<N>& s) const
    {
      Array<T,N> a(*this);
#ifndef NECOMI_NO_BOUND_CHECKS
      for (ArrayIndex i = 0; i < N; i++) {
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
    Array<T,N> slice_for_dim(ArrayIndex dim, ArrayIndex val) const
    {
      std::array<ArrayIndex,N> start, size, strides;

      start.fill(0);
      std::copy(this->m_dims.cbegin(), this->m_dims.cend(), size.begin());
      strides.fill(1);
      
      start[dim] = val;
      size[dim] = 1;
      
      return this->slice(Slice<N>(start, size, strides));
    }

    Array<T,N> operator()(const Slice<N>& s) const
    {
      return this->slice(s);
    }

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
      Coordinates<N> path;
      for_looper<UnaryOperation,0,Array>(*this, path, f);
    }
    
    /**
     * Apply a function to all the elements in the array.
     * \param f is a callable taking a path and an element, such as
     *        a std::function<void(Path&, const& T)>.
     */
    template <typename ConstMapOperation>
    void map(ConstMapOperation f) const
    {
      Coordinates<N> path;
      const_for_looper<ConstMapOperation,0,Array<T,N>>(*this, path, f);
    }


    /**
     * Fill an entire array with a single value.
     */
    void fill(const T& val)
    {
      this->map([&val](auto& path, auto& old) {
	  (void) path;
	  old = val;
	});
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
      if (this->dims() != a.dims())
	throw std::length_error("cannot copy from array with different dimensions");
#endif
      this->map([&a](auto& path, auto& val) {
	  val = a(path);
	});
    }

    void operator=(const Array<T,N>& a)
    {
      this->operator=<Array<T,N>>(a);
    }

    void operator=(const T& value)
    {
      this->map([&value](const auto&, auto& val) {
	  val = value;
	});
    }

    /**
     * Construct a copy of an array.
     * All the elements are put in a contiguous region of memory
     * newly allocated with, initially, no other view on it.
     */
    Array<T,N> copy() const
    {
      Array<T,N> a(this->m_dims);
      this->map([&a](auto& path, auto&& val) {
	  T v = val;
	  a(path) = v;
	});
      return a;
    }

    /**
     * Divide each element of the array by a number.
     */
    template <typename U,
	      std::enable_if_t<is_promotable<U,T>::value>* = nullptr>
    Array<T,N>&
    operator/=(const U& div)
    {
      this->map([&div](auto& path, T& val) {
	  (void) path;
	  val /= div;
	});
      return *this;
    }

  protected:
    std::array<ArrayIndex,N> m_strides;
    std::shared_ptr<T> m_shared_data;
    T* m_data;
  };


/**
 * Cumulative sum.
 */
template <typename Indexable, typename T=typename Indexable::dtype>
Array<T,Indexable::ndim> cumsum(const Indexable& a, ArrayIndex dim = 0)
{
  Array<T,Indexable::ndim> res(a.dims());
  res.map([&res,dim,&a](auto& path, auto& valx) {
      if (path[dim] == 0) {
	valx = a(path);
      }
      else {
	auto prev = path;
	prev[dim]--;
	valx = res(prev) + a(path);
      }
    });
  return res;
}

template <typename Indexable, typename T=typename Indexable::dtype>
Array<T,Indexable::ndim>& operator+=(Array<T,Indexable::ndim>& a,
				     const Indexable& b)
{
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions of a and b are the same
  if (a.dims() != b.dims())
    throw std::length_error("cannot increment with array of different dimensions");
#endif
  a.map([&b](auto& path, auto& val) {val += b(path);});
  return a;
}

/**
 * Convert an abstract array to an immediate one with element casting.
 *
 * A new array with copied or casted elements is returned,
 * even if the original array already was an immediate with same
 * element type.
 */
template <typename U, typename From, typename T=typename From::dtype,
	  typename std::enable_if_t<std::is_convertible<T,U>::value>* = nullptr>
Array<U, From::ndim> immediate(const From& a)
{
  Array<U, From::ndim> res(a.dims());
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
Array<T, From::ndim> immediate(const From& a)
{
  return immediate<T,From>(a);
}

  template <typename T=double,
	    typename ...Values,
            typename std::enable_if_t<all_convertible<T,Values...>::value>* = nullptr>
  Array<T,1> litarray(Values... values)
  {
    Array<T,1> a(sizeof...(Values));
    std::initializer_list<T> vals = {static_cast<T>(values)...};
    std::copy_n(vals.begin(), sizeof...(Values), a.data());
    return a;
  }

template <typename T, ArrayDimension N,
	  typename Indexable, typename U=typename Indexable::dtype,
	  std::enable_if_t<is_promotable<U,T>::value
			   && N==Indexable::ndim>* = nullptr>
Array<T,N>& operator*=(Array<T,N>& numerator, const Indexable& denominator)
{
  numerator.map([&denominator](auto& coords, auto& val) {
      val *= denominator(coords);
    });
  return numerator;
}

template <typename T, ArrayDimension N,
	  typename Indexable, typename U=typename Indexable::dtype,
	  std::enable_if_t<is_promotable<U,T>::value
			   && N==Indexable::ndim>* = nullptr>
Array<T,N>& operator/=(Array<T,N>& numerator, const Indexable& denominator)
{
  numerator.map([&denominator](auto& coords, auto& val) {
      val /= denominator(coords);
    });
  return numerator;
}
  
namespace broadcasting
{
template <typename T, ArrayDimension N,
	  typename Indexable, typename U=typename Indexable::dtype,
	  std::enable_if_t<(N>Indexable::ndim) && is_promotable<U,T>::value>* = nullptr>
Array<T,N>& operator/=(Array<T,N>& numerator, const Indexable& denominator)
{
  return necomi::operator/=(numerator,
			    widen(numerator.dims(), denominator));
}

} // namespace broadcasting
} // namespace necomi

// Local Variables:
// mode: c++
// End:
