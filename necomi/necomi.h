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
#include "delayed/broadcasting.h"
#include "delayed/comparisons.h"
#include "delayed/ranges.h"
#include "delayed/transforms.h"

// Numerics
#include "numerics/exponents.h"
#include "numerics/interpolation.h"
#include "numerics/random.h"
#include "numerics/sde.h"
#include "numerics/statistics.h"
#include "numerics/trigonometrics.h"

// Filters
#include "filters/deriche.h"

// Codecs
#include "codecs/streams.h"
#ifdef HAVE_HDF5
#include "codecs/hdf5.h"
#endif
#ifdef HAVE_LIBPNG
#include "codecs/png.h"
#endif

// Old uncleaned untested headers
#if 0
#include "codecs/inr.h"
#endif

// Local Variables:
// mode: c++
// End:
