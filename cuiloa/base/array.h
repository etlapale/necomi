#pragma once

#include <string.h>

#include <array>
#include <exception>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>

#include <stdarg.h>

namespace cuiloa
{

typedef unsigned int ArrayIndex;

/**
 * Indicate an indexing error in an Array.
 */
class ArrayIndexException : public std::exception
{
public:
  ArrayIndexException(const std::string & msg) { m_msg = msg; }
  ~ArrayIndexException() throw() {}
  virtual const char* what() const throw() { return m_msg.c_str(); }
protected:
  std::string m_msg;
};
  

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

/*
 * Allows arbitrary for-loop nesting.
 */
template <typename T, ArrayIndex N, ArrayIndex M>
std::enable_if_t<M==N>
for_looper(Array<T,N>& a,
           std::array<ArrayIndex,N>& path,
           const std::function<T(const std::array<ArrayIndex,N>&, const T&)>& f)
{
  T* data = a.data();
  auto idx = a.index(path);
  data[idx] = f(path, data[idx]);
}

template <typename T, ArrayIndex N, ArrayIndex M>
std::enable_if_t<M<N>
for_looper(Array<T,N>& a,
           std::array<ArrayIndex,N>& path,
           const std::function<T(const std::array<ArrayIndex,N>&, const T&)>& f)
{
  for (ArrayIndex i = 0; i < a.dimensions()[M]; i++) {
    path[M] = i;
    for_looper<T,N,M+1>(a, path, f);
  }
}


/**
 * Multi-dimensional contiguous arrays allowing shared data.
 * 
 * Arrays created via the copy constructor, the operator[] or any
 * other operation on a \c const Array will share the same data as
 * their source. To get a real copy of an array, use the
 * Array::copy() method.
 * The \c const qualifier is only there to say the array view cannot be
 * changed (unmodifiable view dimensions, target data, ...). Unmodifiable
 * data should be marked as such in the template parameter.
 */
template <typename T, ArrayIndex N>
class Array
{
  template <typename U, ArrayIndex M> friend class Array;

  typedef std::array<ArrayIndex,N> Path;
public:
  /**
   * Create a new multi-dimensional array with uninitialized elements.
   */
  template <typename ...Dim>
  Array(Dim ...dims)
    : Array(std::array<ArrayIndex,N>({{static_cast<ArrayIndex>(dims)...}}))
  {
    // Duplicate of m_dims to get a better message
    static_assert(sizeof...(Dim) == N, "improper dimensions arity");
    static_assert(all_indices<Dim...>(), "invalid dimensions type");
  }

  /**
   * Create a new multi-dimensional array with uninitialized elements.
   */
  Array(const std::array<ArrayIndex,N>& dims)
    : m_dims(dims)
    , m_shared_data(new T[size()], [](T* p){ delete [] p; })
    , m_data(m_shared_data.get())
  {
    // Build the strides
    if (N > 0) {
      auto prev = m_strides[N - 1] = 1;
      for (int i = N - 2; i >= 0; i--)
        prev = m_strides[i] = m_dims[i + 1] * prev;
    }
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
  template <typename ...Indices>
  ArrayIndex index(const Path& path)
  {
    return std::inner_product(path.cbegin(), path.cend(),
                              m_strides.cbegin(), 0);
  }

  /**
   * Convert a multi-dimensional position into an offset from
   * the beginning of the data (m_data).
   */
  template <typename ...Indices>
  ArrayIndex index(Indices... indices)
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
   * Apply a function to all the elements in the array.
   */
  void map(const std::function<T(const Path&, const T&)>& f)
  {
    std::array<ArrayIndex,N> path;
    for_looper<T,N,0>(*this, path, f);
  }

  /**
   * Fill an entire array with a single value.
   */
  void fill(const T& val)
  {
    map([&val](auto path, auto& old) {
        (void) path; (void) old;
        return val;
      });
  }

  /**
   * Construct a copy of an array.
   * All the elements are put in a contiguous region of memory
   * newly allocated with, initially, no other view on it.
   * TODO Implement this for const arrays.
   */
  Array<T,N> copy()
  {
    Array<T,N> a(m_dims);
    map([&a](auto path, auto& val) {
        return a(path) = val;
      });
    return a;
  }

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
