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


namespace cuiloa
{
  /**
   * Filtering type used by the Canny-Deriche filter.
   * \ingroup Filters
   */
  typedef enum {DERICHE_BLUR,
		DERICHE_FIRST_DERIVATIVE,
		DERICHE_SECOND_DERIVATIVE} DericheFilteringType;

  /**
   * Apply a Canny-Deriche recursive filter in-place on an array.
   * \ingroup Filters
   */
  template <typename T, unsigned int n>
    Array<T,n> &
    deriche_filter(Array<T,n> & a, double sigma,
		   DericheFilteringType type,
		   int dim,
		   bool cond = true)
    {
      const int* dims = a.dimensions();

      if (a.is_empty() || sigma == 0.0 || dims[dim] <= 1)
	{
	  std::cerr << "cuiloa::deriche_filter(): Nothing to do" << std::endl;
	  return a;
	}
      if (sigma < 0)
	throw std::runtime_error("cuiloa::deriche_filter(): Invalid sigma");
      
      const double alpha = 1.695 / sigma;
      const double exp_a = exp(alpha);
      const double exp_na = exp(-alpha);
      const double exp_n2a = exp_na * exp_na;
      const double b1 = -2 * exp_na;
      const double b2 = exp_n2a;
      
      double valk = 0, valkn = 0, a0 = 0, a1 = 0, a2 = 0, a3 = 0, g0 = 0;
      double sumg1 = 0, sumg0 = 0;
      
      int parity = 0;

      switch (type)
	{
	case DERICHE_BLUR:
	  valk = (1 - exp_na) * (1 - exp_na) / (1 + 2*alpha*exp_na - exp_n2a);
	  a0 = valk;
	  a1 = valk * exp_na * (alpha - 1);
	  a2 = valk * exp_na * (alpha + 1);
	  a3 = -valk*exp_n2a;
	  parity = 1;
	  if (cond)
	    {
	      sumg1 = valk * (alpha * exp_a + exp_a - 1) / ((exp_a - 1) * (exp_a));
	      g0 = valk;
	      sumg0 = g0 + sumg1;
	    }
          else
	    {
	      g0 = sumg0 = sumg1 = 0;
	    }
	  break;
	case DERICHE_FIRST_DERIVATIVE:
          valk = -(1-exp_na)*(1-exp_na)*(1-exp_na)/(2*(exp_na+1)*exp_na);
          a0 = a3 = 0;
          a1 = valk*exp_na;
          a2 = -valk*exp_na;
          parity =-1;
          if (cond)
	    {
	      sumg1 = (valk*exp_a)/((exp_a-1)*(exp_a-1));
	      g0 = 0;
	      sumg0 = g0+sumg1;
	    }
          else
	    {
	      g0 = sumg0 = sumg1 = 0;
	    }
          break;
	case DERICHE_SECOND_DERIVATIVE:
          valkn = (-2*(-1+3*exp_a-3*exp_a*exp_a+exp_a*exp_a*exp_a)/(3*exp_a+1+3*exp_a*exp_a+exp_a*exp_a*exp_a) );
          valk = -(exp_n2a-1)/(2*alpha*exp_na);
          a0 = valkn;
          a1 = -valkn*(1+valk*alpha)*exp_na;
          a2 = valkn*(1-valk*alpha)*exp_na;
          a3 = -valkn*exp_n2a;
          parity = 1;
          if (cond)
	    {
	      sumg1 = valkn/2;
	      g0 = valkn;
	      sumg0 = g0+sumg1;
	    }
          else
	    {
	      g0 = sumg0 = sumg1 = 0;
	    }
	  break;
	}
      
      double* y = new double[dims[dim]];
      int stride = a.strides()[dim];
      int* path = new int[n];
      deriche_filter_inner_loop(0, path, dims,
				dim, a, y, stride,
				parity, b1, b2,
				a0, a1, a2, a3,
				g0, sumg0, sumg1);

      return a;
    }
  
  /**
   * This function is used by the Deriche filter function
   * deriche_filter(Array<T>&,double,DericheFilteringType,int,bool)
   * and should not be called by normal code.
   */
  template <typename T, unsigned int n>
    void
    deriche_filter_inner_loop(unsigned int level, int* path,
			      const int* dims,
			      unsigned int dim, cuiloa::Array<T,n>& array,
			      double* y, int stride,
			      int parity, double b1, double b2,
			      double a0, double a1, double a2, double a3,
			      double g0, double sumg0, double sumg1)
    {
      if (level < n)
	{
	  if (level == dim)
	    {
	      path[level] = 0;
	      deriche_filter_inner_loop(level + 1, path,
					dims, dim, array, y, stride,
					parity, b1, b2, a0, a1, a2, a3, g0, sumg0, sumg1);
	    }
	  else
	    {
	      for (int i = 0; i < dims[level]; i++)
		{
		  path[level] = i;
		  deriche_filter_inner_loop(level + 1, path,
					    dims,
					    dim, array, y, stride,
					    parity, b1, b2, a0, a1, a2, a3, g0, sumg0, sumg1);
		}
	    }
	  return;
	}
      
      T* ima = array.data() + array.index(path);
      T I1 = *ima;
      T I0 = *(ima += stride);
      double y2 = *(y++) = sumg0 * I1;
      double y1 = *(y++) = g0 * I0 + sumg1 * I1;
      
      ima += stride;
      for (int i=2; i<(int)dims[dim]; ++i)
	{
	  I0 = *ima;
	  ima += stride;
	  double Y0 = *(y++) = a0*I0 + a1*I1 - b1*y1 - b2*y2;
	  I1 = I0;
	  y2 = y1;
	  y1 = Y0;
	}
      I1 = *(ima -= stride);
      y2 = y1 = parity*sumg1*I1;
      *ima = (T)(*(--y)+y2);
      I0 = *(ima -= stride);
      *ima = (T)(*(--y)+y1);
      for (int ii=(int)dims[dim]-3; ii>=0; --ii)
	{
	  double Y0 = a2*I0 + a3*I1 - b1*y1 - b2*y2;
	  I1 = I0;
	  I0 = *(ima -= stride);
	  *ima = (T) (*(--y) + Y0 );
	  y2 = y1; y1 = Y0;
	}
    }

  /**
   * Blur the given array along the specified dimension.
   * The function modifies the given array.
   * Calling this function is equivalent to use
   * deriche_filter(a,sigma,DERICHE_BLUR,dim,cond)
   *
   * \param a   The array to be blurred.
   * \param dim The dimension on which to blur.
   * \return A reference to the blurred array.
   * \ingroup Filters
   */
  template <typename T,unsigned int n>
    Array<T,n>&
    deriche_blur(Array<T,n>& a, unsigned int dim, double sigma, bool cond = true)
    {
      return deriche_filter(a, sigma, DERICHE_BLUR, dim, cond);
    }

  /**
   * Blur the given array along all its dimensions.
   * To blur only along a given dimension use either
   * deriche_blur(Array<T>&,int,double,bool) or
   * deriche_filter(Array<T>&,double,DericheFilteringType,int,bool)
   * \ingroup Filters
   */
  template <typename T, unsigned int n>
    Array<T,n>&
    deriche_blur(Array<T,n>& a, double sigma)
    {
      for (ArrayIndex i = 0; i < n; i++)
	deriche_blur(a, i, sigma);
      return a;
    }
}
