// necomi/core/iterators.h – STL compatible iterators
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <cstddef>
#include <iterator>
#include "strides.h"

namespace necomi
{

template <typename Array>
class ArrayIterator
  : public std::iterator<std::random_access_iterator_tag,
			 typename Array::dtype>
{
public:
  using dim_type = typename Array::dim_type;
  using dims_type = typename Array::dims_type;
  using dtype = typename Array::dtype;
  enum { ndim = Array::ndim };
  
  explicit ArrayIterator(Array& array)
    : m_array(array)
  {
    m_coords.fill(0);
  }
  ArrayIterator(Array& array, dims_type coords)
    : m_array(array)
    , m_coords(coords)
  {
    // TODO: check that coords are in range
  }

  ArrayIterator<Array>& operator=(const ArrayIterator<Array>& other)
  {
    m_array = other.m_array;
    m_coords = other.m_coords;
    return *this;
  }

  dims_type coords() const
  {
    return m_coords;
  }

  dtype& operator*()
  {
    return m_array(m_coords);
  }

  ArrayIterator<Array>& operator++()
  {
    for (long i = ndim-1; i >= 0; i--) {
      if (i == 0 || m_coords[i] < m_array.dim(i) - 1) {
        m_coords[i]++;
        break;
      }
      else {
        m_coords[i] = 0;
      }
    }
    return *this;
  }

  ArrayIterator<Array>& operator--()
  {
    for (long i = ndim-1; i >= 0; i--) {
      if (m_coords[i] > 0) {
        m_coords[i]--;
        break;
      }
      else {
        m_coords[i] = m_array.dim(i) - 1;
      }
    }
    return *this;
  }

  bool operator!=(const ArrayIterator<Array>& other) const
  {
    return &m_array != &other.m_array
      || m_coords != other.m_coords;
  }

  bool operator==(const ArrayIterator<Array>& other) const
  {
    return ! this->operator!=(other);
  }

  bool operator<(const ArrayIterator<Array>& other) const
  {
    // Check arrays
    return m_coords < other.m_coords;
  }

  std::ptrdiff_t operator-(const ArrayIterator<Array>& other) const
  {
    // TODO: throw exception when arrays mismatch
    auto dims = m_array.dims();
    return static_cast<std::ptrdiff_t>(strided_index(dims, m_coords))
      - static_cast<std::ptrdiff_t>(strided_index(dims, other.m_coords));
  }

  ArrayIterator<Array> operator+(std::ptrdiff_t offset) const
  {
    ArrayIterator<Array> other(m_array);
    auto dims = m_array.dims();
    auto strides = default_strides(dims);
    auto current_idx = static_cast<std::ptrdiff_t>(strided_index(dims, m_coords));
    // TODO: check for negative values
    other.m_coords = strided_index_to_coords(current_idx + offset, strides);
    return other;
  }
  
  ArrayIterator<Array> operator-(std::ptrdiff_t offset) const
  {
    return this->operator+(-offset);
  }
protected:
  Array& m_array;
  dims_type m_coords;
};

} // namespace necomi

// Local Variables:
// mode: c++
// End:
