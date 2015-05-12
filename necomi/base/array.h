// necomi/base/array.h – Immediate arrays
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

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


namespace necomi
{


/**
 * Cumulative sum.
 */
template <typename Indexable, typename T=typename Indexable::dtype>
Array<T,Indexable::ndim> cumsum(const Indexable& a, ArrayIndex dim = 0)
{
  Array<T,Indexable::ndim> res(a.dims());
  res.map([&res,dim,&a](auto& path, auto& valx) {
      if (path[dim] == 0) {
	valx = a(path);
      }
      else {
	auto prev = path;
	prev[dim]--;
	valx = res(prev) + a(path);
      }
    });
  return res;
}

template <typename Indexable, typename T=typename Indexable::dtype>
Array<T,Indexable::ndim>& operator+=(Array<T,Indexable::ndim>& a,
				     const Indexable& b)
{
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions of a and b are the same
  if (a.dims() != b.dims())
    throw std::length_error("cannot increment with array of different dimensions");
#endif
  a.map([&b](auto& path, auto& val) {val += b(path);});
  return a;
}

/**
 * Convert an abstract array to an immediate one with element casting.
 *
 * A new array with copied or casted elements is returned,
 * even if the original array already was an immediate with same
 * element type.
 */
template <typename U, typename From, typename T=typename From::dtype,
	  typename std::enable_if_t<std::is_convertible<T,U>::value>* = nullptr>
Array<U, From::ndim> immediate(const From& a)
{
  Array<U, From::ndim> res(a.dims());
  res.map([&a](auto& coords, auto& val) {
      val = static_cast<U>(a(coords));
    });
  return res;
}

/**
 * Convert an abstract array to an immediate one with same element type.
 *
 * A new array with copied or casted elements is always returned.
 */
template <typename From, typename T=typename From::dtype>
Array<T, From::ndim> immediate(const From& a)
{
  return immediate<T,From>(a);
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

template <typename T, ArrayDimension N,
	  typename Indexable, typename U=typename Indexable::dtype,
	  std::enable_if_t<is_promotable<U,T>::value
			   && N==Indexable::ndim>* = nullptr>
Array<T,N>& operator*=(Array<T,N>& numerator, const Indexable& denominator)
{
  numerator.map([&denominator](auto& coords, auto& val) {
      val *= denominator(coords);
    });
  return numerator;
}

template <typename T, ArrayDimension N,
	  typename Indexable, typename U=typename Indexable::dtype,
	  std::enable_if_t<is_promotable<U,T>::value
			   && N==Indexable::ndim>* = nullptr>
Array<T,N>& operator/=(Array<T,N>& numerator, const Indexable& denominator)
{
  numerator.map([&denominator](auto& coords, auto& val) {
      val /= denominator(coords);
    });
  return numerator;
}
  
namespace broadcasting
{
template <typename T, ArrayDimension N,
	  typename Indexable, typename U=typename Indexable::dtype,
	  std::enable_if_t<(N>Indexable::ndim) && is_promotable<U,T>::value>* = nullptr>
Array<T,N>& operator/=(Array<T,N>& numerator, const Indexable& denominator)
{
  return necomi::operator/=(numerator,
			    widen(numerator.dims(), denominator));
}

} // namespace broadcasting
} // namespace necomi

// Local Variables:
// mode: c++
// End:
