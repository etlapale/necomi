#pragma once

#include "base/array.h"
#include "filters/deriche.h"
#ifdef HAVE_HDF5
#include "codecs/hdf5.h"
#endif
#include "codecs/inr.h"

/**
 * \defgroup Codecs
 * Define input/output utilities to save or restore arrays.
 *
 * \defgroup Filters
 * Define some filters which can be used on arrays.
 */
