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

#include <cmath>
#include <iostream>
#include <stdexcept>

#include "../base/array.h"

/**
 * \file deriche.h Canny-Deriche recursive filtering
 * \ingroup filters
 */

namespace cuiloa
{

/**
 * Filtering type used by the Canny-Deriche filter.
 *
 * \ingroup filters
 */
  enum class DericheOrder
  {
    BLUR,
    FIRST_DERIVATIVE,
    SECOND_DERIVATIVE,
  };
  
#ifndef IN_DOXYGEN
template <typename T, ArrayIndex N, ArrayIndex K>
std::enable_if_t<K<N && 0<N && std::is_floating_point<T>::value,void>
inner_loop(std::array<ArrayIndex,N>& path,
           ArrayIndex dim,
           bool cond,
           Array<T,N>& a, T* y,
           T a0, T a1, T a2, T a3,
           T b1, T b2)
{
  auto& dims = a.dimensions();

  if (K == dim) {
    path[K] = 0;
    inner_loop<T,N,K+1>(path, dim, cond,
                        a, y, a0, a1, a2, a3, b1, b2);
  }
  else {
    for (ArrayIndex i = 0; i < dims[K]; i++) {
      path[K] = i;
      inner_loop<T,N,K+1>(path, dim, cond,
                          a, y, a0, a1, a2, a3, b1, b2);
    }
  }
}

template <typename T, ArrayIndex N, ArrayIndex K>
std::enable_if_t<K==N && 0<N && std::is_floating_point<T>::value,void>
inner_loop(std::array<ArrayIndex,N>& path,
           ArrayIndex dim,
           bool cond,
           Array<T,N>& a, T* y,
           T a0, T a1, T a2, T a3,
           T b1, T b2)
{
  auto& dims = a.dimensions();
  auto ddim = dims[dim];
  auto stride = a.strides()[dim];
  
  // Compute the index corresponding to the path
  T* ima = &a(path);

  // First pass
  double xp = 0, yp = 0, yb = 0;
  if (cond) {
    xp = *ima;
    double coefp = (a0+a1)/(1+b1+b2);
    yb = yp = coefp * xp;
  }
  for (ArrayIndex i = 0; i < ddim; i++) {
    T xc = *ima;
    ima += stride;
    T yc = *(y++) = a0*xc + a1*xp - b1*yp - b2*yb;
    xp = xc; yb = yp; yp = yc;
  }

  // Second pass
  T xn = 0, xa = 0, yn = 0, ya = 0;
  if (cond) {
    xn = xa = *(ima-stride);
    T coefn = (a2+a3)/(1+b1+b2);
    yn = ya = coefn * xn;
  }
  for (int i = ddim-1; i >= 0; i--) {
    T xc = *(ima-=stride);
    T yc = a2*xn + a3*xa - b1*yn - b2*ya;
    xa = xn; xn = xc; ya = yn; yn = yc;
    *ima = *(--y)+yc;
  }
}

#endif // IN_DOXYGEN

#ifdef IN_DOXYGEN
/**
 * Filter an array using Canny-Deriche along a single dimension.
 * This function is recursive, hence always takes a linear time depending
 * on the size of the dimensions to be filtered, irrespective of the
 * given parameter `sigma`.
 *
 * \param a     Array to be filtered in-place. Its type `T` must be a
 *              floating point number and its dimensionality `>0`.
 * \param dim	Dimension along which to filter, must be less than `N`.
 * \param sigma	Deviation of the approximated Gaussian.
 * \param cond  Whether borders values are taken into account or ignored.
 * \ingroup filters
 */
template <typename T, ArrayIndex N>
Array<T,N>&
deriche(Array<T,N>& a, ArrayIndex dim, double sigma,
        DericheOrder order=DericheOrder::BLUR,
        bool cond=true);
#else
template <typename T, ArrayIndex N>
std::enable_if_t<0<N && std::is_floating_point<T>::value,Array<T,N>&>
deriche(Array<T,N>& a, ArrayIndex dim, T sigma,
        DericheOrder order=DericheOrder::BLUR,
        bool cond=true)
{
  // σ=0 is a nop
  if (sigma == 0)
    return a;
  // σ must be >0
  sigma = std::abs(sigma);

  T alpha = 1.695 / sigma;
  T ena = std::exp(-alpha);
  T ens = ena * ena;
  T b1 = -2 * ena;
  T b2 = ens;
  
  auto& dims = a.dimensions();

  T a0, a1, a2, a3;

  T k;
  switch (order) {
  case DericheOrder::BLUR:
    k = (1-ena) * (1-ena) / (1 + 2*alpha*ena - ens);
    a0 =  k;
    a1 =  k * ena * (alpha - 1);
    a2 =  k * ena * (alpha + 1);
    a3 = -k * ens;
    break;
  case DericheOrder::FIRST_DERIVATIVE:
    k = -(1-ena) * (1-ena) * (1-ena) / (2*(ena+1)*ena);
    a0 = a3 = 0;
    a1 = k*ena;
    a2 = -a1;
    break;
  case DericheOrder::SECOND_DERIVATIVE: {
    const T ea = std::exp(-alpha);
    const T k = -(ens-1)/(2*alpha*ena);
    const T kn = -2*(-1 + 3*ea - 3*ea*ea + ea*ea*ea)
        / (3*ea + 1 + 3*ea*ea + ea*ea*ea);

    a0 = kn;
    a1 = -kn*(1+k*alpha)*ena;
    a2 = kn*(1-k*alpha)*ena;
    a3 = -kn*ens;
    }
    break;
  }

  std::array<ArrayIndex,N> path;
  T y[dims[dim]];

  inner_loop<T,N,0>(path, dim, cond, a, y, a0, a1, a2, a3, b1, b2);

  return a;
}
#endif // IN_DOXYGEN
  
#ifdef IN_DOXYGEN
/**
 * Filter an array using Canny-Deriche along all its dimensions.
 * \ingroup filters
 */
template <typename T, ArrayIndex N>
Array<T,N>&
deriche(Array<T,N>& a, T sigma, DericheOrder order=DericheOrder::BLUR);
#else
template <typename T, ArrayIndex N>
std::enable_if_t<0<N && std::is_floating_point<T>::value,Array<T,N>&>
deriche(Array<T,N>& a, double sigma, DericheOrder order=DericheOrder::BLUR)
{
for (ArrayIndex i = 0; i < N; i++)
deriche<T,N>(a, i, sigma,order);
return a;
}
#endif // IN_DOXYGEN

} // namespace cuiloa
