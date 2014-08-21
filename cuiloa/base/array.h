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
  
/** Duplicate the given array using new. */
template <typename T>
T* memdup(const T* src, unsigned int count)
{
  T* dst = new T[count];
  memcpy(dst, src, count * sizeof(T));
  return dst;
}

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

/**
 * Multi-dimensional arrays allowing shared data and non-contiguity.
 * 
 * Arrays created via the copy constructor, the operator[] or any
 * other operation on a \c const Array will share the same data as
 * their source. To get a real copy of an array, use the
 * Array::copy() method.
 * The \c const qualifier is only there to say the array view cannot be
 * changed (unmodifiable view dimensions, target data, ...). Unmodifiable
 * data should be marked as such in the template parameter.
 */
template <typename T, unsigned int N>
class Array
{
public:
  /**
   * Create a new multi-dimensional array with uninitialized elements.
   */
  template <typename ...Dim>
  Array(Dim ...dims)
    : m_dims{{static_cast<ArrayIndex>(dims)...}}
    , m_data(new T[size()], [](T* p){ delete [] p; })
    , m_contiguous(true)
  {
    // Duplicate of m_dims to get a better message
    static_assert(sizeof...(Dim) == N, "improper dimensions arity");
    static_assert(all_indices<Dim...>(), "invalid dimensions type");

    // Build the strides
    if (N > 0) {
      auto prev = m_strides[N - 1] = 1;
      for (int i = N - 2; i >= 0; i--)
        prev = m_strides[i] = m_dims[i + 1] * prev;
    }
  }

#if 0
  /**
   * Create another view on the given array.
   */
  Array(const Array<T,N>& src);
  
  /** Construct a copy of an array. */
  Array<T,N> copy() const;

  /** Make the array a view on another one. */
  //Array<T,N>& operator=(const Array<T,N>& src);

#endif
  const std::array<ArrayIndex,N>& dimensions() const
  { return m_dims; }

  const std::array<ArrayIndex,N>& strides() const
  { return m_strides; }
  
  /**
   * Return the total number of elements in the array.
   * This is equivalement to
   * dimensions()[0] * ... * dimensions()[dimensions_count() - 1]
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

  /**
   * Return a dimensionality reduced view of the array.
   */
  Array<T,N> operator[](unsigned int index) const;

  /**
   * Convert a singleton to its value.
   */
  operator T&();
  
  /**
   * Change the value at the given position.
   */
  T operator=(const T & src);

#endif
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
    return std::inner_product(idx.cbegin(), idx.cend(),
                              m_strides.cbegin(), 0);
  }

  /**
   * Return a reference to a single element.
   */
  template <typename ...Indices>
  T& operator()(Indices... indices)
  {
    return m_data.get()[index(indices...)];
  }

  /**
   * Return the value of a single element.
   */
  template <typename ...Indices>
  T operator()(Indices... indices) const
  {
    return m_data.get()[index(indices...)];
  }

  /**
   * Return the data associated with the array.
   */
  T* data()
  { return m_data.get(); }

  /**
   * Return the immutable data associated with the array.
   */
  const T* data() const
  { return m_data.get(); }

#if 0
  /**
   * Fill an entire array with a single value.
   */
  void fill(T val);
#endif

protected:
  std::array<ArrayIndex,N> m_dims;
  std::array<ArrayIndex,N> m_strides;
  /**
   * Pointer to the actual data shared between the views.
   */
  std::shared_ptr<T> m_data;
  bool m_contiguous;
};


#if 0
template <typename T, unsigned int n>
Array<T,n>::Array(const Array<T,n>& src)
  : m_dims(NULL), m_strides(NULL), m_inner(NULL)
  {
    //this->operator=(src);
    // TODO something!
  }

template <typename T, unsigned int n>
  Array<T,n>
  Array<T,n>::copy() const
  {
    Array<T,n> a(m_dims);
    for (auto i = 0; i < this->size(); i++)
      a.m_data[i] = m_data[i];
    return a;
  }

/*
template <typename T, unsigned int n>
  Array<T,n>&
  Array<T,n>::operator=(const Array<T,n>& src)
  {
    this->~Array();
    m_dims = memdup<unsigned int>(src.m_dims, n);
    m_strides = memdup<unsigned int>(src.m_strides, n);
    m_contiguous = src.m_contiguous;
    m_inner = src.m_inner;
    m_inner->m_refcount++;
    m_data = src.m_data;
    return *this;
  }*/

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

template <typename T, unsigned int n>
  Array<T,n>
  Array<T,n>::operator[](unsigned int index) const
  {
    Array<T,n> a(*this);
    
    a.m_data = &(m_data[index * m_strides[0]]);
    a.m_ndim--;
    memmove(a.m_dims, a.m_dims + 1, n * sizeof(unsigned int));
    memmove(a.m_strides, a.m_strides + 1, a * sizeof(unsigned int));
    
    return a;
  }

template <typename T, unsigned int n>
  T
  Array<T,n>::operator=(const T & src)
  {
    return ((T &) (*this)) = src;
  }

template <typename T, unsigned int n>
  void
  Array<T,n>::fill(T val)
  {
    auto sz = this->size();
    for (auto i = 0; i < sz; i++)
      m_data[i] = val;
  }

#define cuiloa_for(array, var, dim) \
  for (unsigned int var = 0; var < (array).dimensions()[dim]; var++)

#if 0
  /*#define cuiloa_for(a, i)					\
  for (unsigned int (i) = 0; (i) < (a).dimensions()[(i)]; (i)++)

#define cuiloa_for2(a, i, j)			\
  cuiloa_for(a, i) cuiloa_for(a, j)

#define cuiloa_for3(a, i, j, k) \
  cuiloa_for2(a, i, j) cuiloa_for(a, k)

#define cuiloa_for4(a, i, j, k, l)  \
  cuiloa_for2(a, i, j) cuiloa_for2(a, k, l)
  */
#endif


#define cuiloa_for_path(a, path, i)					\
  for ((path)[(i)] = 0; (path)[(i)] < (a).dimensions()[(i)]; (path)[(i)]++)

#define cuiloa_for_path2(a, path, i, j)				\
  cuiloa_for_path(a, path, i) cuiloa_for_path(a, path, j)

#define cuiloa_for_path3(a, path, i, j, k)			\
  cuiloa_for_path2(a, path, i, j) cuiloa_for_path(a, path, k)

#define cuiloa_for_path4(a, path, i, j, k, l)				\
  cuiloa_for_path2(a, path, i, j) cuiloa_for_path2(a, path, k, l)
#endif
}
