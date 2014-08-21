#pragma once

#include <string.h>

#include <exception>
#include <iostream>
#include <string>

#include <stdarg.h>

namespace cuiloa
{
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
   * Multi-dimensional array class.
   * 
   * Arrays created via the copy constructor, the operator[] or any
   * other operation on a \c const Array will share the same data as
   * their source. To get a real copy of an array, use the
   * Array::copy() method. The \c const qualifier is only there to say
   * the array view cannot be changed (unmodifiable view dimensions,
   * target data, ...). Unmodifiable data should be marked as such in
   * the template parameter.
   */
  template <typename T, unsigned int n>
    class Array
    {
    public:
      //Array();

      /**
       * Create a new multi-dimensional array.
       * If data is passed as argument then it will be destroyed by
       * delete[] by the constructor. The data should also be contiguous.
       */
      Array(const unsigned int* dims, T* data = NULL);

      /**
       * Create another view on the given array.
       */
      Array(const Array<T,n>& src);
      
      ~Array();

      /** Construct a copy of an array. */
      Array<T,n> copy() const;

      /** Make the array a view on another one. */
      Array<T,n>& operator=(const Array<T,n>& src);
      
      /** Return the number of dimensions in the array. */
      //unsigned int dimensions_count() const;

      /** Return all the dimensions of the array. */
      inline const unsigned int* dimensions() const;
      
      const unsigned int* strides() const;
      
      /**
       * Return the total number of elements in the array.
       * This is equivalement to
       * dimensions()[0] * ... * dimensions()[dimensions_count() - 1]
       */
      unsigned int size() const;

      /**
       * Return a shared view of the array.
       * The view will be of the given size starting at the desired
       * position.
       */
      Array<T,n> view(unsigned int* dims, unsigned int* offset) const;

      /**
       * Return a dimensionality reduced view of the array.
       */
      Array<T,n> operator[](unsigned int index) const;
      
      /**
       * Return a reference to a single element in the array.
       */
      T& operator()(unsigned int index_0, ...);

      T operator()(unsigned int index_0, ...) const;
      
      /**
       * Convert a singleton to its value.
       */
      operator T&();
      
      /**
       * Change the value at the given position.
       */
      T operator=(const T & src);

      /**
       * Convert a multi-dimensional position into an offset from
       * the beginning of the data (m_data).
       */
      unsigned int index(unsigned int* pos) const;

      /**
       * Return the data associated with the array.
       */
      T* data() const;

      /**
       * Fill an entire array with a single value.
       */
      void fill(T val);
            
    protected:
      struct InnerData
      {
	int m_refcount;
	T* m_data;
      };

      unsigned int* m_dims;
      unsigned int* m_strides;
      bool m_contiguous;
      InnerData *m_inner;
      T* m_data;
    };
  
  /** Duplicate the given array using new. */
  template <typename T>
    T* memdup(const T* src, unsigned int count)
    {
      T* dst = new T[count];
      memcpy(dst, src, count * sizeof(T));
      return dst;
    }

  /*
  template <typename T>
    Array<T>::Array()
      : m_contiguous(true)
    {
      m_ndim = 0;
      m_dims = NULL;
      m_inner = NULL;
      m_strides = NULL;
      m_data = NULL;
    }*/
  
  template <typename T, unsigned int n>
    Array<T,n>::Array(const unsigned int* dims, T* data)
      : m_contiguous(true)
    {
      m_dims = memdup<unsigned int>(dims, n);
      m_inner = new InnerData();
      m_inner->m_refcount = 1;
      m_data = m_inner->m_data = data == NULL ? new T[this->size()] : data;

      // Build the strides
      m_strides = new unsigned int[n];
      auto prev = m_strides[n - 1] = 1;
      for (int i = n - 2; i >= 0; i--)
	prev = m_strides[i] = m_dims[i + 1] * prev;
    }
  
  template <typename T, unsigned int n>
  Array<T,n>::Array(const Array<T,n>& src)
    : m_dims(NULL), m_strides(NULL), m_inner(NULL)
    {
      this->operator=(src);
    }
  
  template <typename T, unsigned int n>
    Array<T,n>::~Array()
    {
      if (m_inner && --m_inner->m_refcount == 0)
	delete m_inner;
      if (m_strides)
	delete [] m_strides;
      if (m_dims)
	delete [] m_dims;
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
    }

  template <typename T, unsigned int n>
    inline const unsigned int*
    Array<T,n>::dimensions() const
    {
      return m_dims;
    }

  template <typename T, unsigned int n>
    const unsigned int*
    Array<T,n>::strides() const
    {
      return m_strides;
    }

  template <typename T, unsigned int n>
    unsigned int
    Array<T,n>::size() const
    {
      auto ans = 1;
      for (unsigned int i = 0; i < n; i++)
	ans *= m_dims[i];
      return ans;
    }

  template <typename T, unsigned int n>
    Array<T,n>
    Array<T,n>::view(unsigned int* dims, unsigned int* offset) const
    {
      Array<T,n> a(*this);

      memcpy (a.m_dims, dims, n * sizeof(unsigned int));
      a.m_data = &(m_data[this->index(offset)]);

      return a;
    }
  
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
    Array<T,n>::operator T&()
    {
#ifdef CUILOA_DEBUG
      if (this->size() != 1)
	throw ArrayIndexException("Cannot index-cast a non-singleton");
#endif /* CUILOA_DEBUG */
      
      return m_data[0];
    }

  template <typename T, unsigned int n>
    T
    Array<T,n>::operator=(const T & src)
    {
      return ((T &) (*this)) = src;
    }
  
#if 0
  template <typename T, unsigned int n>
    T&
    Array<T,n>::operator()(unsigned int* path)
    {
      return m_data[this->index(path)];
    }
#endif
  
  template <typename T, unsigned int n>
    T&
    Array<T,n>::operator()(unsigned int index_0, ...)
    {
      va_list ap;
      auto index = index_0 * m_strides[0];
#ifdef CUILOA_DEBUG
      if (index_0 >= m_dims[0])
	throw ArrayIndexException("Invalid index");
#endif /* CUILOA_DEBUG */

      va_start(ap, index_0);
      
      for (unsigned int i = 1; i < n; i++)
#ifdef CUILOA_DEBUG
	{
	  unsigned int index_i = va_arg(ap, unsigned int);
	  if (index_i >= m_dims[i])
	    throw ArrayIndexException("Invalid index");
	  index += index_i * m_strides[i];
	}
#else
      index += va_arg(ap, unsigned int) * m_strides[i];
#endif /* CUILOA_DEBUG */
      
      va_end (ap);
      return m_data[index];
    }
  
  template <typename T, unsigned int n>
    T
    Array<T,n>::operator()(unsigned int index_0, ...) const
    {
      va_list ap;
      auto index = index_0 * m_strides[0];
#ifdef CUILOA_DEBUG
      if (index_0 >= m_dims[0])
	throw ArrayIndexException("Invalid index");
#endif /* CUILOA_DEBUG */

      va_start(ap, index_0);
      
      for (auto i = 1; i < n; i++)
#ifdef CUILOA_DEBUG
	{
	  unsigned int index_i = va_arg(ap, unsigned int);
	  if (index_i >= m_dims[i])
	    throw ArrayIndexException("Invalid index");
	  index += index_i * m_strides[i];
	}
#else
      index += va_arg(ap, unsigned int) * m_strides[i];
#endif /* CUILOA_DEBUG */
      
      va_end (ap);
      return m_data[index];
    }
  
  template <typename T, unsigned int n>
    unsigned int
    Array<T,n>::index(unsigned int* pos) const
    {
      auto ans = 0;
      for (unsigned int i = 0; i < n; i++)
	ans += pos[i] * m_strides[i];
      return ans;
    }
  
  template <typename T, unsigned int n>
    T*
    Array<T,n>::data() const
    {
      return m_data;
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
}
