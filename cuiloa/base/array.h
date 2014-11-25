/*
 * Copyright © 2014	University of California, Irvine
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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


namespace cuiloa
{
  /**
   * Multi-dimensional arrays supporting non-contiguous shared data.
   */
  template <typename T, ArrayIndex N>
  class Array : public AbstractArray<Array<T,N>,T,N>
  {
  public:
    template <typename U, ArrayIndex M> friend class Array;

    /// Short name for the parent class
    typedef AbstractArray<Array<T,N>,T,N> Parent;

  public:
    /**
     * Create a new multi-dimensional array with uninitialized elements.
     */
    template <typename ...Dim,
	      typename std::enable_if_t<sizeof...(Dim) == N && all_indices<Dim...>()>* = nullptr>
      Array(Dim ...dims)
      : Array(std::array<ArrayIndex,N>({{static_cast<ArrayIndex>(dims)...}}))
    {}

    /**
     * Create a new multi-dimensional array with uninitialized elements.
     */
    Array(const std::array<ArrayIndex,N>& dims)
      : Parent(dims)
      , m_strides(default_strides(dims))
      , m_shared_data(new T[Parent::size()], [](T* p){ delete [] p; })
      , m_data(m_shared_data.get())
    {
    }

    /**
     * Create a shared view of the given array.
     */
    Array(const Array<T,N>& src)
      : Parent(src.m_dims)
      , m_strides(src.m_strides)
      , m_shared_data(src.m_shared_data)
      , m_data(src.m_data)
    {
    }

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
      : Array(a.dimensions())
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
#ifndef CUILOA_NO_BOUND_CHECKS
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

    /**
     * Convert a multi-dimensional position into an offset from
     * the beginning of the data (m_data).
     */
    ArrayIndex index(const Coordinates<N>& path) const
    {
      return std::inner_product(path.cbegin(), path.cend(),
				m_strides.cbegin(), 0);
    }

    /**
     * Convert a multi-dimensional position into an offset from
     * the beginning of the data (m_data).
     */
    template <typename ...Indices>
    std::enable_if_t<sizeof...(Indices) == N && all_indices<Indices...>(),
                     ArrayIndex>
    index(Indices... indices) const
    {
      std::array<ArrayIndex,N> idx {{static_cast<ArrayIndex>(indices)...}};
      return index(idx);
    }

    T& operator()(const Coordinates<N>& path) {
      return m_data[index(path)];
    }

    /**
     * Return a reference to a single element.
     */
    template <typename ...Indices>
    std::enable_if_t<sizeof...(Indices) == N && all_indices<Indices...>(),
                     T&>
    operator()(Indices... indices)
    {
      // TODO check indices for out of bounds
      return m_data[index(indices...)];
    }

    const T& operator()(const Coordinates<N>& coords) const {
      return m_data[index(coords)];
    }

    /**
     * Return the value of a single element.
     */
    template <typename ...Indices>
    std::enable_if_t<sizeof...(Indices) == N && all_indices<Indices...>(),
                     const T&>
    operator()(Indices... indices) const
    {
      // TODO check indices for out of bounds
      return m_data[index(indices...)];
    }

    /**
     * Return a restricted view on the array.
     */
    Array<T,N> slice(const Slice<N>& s)  const
    {
      Array<T,N> a(*this);
#ifndef CUILOA_NO_BOUND_CHECKS
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

    Array<T,N> operator()(const Slice<N>& s) const
    {
      return this->slice(s);
    }

    /**
     * Return the data associated with the array.
     */
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
     * Fill an entire array with a single value.
     */
    void fill(const T& val)
    {
      this->map([&val](auto& path, auto& old) {
	  (void) path;
	  old = val;
	});
    }
    
    void operator=(const Array<T,N>& a)
    {
      this->operator=(static_cast<const AbstractArray<Array<T,N>,T,N>&>(a));
    }

    /**
     * Fill an array with an abstract array.
     */
    template <typename Concrete>
    void operator=(const AbstractArray<Concrete,T,N>& a)
    {
#ifndef CUILOA_NO_BOUND_CHECKS
      // Make sure the dimensions of a and b are the same
      if (this->dimensions() != a.dimensions())
	throw std::length_error("cannot copy from array with different dimensions");
#endif
      this->map([&a](auto& path, auto& val) {
	  val = a(path);
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
  template <typename Concrete, typename T, ArrayIndex N>
  Array<T,N> cumsum(const AbstractArray<Concrete,T,N>& a,
		    ArrayIndex dim = 0) {
    Array<T,N> res(a.dimensions());
    a.map([&res,dim](auto& path, auto val) {
	if (path[dim] == 0) {
	  res(path) = val;
	}
	else {
	  auto prev = path;
	  prev[dim]--;
	  res(path) = res(prev) + val;
	}
      });
    return res;
  }

  template <typename T, ArrayIndex N, typename Concrete>
  Array<T,N>& operator+=(Array<T,N>& a, const AbstractArray<Concrete,T,N>& b)
  {
#ifndef CUILOA_NO_BOUND_CHECKS
    // Make sure the dimensions of a and b are the same
    if (a.dimensions() != b.dimensions())
      throw std::length_error("cannot increment with array of different dimensions");
#endif
    a.map([&b](auto& path, auto& val) {val += b(path);});
    return a;
  }

  /**
   * Convert an abstract array to an immediate one with element casting.
   *
   * A new array with copied or casted elements is always returned.
   */
  template <typename To, typename From, ArrayIndex N, typename Concrete,
            typename std::enable_if_t<std::is_convertible<From,To>::value>* = nullptr>
  Array<To,N> immediate(const AbstractArray<Concrete,From,N>& a) {
    Array<To,N> res(a.dimensions());
    res.map([&a](auto& coords, auto& val) {
	val = static_cast<To>(a(coords));
      });
    return res;
  }

  /**
   * Convert an abstract array to an immediate one with same element type.
   *
   * A new array with copied or casted elements is always returned.
   */
  template <typename From, ArrayIndex N, typename Concrete>
  Array<From,N> immediate(const AbstractArray<Concrete,From,N>& a)
  {
    return immediate<From,From,N,Concrete>(a);
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
  
  template <typename Concrete, typename T, ArrayDimension N, typename U,
            std::enable_if_t<std::is_convertible<U,T>::value>* = nullptr>
  Array<T,N>& operator/=(Array<T,N>& numerator,
			 const AbstractArray<Concrete,U,N>& denominator)
  {
    numerator.map([&denominator](auto& coords, auto& val) {
	val /= denominator(coords);
      });
    return numerator;
  }
  
  namespace broadcasting
  {
    template <typename T, ArrayDimension N,
	      typename Concrete, typename U, ArrayDimension M,
	      std::enable_if_t<(N>M) && std::is_convertible<U,T>::value>* = nullptr>
    Array<T,N>& operator/=(Array<T,N>& numerator,
			   const AbstractArray<Concrete,U,M>& denominator)
    {
      return cuiloa::operator/=(numerator,
				widen(numerator.dimensions(), denominator));
    }
  } // namespace broadcasting
}

// Local Variables:
// mode: c++
// End:
