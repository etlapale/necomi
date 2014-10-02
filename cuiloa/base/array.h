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
#include <array>
#include <exception>
#include <iostream>
#include <iterator>
#include <memory>
#include <numeric>
#include <string>

template <std::size_t N>
std::ostream&
operator<<(std::ostream& os, const std::array<unsigned int,N>& a)
{
  for (std::size_t i = 0; i < N-1; i++)
    os << a[i] << ',';
  return os << a[N-1];
}

namespace cuiloa
{

/**
 * Standard type to denote coordinate indices or dimensions.
 */
typedef unsigned int ArrayIndex;

typedef int ArrayOffset;


/**
 * Checks if a pack of types are all valid array indexes.
 */
template <typename ...Indices>
struct all_indices;

template <>
struct all_indices<> : std::true_type
{};

template <typename Index, typename ...Indices>
struct all_indices<Index, Indices...>
  : std::integral_constant<bool,
        std::is_convertible<Index, ArrayIndex>::value &&
        all_indices<Indices...>::value>
{};

template <typename T, ArrayIndex N> class Array;

#ifndef IN_DOXYGEN
/**
 * Final case of for loops through template metaprogramming.
 *
 * \ingroup core
 * \see Array::map
 */
template <typename UnaryOperation, typename T, ArrayIndex N, ArrayIndex M>
std::enable_if_t<M==N>
for_looper(Array<T,N>& a,
           std::array<ArrayIndex,N>& path,
           UnaryOperation f)
{
  T* data = a.data();
  auto idx = a.index(path);
  f(path, data[idx]);
}

/**
 * Recursion case of for loops through template metaprogramming.
 *
 * \ingroup core
 * \see Array::map
 */
template <typename UnaryOperation, typename T, ArrayIndex N, ArrayIndex M>
std::enable_if_t<M<N>
for_looper(Array<T,N>& a,
           std::array<ArrayIndex,N>& path,
           UnaryOperation f)
{
  for (ArrayIndex i = 0; i < a.dimensions()[M]; i++) {
    path[M] = i;
    for_looper<UnaryOperation,T,N,M+1>(a, path, f);
  }
}

/**
 * Final case of for loops through template metaprogramming
 * for constant arrays.
 *
 * \ingroup core
 * \see Array::map
 */
template <typename UnaryOperation, typename T, ArrayIndex N, ArrayIndex M>
std::enable_if_t<M==N>
for_looper(const Array<T,N>& a,
           std::array<ArrayIndex,N>& path,
           UnaryOperation f)
{
  const T* data = a.data();
  auto idx = a.index(path);
  f(path, data[idx]);
}

/**
 * Recursion case of for loops through template metaprogramming
 * for constant arrays.
 *
 * \ingroup core
 * \see Array::map
 */
template <typename UnaryOperation, typename T, ArrayIndex N, ArrayIndex M>
std::enable_if_t<M<N>
for_looper(const Array<T,N>& a,
           std::array<ArrayIndex,N>& path,
           UnaryOperation f)
{
  for (ArrayIndex i = 0; i < a.dimensions()[M]; i++) {
    path[M] = i;
    for_looper<UnaryOperation,T,N,M+1>(a, path, f);
  }
}

/**
 * Final case of brekable for loops through template metaprogramming
 * for constant arrays.
 *
 * \ingroup core
 */
template <typename Predicate, typename T, ArrayIndex N, ArrayIndex M>
std::enable_if_t<M==N,bool>
breakable_for_looper(const Array<T,N>& a,
		     std::array<ArrayIndex,N>& path,
		     Predicate p)
{
  const T* data = a.data();
  auto idx = a.index(path);
  return p(data[idx]);
}

/**
 * Recursion case of for loops through template metaprogramming
 * for constant arrays.
 *
 * \ingroup core
 * \see Array::map
 */
template <typename Predicate, typename T, ArrayIndex N, ArrayIndex M>
std::enable_if_t<(M<N),bool>
breakable_for_looper(const Array<T,N>& a,
		     std::array<ArrayIndex,N>& path,
		     Predicate p)
{
  for (ArrayIndex i = 0; i < a.dimensions()[M]; i++) {
    path[M] = i;
    bool ret = breakable_for_looper<Predicate,T,N,M+1>(a, path, p);
    if (ret) return true;
  }
  return false;
}
#endif  // IN_DOXYGEN

/**
 * Iterator over an array.
 * \ingroup core
 */
template <typename T, ArrayIndex N>
class ArrayIterator
{
public:
  ArrayIterator(Array<T,N>& array)
    : m_array(array)
    , m_dims(array.dimensions())
  {
    m_path.fill(0);
  }

