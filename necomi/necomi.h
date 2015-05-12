// necomi/necomi.h – Include all headers
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

// Type traits
#include "traits/arrays.h"
#include "traits/generic.h"

// Core definitions
#include "core/loops.h"
#include "core/shape.h"
#include "core/slices.h"
#include "core/strides.h"

// Default array classes
#include "arrays/delayed.h"
#include "arrays/stridedarray.h"
#include "arrays/vararray.h"

// Array creation
#include "delayed/ranges.h"

// Numerics
#include "numerics/interpolation.h"


#if 0
namespace necomi {
  /**
   * Divide each element of the array by a number.
   */
  template <typename U,
	    std::enable_if_t<is_promotable<U,T>::value>* = nullptr>
  Array<T,N>&
  operator/=(const U& div)
  {
    this->map([&div](auto& path, T& val) {
	(void) path;
	val /= div;
      });
    return *this;
  }
}
#endif

#if 0
#include "base/array.h"
#include "base/broadcasting.h"
#include "base/delayed.h"
#include "base/dynarray.h"

#include "funcs/numerics.h"
#include "funcs/random.h"
#include "funcs/trigonometrics.h"

#include "integrate/sde.h"

/**
 * \defgroup Codecs Input/output functions
 *
 * Define input/output utilities to store arrays on disk or load them.
 */
#ifdef HAVE_HDF5
#include "codecs/hdf5.h"
#endif

#ifdef HAVE_LIBPNG
#include "codecs/png.h"
#endif

//#include "codecs/inr.h"

/**
 * \defgroup filters Recursive filtering
 * Implement recursive filters such as Canny-Deriche or Gamma filters.
 */
#include "filters/deriche.h"
#endif

// Local Variables:
// mode: c++
// End:
