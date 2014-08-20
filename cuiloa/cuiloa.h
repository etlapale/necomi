// cuiloa - A scientific multi-dimensional array library.
#pragma once

#include "base/array.h"
#include "filters/deriche.h"
#ifdef HAVE_HDF5
#include "codecs/hdf5.h"
#endif
#include "codecs/inr.h"


/**
 * \mainpage
 *
 * Cuiloa is a scientific multi-dimensional array library.
 *
 * \section Introduction
 *
 * Cuiloa is a scientific library built on a multi-dimensional
 * array class: cuiloa::Array. The Modules page contains
 * a list of the modules to operate on this arrays including
 * Codecs to read/write arrays on permanent storage and with
 * different formats and Filters to apply filters on the arrays.
 *
 * \section License
 * \verbatim
cuiloa - A scientific multi-dimensional array library.
Copyright (C) 2007  Émilien Tlapale

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
\endverbatim
*/

/**
 * \defgroup Codecs
 * Define input/output utilities to save or restore arrays.
 *
 * \defgroup Filters
 * Define some filters which can be used on arrays.
 */