  ArrayIterator(Array<T,N>& array, const std::array<ArrayIndex,N>& path)
    : m_array(array)
    , m_dims(array.dimensions())
    , m_path(path)
  {}

  const std::array<ArrayIndex,N> path() const
  {
    return m_path;
  }

  T& operator*()
  {
    return m_array(m_path);
  }

  T* operator->()
  {
    return &(operator*());
  }

  ArrayIterator<T,N>& operator++()
  {
    // Find first dimension to increment
    for (int i = N-1; i >= 0; i--) {
      if (i == 0 || m_path[i] < m_dims[i] - 1) {
      //if (i == 0 || m_path[i] < m_array.dimensions()[i] - 1) {
        m_path[i]++;
        break;
      }
      else {
        m_path[i] = 0;
      }
    }
    return *this;
  }

  bool operator!=(const ArrayIterator<T,N>& other)
  {
    return &m_array != &other.m_array || m_path != other.m_path;
  }

protected:
  Array<T,N>& m_array;
  std::array<ArrayIndex,N> m_dims;
  std::array<ArrayIndex,N> m_path;
};

template <typename Expr, typename T, ArrayIndex N>
class DelayedArray;

/**
 * The base class of all Array<T,N>.
 */
class BaseArray
{
};

/**
 * Multi-dimensional arrays allowing shared data and non-contiguous regions.
 *
 * Arrays can have any number of dimensions, including 0 which represents
 * simple scalars. 
 *
 * ~~~
 * cuiloa::Array<double,0> a0;     // Represent a simple double scalar
 * cuiloa::Array<int,1> a1(12);    // 1D array (vector)
 * cuiloa::Array<int,2> a2(4,3);   // 2D array (matrix)
 * ~~~
 *
 * Elements are directly accessible through the `()` operator, by specifying
 * their coordinates.
 *
 * ~~~
 * double d = a0() + a1(3) + a2(1,2);
 * ~~~
 *
 * Interfacing with other libraries is possible by fetching the underlying
 * elements array with \ref data(). It is also possible to construct an
 * Array instance to wrap a raw array of elements with the constructors
 * taking a `T` and dimensions as argument.
 *
 * ~~~
 * double f1[12];
 * cuiloa::Array<double,2> f2(a, 3, 4); // f1 and f2 share their elements
 * ~~~
 *
 * A standard `iterator` interface is offered, but users are encouraged to
 * use the fast \ref map functions.
 *
 * \ingroup core
 */
template <typename T, ArrayIndex N>
class Array : public BaseArray
{
public:
  template <typename U, ArrayIndex M> friend class Array;

  /**
   * Represent the coordinates to an element.
   */
  typedef std::array<ArrayIndex,N> Path;

