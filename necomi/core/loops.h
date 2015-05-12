// necomi/core/loops.h – Iterate array elements by coordinates
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

namespace necomi {

template <typename Array, typename Array::dim_type M, typename UnaryOperation>
std::enable_if_t<M<Array::ndim>
for_looper(Array& a, typename Array::dims_type& coords, UnaryOperation f)
{
  for (typename Array::dim_type i = 0; i < a.dim(M); i++) {
    coords[M] = i;
    for_looper<Array,M+1,UnaryOperation>(a, coords, f);
  }
}

template <typename Array, typename Array::dim_type M, typename UnaryOperation>
std::enable_if_t<M==Array::ndim>
for_looper(Array& a, typename Array::dims_type& coords, UnaryOperation f)
{
  f(coords, a(coords));
}


template <typename Array, typename Array::dim_type M, typename Callable>
std::enable_if_t<M<Array::ndim>
const_for_looper(const Array& a, typename Array::dims_type& coords, Callable f)
{
  for (typename Array::dim_type i = 0; i < a.dim(M); i++) {
    coords[M] = i;
    const_for_looper<Array,M+1,Callable>(a, coords, f);
  }
}

template <typename Array, typename Array::dim_type M, typename Callable>
std::enable_if_t<M==Array::ndim>
const_for_looper(const Array& a, typename Array::dims_type& coords, Callable f)
{
  f(coords, a(coords));
}


template <typename Array, typename Array::dim_type M, typename Predicate>
std::enable_if_t<(M<Array::ndim),bool>
breakable_for_looper(const Array& a, typename Array::dims_type& coords, Predicate p)
{
  for (typename Array::dim_type i = 0; i < a.dims()[M]; i++) {
    coords[M] = i;
    bool ret = breakable_for_looper<Array,M+1,Predicate>(a, coords, p);
    if (ret) return true;
  }
  return false;
}

template <typename Array, typename Array::dim_type M, typename Predicate>
std::enable_if_t<M==Array::ndim,bool>
breakable_for_looper(const Array& a, typename Array::dims_type& coords, Predicate p)
{
  return p(a(coords));
}

} // namespace necomi

// Local Variables:
// mode: c++
// End:
