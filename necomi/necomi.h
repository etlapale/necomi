// necomi/necomi.h – Include all headers
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

// Type traits
#include "traits/arrays.h"
#include "traits/shape.h"
#include "traits/generic.h"

// Core definitions
#include "core/coordinates.h"
#include "core/loops.h"
#include "core/mpl.h"
#include "core/shape.h"
#include "core/slices.h"
#include "core/strides.h"

// Default array classes
#include "arrays/delayed.h"
#include "arrays/stridedarray.h"
#include "arrays/vararray.h"

// Array creation
#include "delayed/arithmetic.h"
#include "delayed/comparisons.h"
#include "delayed/ranges.h"
#include "delayed/transforms.h"

// Numerics
#include "numerics/exponents.h"
#include "numerics/interpolation.h"
#include "numerics/random.h"
#include "numerics/statistics.h"
#include "numerics/trigonometrics.h"

// Codecs
#ifdef HAVE_HDF5
#include "codecs/hdf5.h"
#endif
#ifdef HAVE_LIBPNG
#include "codecs/png.h"
#endif


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

#include "base/array.h"
#include "base/broadcasting.h"
#include "base/delayed.h"

#include "funcs/numerics.h"

#include "integrate/sde.h"

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