  typedef std::forward_iterator_tag iterator_category;
  typedef ArrayIterator<T,N> iterator;
  typedef ArrayOffset difference_type;
  typedef ArrayIndex size_type;
  typedef T value_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;

protected:
  void build_strides() {
    if (N > 0) {
      auto prev = m_strides[N - 1] = 1;
      for (int i = N - 2; i >= 0; i--)
        prev = m_strides[i] = m_dims[i + 1] * prev;
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
    : m_dims(dims)
    , m_shared_data(new T[size()], [](T* p){ delete [] p; })
    , m_data(m_shared_data.get())
  {
    build_strides();
  }

  /**
   * Create a shared view of the given array.
   */
  Array(const Array<T,N>& src)
    : m_dims(src.m_dims)
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
    : m_dims(dims)
    , m_shared_data(data, [](T* p){(void) p;})
    , m_data(data)
  {
    build_strides();
  }

  const std::array<ArrayIndex,N>& dimensions() const
  { return m_dims; }

  const std::array<ArrayIndex,N>& strides() const
  { return m_strides; }
  
  /**
   * Return the number of elements in the array.
   */
  ArrayIndex size() const
  {
    return std::accumulate(m_dims.cbegin(), m_dims.cend(), 1,
        [] (ArrayIndex a, ArrayIndex b) { return a * b; });
  }

  /**
   * Checks whether the array is contiguous.
   */
  bool
  contiguous() const
  {
    if (N == 0)
      return true;

    ArrayIndex prev = 1;
    for (int i = N - 1; i >= 0; i--) {
      if (m_strides[i] != prev)
        return false;
      prev *= m_dims[i];
    }
    return true;
  }

#if 0
  /**
   * Return a shared view of the array.
   * The view will be of the given size starting at the desired
   * position.
   */
  //Array<T,n> view(unsigned int* dims, unsigned int* offset) const;

#endif
  /**
   * Return a view on a slice of the array.
   */
  Array<T,N-1> operator[](ArrayIndex index)
  {
    std::array<ArrayIndex,N-1> dims;
    std::copy(std::next(m_dims.cbegin()), m_dims.cend(), dims.begin());

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


  // Iterators
  
  iterator begin()
  {
    return iterator(*this);
  }

  iterator end()
  {
    Path path = m_dims;
    std::transform(path.begin(), path.end(), path.begin(),
                   [](auto val) { return val - 1; });
    return ++iterator(*this, path);
  }

  /**
   * Check if a predicate is true for any element in the array.
   * Return as soon as one matching element is found.
   */
  template <typename Predicate>
  bool any(Predicate p) const
  {
    std::array<ArrayIndex,N> path;
    return breakable_for_looper<Predicate,T,N,0>(*this, path, p);
  }

  /**
   * Apply a function to all the elements in the array.
   * \param f is a callable taking a path and an element, such as
   *        a std::function<void(Path&,T&)>.
   */
  template <typename UnaryOperation>
  void map(UnaryOperation f)
  {
    std::array<ArrayIndex,N> path;
    for_looper<UnaryOperation,T,N,0>(*this, path, f);
  }

  /**
   * Apply a function to all the elements in the array.
   * \param f is a callable taking a path and an element, such as
   *        a std::function<void(Path&, const& T)>.
   */
  template <typename UnaryOperation>
  void map(UnaryOperation f) const
  {
    std::array<ArrayIndex,N> path;
    for_looper<UnaryOperation,T,N,0>(*this, path, f);
  }

  /**
   * Fill an entire array with a single value.
   */
  void fill(const T& val)
  {
    map([&val](auto& path, auto& old) {
        (void) path; (void) old;
        old = val;
      });
  }

  /**
   * Fill an array with a delayed array.
   */
  template <typename Expr>
  void operator=(const DelayedArray<Expr,T,N>& a)
  {
    map([&a](auto& path, auto& val) {
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
    Array<T,N> a(m_dims);
    map([&a](auto& path, auto& val) {
        a(path) = val;
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
    Array<T,N> a(m_dims);
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
   * Sum all the elements of an array.
   * \warning This may overflow.
   */
  T sum() {
    T total = 0;
    map([&](auto& path, auto& val) {
        (void) path;
        total += val;
      });
    return total;
  }

  /**
   * Sum an array along a given dimension.
   */
  std::enable_if_t<true,Array<T,N-1>>
  sum(ArrayIndex dim) const
  {
    // Compute the dimensions of the new array
    std::array<ArrayIndex,N-1> dims;
    auto oit = std::copy_n(m_dims.cbegin(), dim, dims.begin());
    if (dim != N-1)
      std::copy(m_dims.cbegin()+dim+1, m_dims.cend(), oit);

    Array<T,N-1> a(dims);
    std::array<ArrayIndex,N> orig_path;
    a.map([&](auto& path, auto& val) {
        // Compute the index in the original array
        auto oit = std::copy_n(path.cbegin(), dim, orig_path.begin());
        if (dim != N-1)
          std::copy(path.cbegin()+dim, path.cend(), oit+1);

        val = 0;
        for (ArrayIndex i = 0; i < m_dims[dim]; i++) {
          orig_path[dim] = i;
          val += m_data[this->index(orig_path)];
        }
      });
    return a;
  }

  /**
   * Divide each element of the array by a number.
   * \warning Follows standard C++ rules.
   */
  template <typename U>
  Array<T,N>&
  operator/=(const U& div)
  {
    map([&div](auto& path, T& val) {
        (void) path;
        val /= div;
      });
    return *this;
  }

  #if 0
  /**
   * Element wise multiplication of two arrays.
   */
  Array<T,N> operator*(const Array<T,N>& other) {
    Array<T,N> res(m_dims);
    res.map([&other](auto& path, auto& val) {

      });
    res.m_data[0] = 42;

    return res;
  }
  #endif

protected:
  std::array<ArrayIndex,N> m_dims;
  std::array<ArrayIndex,N> m_strides;

  std::shared_ptr<T> m_shared_data;
  T* m_data;
};


#if 0
template <typename T, unsigned int n>
  Array<T,n>
  Array<T,n>::view(unsigned int* dims, unsigned int* offset) const
  {
    Array<T,n> a(*this);

    memcpy (a.m_dims, dims, n * sizeof(unsigned int));
    a.m_data = &(m_data[this->index(offset)]);

    return a;
  }
#endif
}
