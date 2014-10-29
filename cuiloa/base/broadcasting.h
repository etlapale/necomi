/*
 * Copyright © 2014 University of California, Irvine
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

#include "delayed.h"

namespace cuiloa {

  /**
   * Prepend extra dimensions to an array.
   */
  template <ArrayIndex M, typename Concrete, typename T, ArrayIndex N,
	    typename std::enable_if<(M>N)>::type* = nullptr>
  auto widen(const Dimensions<M>& dims,
	     const AbstractArray<Concrete,T,N>& a)
	     
  {
#ifndef CUILOA_NO_BOUND_CHECKS
    // Make sure the dimensions are matching
    for (ArrayIndex i = 0; i < N; i++)
      if (a.dimensions()[i] != dims[i+M-N])
	throw std::length_error("cannot broadcast arrays to different dimensions");
#endif
    return make_delayed<T,M>(dims,
			     [a=a.shallow_copy()](auto& path)
			     { Coordinates<N> old_path;
			       std::copy(path.cbegin()+(M-N), path.cend(),
					 old_path.begin());
			       return a(old_path); });
  }

  /**
   * Append extra dimensions to an array.
   */
  template <ArrayIndex M, typename Concrete, typename T, ArrayIndex N,
	    typename std::enable_if<(M>N)>::type* = nullptr>
  auto widen_right(const Dimensions<M>& dims,
		   const AbstractArray<Concrete,T,N>& a)
	     
  {
#ifndef CUILOA_NO_BOUND_CHECKS
    // Make sure the dimensions are matching
    for (ArrayIndex i = 0; i < N; i++)
      if (a.dimensions()[i] != dims[i])
	throw std::length_error("cannot right-broadcast arrays to different dimensions");
#endif
    return make_delayed<T,M>(dims,
			     [a=a.shallow_copy()](const auto& coords)
			     { Coordinates<N> coords_a;
			       std::copy_n(coords.cbegin(), N, coords_a.begin());
			       return a(coords_a); });
  }

  namespace delayed {
    namespace broadcasting {

      template <typename Concrete1, typename T, ArrayIndex N,
		typename Concrete2, ArrayIndex M,
		typename std::enable_if<(M>N)>::type* = nullptr
		>
      auto operator*(const AbstractArray<Concrete1,T,N>& a,
		     const AbstractArray<Concrete2,T,M>& b)
      {
	return cuiloa::delayed::operator*(widen<M>(b.dimensions(), a), b);
      }

      template <typename Concrete1, typename T, ArrayIndex N,
		typename Concrete2, ArrayIndex M,
		typename std::enable_if<(N>M)>::type* = nullptr
		>
      auto operator*(const AbstractArray<Concrete1,T,N>& a,
		     const AbstractArray<Concrete2,T,M>& b)
      {
	return cuiloa::delayed::operator*(a, widen<N>(a.dimensions(), b));
      }

      template <typename Concrete1, typename T, ArrayIndex N,
		typename Concrete2, ArrayIndex M,
		typename std::enable_if<(M>N)>::type* = nullptr
		>
      auto operator/(const AbstractArray<Concrete1,T,N>& a,
		     const AbstractArray<Concrete2,T,M>& b)
      {
	return cuiloa::delayed::operator/(widen<M>(b.dimensions(), a), b);
      }

      template <typename Concrete1, typename T, ArrayIndex N,
		typename Concrete2, ArrayIndex M,
		typename std::enable_if<(N>M)>::type* = nullptr
		>
      auto operator/(const AbstractArray<Concrete1,T,N>& a,
		     const AbstractArray<Concrete2,T,M>& b)
      {
	return cuiloa::delayed::operator/(a, widen<N>(a.dimensions(), b));
      }

      template <typename Concrete1, typename T, ArrayIndex N,
		typename Concrete2, ArrayIndex M,
		typename std::enable_if<(M>N)>::type* = nullptr
		>
      auto operator+(const AbstractArray<Concrete1,T,N>& a,
		     const AbstractArray<Concrete2,T,M>& b)
      {
	return cuiloa::delayed::operator+(widen<M>(b.dimensions(), a), b);
      }

      template <typename Concrete1, typename T, ArrayIndex N,
		typename Concrete2, ArrayIndex M,
		typename std::enable_if<(N>M)>::type* = nullptr
		>
      auto operator+(const AbstractArray<Concrete1,T,N>& a,
		     const AbstractArray<Concrete2,T,M>& b)
      {
	return cuiloa::delayed::operator+(a, widen<N>(a.dimensions(), b));
      }

      template <typename Concrete1, typename T, ArrayIndex N,
		typename Concrete2, ArrayIndex M,
		typename std::enable_if<(M>N)>::type* = nullptr
		>
      auto operator-(const AbstractArray<Concrete1,T,N>& a,
		     const AbstractArray<Concrete2,T,M>& b)
      {
	return cuiloa::delayed::operator-(widen<M>(b.dimensions(), a), b);
      }

      template <typename Concrete1, typename T, ArrayIndex N,
		typename Concrete2, ArrayIndex M,
		typename std::enable_if<(N>M)>::type* = nullptr
		>
      auto operator-(const AbstractArray<Concrete1,T,N>& a,
		     const AbstractArray<Concrete2,T,M>& b)
      {
	return cuiloa::delayed::operator-(a, widen<N>(a.dimensions(), b));
      }
    } // namespace broadcasting
  } // namespace delayed
} // namespace cuiloa

// Local Variables:
// mode: c++
// End:
