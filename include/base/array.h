/*
 * cuiloa - A scientific multi-dimensional array library.
 * Copyright (C) 2007  Émilien Tlapale
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __CUILOA_ARRAY_H
#define __CUILOA_ARRAY_H


#include <string.h>

#include <exception>
#include <iostream>
#include <string>


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
  template <typename T>
    class Array
    {
    public:
      Array();

      /**
       * Create a new multi-dimensional array.
       * If data is passed as argument then it will be destroyed by
       * delete[] by the constructor. The data should also be contiguous.
       */
      Array(int ndim, const int* dims, T* data = NULL);

      /**
       * Create another view on the given array.
       */
      Array(const Array<T>& src);
      
      ~Array();

      /** Construct a copy of an array. */
      Array<T> copy() const;

      /** Make the array a view on another one. */
      Array<T>& operator=(const Array<T>& src);
      
      /** Return the number of dimensions in the array. */
      int dimensions_count() const;

      /** Return all the dimensions of the array. */
      inline const int* dimensions() const;
      
      const int* strides() const;
      
      /**
       * Return the total number of elements in the array.
       * This is equivalement to
       * dimensions()[0] * ... * dimensions()[dimensions_count() - 1]
       */
      int size() const;

      bool is_empty() const;
      
      /**
       * Return a shared view of the array.
       * The view will be of the given size starting at the desired
       * position.
       */
      Array<T> view(int* dims, int* offset) const;

      /**
       * Return a dimensionality reduced view of the array.
       */
      Array<T> operator[](int index) const;
      
      /**
       * Return a reference to a single element in the array.
       */
      T& operator()(int* path);
      
      T operator()(int* path) const;

      /**
       * Return a reference to a single element in the array.
       */
      T& operator()(int index_0, ...);

      T operator()(int index_0, ...) const;
      
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
      int index(int* pos) const;

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

      int m_ndim;
      int* m_dims;
      int* m_strides;
      bool m_contiguous;
      InnerData *m_inner;
      T* m_data;
    };
  
  /** Duplicate the given array using new. */
  template <typename T>
    T* memdup(const T* src, int count)
    {
      T* dst = new T[count];
      memcpy(dst, src, count * sizeof(T));
      return dst;
    }

  template <typename T>
    Array<T>::Array()
      : m_contiguous(true)
    {
      m_ndim = 0;
      m_dims = NULL;
      m_inner = NULL;
      m_strides = NULL;
      m_data = NULL;
    }
  
  template <typename T>
    Array<T>::Array(int ndim, const int* dims, T* data)
      : m_contiguous(true)
    {
      m_ndim = ndim;
      m_dims = memdup<int>(dims, ndim);
      m_inner = new InnerData();
      m_inner->m_refcount = 1;
      m_data = m_inner->m_data = data == NULL ? new T[this->size()] : data;

      // Build the strides
      m_strides = new int[ndim];
      int prev = m_strides[m_ndim - 1] = 1;
      for (int i = m_ndim - 2; i >= 0; i--)
	prev = m_strides[i] = m_dims[i + 1] * prev;
    }
  
  template <typename T>
  Array<T>::Array(const Array<T>& src)
    : m_dims(NULL), m_strides(NULL), m_inner(NULL)
    {
      this->operator=(src);
    }
  
  template <typename T>
    Array<T>::~Array()
    {
      if (m_inner && --m_inner->m_refcount == 0)
	delete m_inner;
      if (m_strides)
	delete [] m_strides;
      if (m_dims)
	delete [] m_dims;
    }

  template <typename T>
    Array<T>
    Array<T>::copy() const
    {
      Array<T> a(m_ndim, m_dims);
      for (int i = 0; i < this->size(); i++)
	a.m_data[i] = m_data[i];
      return a;
    }

  template <typename T>
    Array<T>&
    Array<T>::operator=(const Array<T>& src)
    {
      this->~Array();
      m_ndim = src.m_ndim;
      m_dims = memdup<int>(src.m_dims, m_ndim);
      m_strides = memdup<int>(src.m_strides, m_ndim);
      m_contiguous = src.m_contiguous;
      m_inner = src.m_inner;
      m_inner->m_refcount++;
      m_data = src.m_data;
      return *this;
    }

  template <typename T>
    int
    Array<T>::dimensions_count() const
    {
      return m_ndim;
    }

  template <typename T>
    inline const int*
    Array<T>::dimensions() const
    {
      return m_dims;
    }

  template <typename T>
    const int*
    Array<T>::strides() const
    {
      return m_strides;
    }

  template <typename T>
    int
    Array<T>::size() const
    {
      int ans = 1;
      for (int i = 0; i < m_ndim; i++)
	ans *= m_dims[i];
      return ans;
    }

  template <typename T>
    Array<T>
    Array<T>::view(int* dims, int* offset) const
    {
      Array<T> a(*this);

      memcpy (a.m_dims, dims, m_ndim * sizeof(int));
      a.m_data = &(m_data[this->index(offset)]);

      return a;
    }
  
  template <typename T>
    Array<T>
    Array<T>::operator[](int index) const
    {
      Array<T> a(*this);
      
      a.m_data = &(m_data[index * m_strides[0]]);
      a.m_ndim--;
      memmove(a.m_dims, a.m_dims + 1, a.m_ndim * sizeof(int));
      memmove(a.m_strides, a.m_strides + 1, a.m_ndim * sizeof(int));
      
      return a;
    }

  template <typename T>
    Array<T>::operator T&()
    {
#ifdef CUILOA_DEBUG
      if (this->size() != 1)
	throw ArrayIndexException("Cannot index-cast a non-singleton");
#endif /* CUILOA_DEBUG */
      
      return m_data[0];
    }

  template <typename T>
    T
    Array<T>::operator=(const T & src)
    {
      return ((T &) (*this)) = src;
    }
  
  template <typename T>
    T&
    Array<T>::operator()(int* path)
    {
      return m_data[this->index(path)];
    }
  
  template <typename T>
    T
    Array<T>::operator()(int* path) const
    {
      return m_data[this->index(path)];
    }
  
  template <typename T>
    T&
    Array<T>::operator()(int index_0, ...)
    {
      va_list ap;
      int index = index_0 * m_strides[0];
#ifdef CUILOA_DEBUG
      if (index_0 >= m_dims[0])
	throw ArrayIndexException("Invalid index");
#endif /* CUILOA_DEBUG */

      va_start(ap, index_0);
      
      for (int i = 1; i < m_ndim; i++)
#ifdef CUILOA_DEBUG
	{
	  int index_i = va_arg(ap, int);
	  if (index_i >= m_dims[i])
	    throw ArrayIndexException("Invalid index");
	  index += index_i * m_strides[i];
	}
#else
      index += va_arg(ap, int) * m_strides[i];
#endif /* CUILOA_DEBUG */
      
      va_end (ap);
      return m_data[index];
    }
  
  template <typename T>
    T
    Array<T>::operator()(int index_0, ...) const
    {
      va_list ap;
      int index = index_0 * m_strides[0];
#ifdef CUILOA_DEBUG
      if (index_0 >= m_dims[0])
	throw ArrayIndexException("Invalid index");
#endif /* CUILOA_DEBUG */

      va_start(ap, index_0);
      
      for (int i = 1; i < m_ndim; i++)
#ifdef CUILOA_DEBUG
	{
	  int index_i = va_arg(ap, int);
	  if (index_i >= m_dims[i])
	    throw ArrayIndexException("Invalid index");
	  index += index_i * m_strides[i];
	}
#else
      index += va_arg(ap, int) * m_strides[i];
#endif /* CUILOA_DEBUG */
      
      va_end (ap);
      return m_data[index];
    }
  
  template <typename T>
    int
    Array<T>::index(int* pos) const
    {
      int ans = 0;
      for (int i = 0; i < m_ndim; i++)
	ans += pos[i] * m_strides[i];
      return ans;
    }
  
  template <typename T>
    T*
    Array<T>::data() const
    {
      return m_data;
    }
  
  template <typename T>
    bool
    Array<T>::is_empty() const
    {
      return this->size() == 0;
    }
  
  template <typename T>
    void
    Array<T>::fill(T val)
    {
      int sz = this->size();
      for (int i = 0; i < sz; i++)
	m_data[i] = val;
    }

#define cuiloa_for(array, var, dim) \
  for (int var = 0; var < (array).dimensions()[dim]; var++)

#if 0
  /*#define cuiloa_for(a, i)					\
  for (int (i) = 0; (i) < (a).dimensions()[(i)]; (i)++)

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

#endif



/*
 * Local Variables:
 * mode: c++
 * coding: utf-8
 * End:
 */
