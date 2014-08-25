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

/**
 * \defgroup Core
 * Core array library.
 *
 * \defgroup Codecs
 * Define input/output utilities to save or restore arrays.
 *
 * \defgroup Filters
 * Define some filters which can be used on arrays.
 */

#include "base/array.h"
#include "filters/deriche.h"
#ifdef HAVE_HDF5
#include "codecs/hdf5.h"
#endif
#include "codecs/inr.h"
