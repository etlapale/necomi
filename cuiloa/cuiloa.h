#pragma once

#include "base/array.h"
#if 0
#include "filters/deriche.h"
#ifdef HAVE_HDF5
#include "codecs/hdf5.h"
#endif
#include "codecs/inr.h"
#endif


/**
 * \mainpage
 *
 * Cuiloa is a multi-dimensional array library centered on
 * cuiloa::Array. The Modules page
 * contains a list of the modules to operate on this arrays including Codecs
 * to read/write arrays on permanent storage and with different formats and
 * Filters to apply filters on the arrays.
 */

/**
 * \defgroup Codecs
 * Define input/output utilities to save or restore arrays.
 *
 * \defgroup Filters
 * Define some filters which can be used on arrays.
 */
