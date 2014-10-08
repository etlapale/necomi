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

  /**
   * Represent the coordinates to an element.
   */
  typedef std::array<ArrayIndex,N> Path;

protected:
  void build_strides() {
    if (N > 0) {
      auto prev = m_strides[N - 1] = 1;
      for (long i = N - 2; i >= 0; i--)
        prev = m_strides[i] = this->m_dims[i + 1] * prev;
    }
  }

public:
  /**
   * Create a new multi-dimensional array with uninitialized elements.
   */
  template <typename ...Dim,
            typename std::enable_if<sizeof...(Dim) == N && all_indices<Dim...>(),int>::type = 0>
  Array(Dim ...dims)
    : Array(std::array<ArrayIndex,N>({{static_cast<ArrayIndex>(dims)...}}))
  {}

  /**
   * Create a new multi-dimensional array with uninitialized elements.
   */
  Array(const std::array<ArrayIndex,N>& dims)
    : Parent(dims)
    , m_shared_data(new T[Parent::size()], [](T* p){ delete [] p; })
    , m_data(m_shared_data.get())
  {
    build_strides();
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
  template <typename ...Dim,
            typename std::enable_if<sizeof...(Dim) == N && all_indices<Dim...>(),int>::type = 0>
  Array(T* data, Dim ...dims)
    : Array(data, std::array<ArrayIndex,N>({{static_cast<ArrayIndex>(dims)...}}))
  {}

  /**
   * Construct an array from already existing data.
   * The given data is never destroyed.
   */
  Array(T* data, const std::array<ArrayIndex,N>& dims)
    : Parent(dims)
    , m_shared_data(data, [](T* p){(void) p;})
    , m_data(data)
  {
    build_strides();
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
   */
  Array<T,N-1> operator[](ArrayIndex index) const
  {
    std::array<ArrayIndex,N-1> dims;
    std::copy(std::next(this->m_dims.cbegin()),
	      this->m_dims.cend(), dims.begin());

    Array<T,N-1> a(dims);
    a.m_data = &(m_data[index * m_strides[0]]);
    std::copy(std::next(m_strides.cbegin()), m_strides.cend(),
              a.m_strides.begin());
    
    return a;
  }

  /**
   * Convert a multi-dimensional position into an offset from
   * the beginning of the data (m_data).
   */
  ArrayIndex index(const Path& path) const
  {
    return std::inner_product(path.cbegin(), path.cend(),
                              m_strides.cbegin(), 0);
  }

  /**
   * Convert a multi-dimensional position into an offset from
   * the beginning of the data (m_data).
   */
  template <typename ...Indices>
  ArrayIndex index(Indices... indices) const
  {
    static_assert(sizeof...(Indices) == N, "improper indices arity");
    static_assert(all_indices<Indices...>(), "invalid indices type");

    std::array<ArrayIndex,N> idx {{static_cast<ArrayIndex>(indices)...}};
    return index(idx);
  }

  /**
   * Return a reference to a single element.
   */
  template <typename ...Indices>
  T& operator()(Indices... indices)
  {
    // TODO check indices for out of bounds
    return m_data[index(indices...)];
  }

  /**
   * Return the value of a single element.
   */
  template <typename ...Indices>
  T operator()(Indices... indices) const
  {
    // TODO check indices for out of bounds
    return m_data[index(indices...)];
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
   * Check if a predicate is true for any element in the array.
   * Return as soon as one matching element is found.
   */
  template <typename Predicate>
  bool any(Predicate p) const
  {
    std::array<ArrayIndex,N> path;
    return breakable_for_looper<Predicate,0,Array<T,N>,T,N>(*this, path, p);
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
   * Fill an array with a delayed array.
   */
  template <typename Expr>
  void operator=(const DelayedArray<T,N,Expr>& a)
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
   * Create an array with the same dimensions filled with a constant
   * value.
   * \see zeros_like
   */
  Array<T,N> constants_like(const T& value) const
  {
    Array<T,N> a(this->m_dims);
    std::fill_n(a.data(), a.size(), value);
    return a;
  }

  /**
   * Create an a array with the same dimensions but filled with zeros.
   * \see constants_like
   */
  Array<T,N> zeros_like() const
  {
    return constants_like(0);
  }

  /**
   * Sum an array along a given dimension.
   */
  std::enable_if_t<true,Array<T,N-1>>
  sum(ArrayIndex dim) const
  {
    // Compute the dimensions of the new array
    std::array<ArrayIndex,N-1> dims;
    auto oit = std::copy_n(this->m_dims.cbegin(), dim, dims.begin());
    if (dim != N-1)
      std::copy(this->m_dims.cbegin()+dim+1, this->m_dims.cend(), oit);

    Array<T,N-1> a(dims);
    std::array<ArrayIndex,N> orig_path;
    a.map([&](auto& path, auto& val) {
        // Compute the index in the original array
        auto oit = std::copy_n(path.cbegin(), dim, orig_path.begin());
        if (dim != N-1)
          std::copy(path.cbegin()+dim, path.cend(), oit+1);

        val = 0;
        for (ArrayIndex i = 0; i < this->m_dims[dim]; i++) {
          orig_path[dim] = i;
          val += m_data[this->index(orig_path)];
        }
      });
    return a;
  }

  /**
   * Divide each element of the array by a number.
   */
  template <typename U>
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
}

// Local Variables:
// mode: c++
// End:
