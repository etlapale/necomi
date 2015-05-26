// necomi/arrays/vararray.h – Dynamically-sized strided arrays
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <vector>

#include "../core/shape.h"
#include "../traits/arrays.h"

namespace necomi
{

template <typename T>
class VarArray
{
public:
  using dim_type = std::size_t;
  using dims_type = std::vector<dim_type>;
  using dtype = T;

  template <typename ...Dims,
	    std::enable_if_t<all_convertible<Dims..., dim_type>::value>* = nullptr>
  VarArray(Dims... dims)
    : VarArray(dims_type{static_cast<dim_type>(dims)...})
  {
  }

  VarArray(const dims_type& dims)
    : m_dims(dims)
    , m_strides(default_strides(dims))
    , m_shared_data(new T[size(*this)], [](T* p){ delete [] p; })
    , m_data(m_shared_data.get())
  {
  }

  VarArray(const VarArray<T>& a)
    : m_strides(a.m_strides)
    , m_shared_data(a.m_shared_data)
    , m_data(a.m_data)
  {
  }

  template <typename Array,
	    std::enable_if_t<is_indexable<Array>::value
			     && is_promotable<typename Array::dtype,T>::value>* = nullptr>
  VarArray(const Array& a)
    : VarArray(to_vector<dim_type>(a.dims()))
  {
    for_each(*this, [&a](const auto& coords, auto& value){
	value = a(to_array<typename Array::dim_type, Array::ndim>(coords));
      });
  }

  dim_type ndim() const
  { return m_dims.size(); }

  const dims_type& dims() const
  { return m_dims; }

  dim_type dim(dim_type i) const
  { return m_dims[i]; }

  T& operator()(const dims_type& coords)
  { return m_data[index(*this, coords)]; }
  
  const T& operator()(const dims_type& coords) const
  { return m_data[index(*this, coords)]; }

  template <typename ...Coords,
	    std::enable_if_t<all_convertible<Coords...,dim_type>::value>* = nullptr>
  T& operator()(Coords... coords)
  {
#ifndef NECOMI_NO_BOUND_CHECKS
    if (sizeof...(Coords) != m_dims.size())
      throw std::length_error(std::string("invalid number of coordinates (expecting ") + std::to_string(m_dims.size()) + ", got " + std::to_string(sizeof...(Coords)) + ")");
#endif // NECOMI_NO_BOUND_CHECKS
    
    // TODO check indices for out of bounds
    return m_data[index(*this, coords...)];
  }

  /// Return a raw pointer to the data associated with the array.
  T* data()
  { return m_data; }
  
  ///  Return the immutable data associated with the array.
  const T* data() const
  { return m_data; }
  
  /// Return a new shared pointer to the data.
  std::shared_ptr<T> shared_data() const
  {
    return m_shared_data;
  }

  const dims_type& strides() const
  { return m_strides; }

protected:
  dims_type m_dims;
  dims_type m_strides;
  std::shared_ptr<T> m_shared_data;
  T* m_data;
}; // class VarArray

} // namespace necomi

// Local Variables:
// mode: c++
// End:
