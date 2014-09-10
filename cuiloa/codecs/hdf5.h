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

#include <fstream>
#include <iostream>
#include <stdexcept>

#include "../base/array.h"

#include <H5Cpp.h>

#ifndef H5_NO_NAMESPACE
using namespace H5;
#endif


/**
 * \file hdf5.h HDF5 files utilities.
 * \ingroup Codecs
 */

namespace cuiloa
{

/**
 * Converts a C++ type to an HDF5 PredType value.
 * This class template is specialized for the supported C++ types
 * by defining a static function `type()` returning a `PredType`.
 */
template <typename T>
struct pred_type;

template <>
struct pred_type<float>
{
  static PredType type()
  { return PredType::NATIVE_FLOAT; }
};

template <>
struct pred_type<double>
{
  static PredType type()
  { return PredType::NATIVE_DOUBLE; }
};

template <>
struct pred_type<long double>
{
  static PredType type()
  { return PredType::NATIVE_LDOUBLE; }
};

template <>
struct pred_type<char>
{
  static PredType type()
  { return PredType::NATIVE_CHAR; }
};

template <>
struct pred_type<unsigned char>
{
  static PredType type()
  { return PredType::NATIVE_UCHAR; }
};

template <>
struct pred_type<short>
{
  static PredType type()
  { return PredType::NATIVE_SHORT; }
};

template <>
struct pred_type<unsigned short>
{
  static PredType type()
  { return PredType::NATIVE_USHORT; }
};

template <>
struct pred_type<int>
{
  static PredType type()
  { return PredType::NATIVE_INT; }
};

template <>
struct pred_type<unsigned int>
{
  static PredType type()
  { return PredType::NATIVE_UINT; }
};

template <>
struct pred_type<long>
{
  static PredType type()
  { return PredType::NATIVE_LONG; }
};

template <>
struct pred_type<unsigned long>
{
  static PredType type()
  { return PredType::NATIVE_ULONG; }
};

template <>
struct pred_type<long long>
{
  static PredType type()
  { return PredType::NATIVE_LLONG; }
};

template <>
struct pred_type<unsigned long long>
{
  static PredType type()
  { return PredType::NATIVE_ULLONG; }
};

/**
 * Read a full dataset into a new contiguous Array.
 * The dataset dimensionality must be `N`.
 * \ingroup Codecs
 */
template <typename T, ArrayIndex N>
Array<T,N> hdf5_load(const char* filename, const char* dset_name)
{
  // Open the file and its dataset
  H5File file(filename, H5F_ACC_RDONLY);
  DataSet dset = file.openDataSet (dset_name);
  DataSpace dspace = dset.getSpace();

  // Make sure we have the same rank
  int rank = dspace.getSimpleExtentNdims();
  if (rank != N) {
    throw std::runtime_error("Invalid HDF5 dimensionality");
  }

  // Get the dimensions and copy them to an std::array
  hsize_t hdims[N];
  dspace.getSimpleExtentDims(hdims);
  std::array<ArrayIndex,N> dims;
  for (ArrayIndex i = 0; i < N; i++)
    dims[i] = hdims[i];

  // Create the array (allocates the data)
  Array<T,N> a(dims);

  // Read the data from the dataset in the array
  PredType hptype = pred_type<T>::type();
  dset.read(a.data(), hptype, H5S_ALL, H5S_ALL);

  return a;
}

/**
 * Create a new dataset of the given dimension in an opened HDF5 file.
 * If `duplicates` is greater than zero, an extra first dimension will
 * be added into the dataset, allowing storage of several copies of the
 * array.
 *
 * \ingroup Codecs
 */
template <typename T, ArrayIndex N>
DataSet hdf5_create_dataset(const Array<T,N>& a,
                            H5File& hf,
                            const char* dset_name,
                            hsize_t duplicates=0,
                            PredType output_type = pred_type<T>::type())
{
  // Dataset dimensions
  auto ndims = N + (duplicates > 0);
  hsize_t dims[ndims];
  if (duplicates > 0)
    dims[0] = duplicates;
  std::copy(a.dimensions().cbegin(),
            a.dimensions().cend(),
            dims + (duplicates > 0));
  DataSpace dspace(ndims, dims);

  // Create the dataset
  return hf.createDataSet(dset_name, output_type, dspace);
}

/**
 * Store an array as a slice in a dataset.
 * The dataset would have typically been created by hdf5_create_dataset.
 *
 * \ingroup Codecs
 * \param   slice       Number of the slice that should be stored.
 * \see     hdf5_create_dataset
 * \note The current implementation creates a temporary copy of
 *       the array if the original one is not contiguous.
 */
template <typename T, ArrayIndex N>
void hdf5_store_slice(const Array<T,N>& a, DataSet& dset, hsize_t slice)
{
  // Get the dataset dimensions
  auto dset_space = dset.getSpace();
  auto dset_ndims = dset_space.getSimpleExtentNdims();
  hsize_t dset_dims[dset_ndims];
  dset_space.getSimpleExtentDims(dset_dims);

  // Select a slab in the dataset
  hsize_t dset_start[dset_ndims];
  std::fill_n(dset_start, dset_ndims, 0);
  dset_dims[0] = 1;
  dset_start[0] = slice;
  dset_space.selectHyperslab (H5S_SELECT_SET, dset_dims, dset_start);

  // Define slice size
  DataSpace mem_space(dset_ndims, dset_dims);

  // Copy the slice
  dset.write((a.contiguous() ? a : a.copy()).data(),
             pred_type<T>::type(),
             mem_space,
             dset_space);
}

/**
 * Store an array in an already opened HDF5 file.
 *
 * \ingroup Codecs
 * \note The current implementation creates a temporary copy of
 *       the array if the original one is not contiguous.
 */
template <typename T, ArrayIndex N>
void hdf5_save(const Array<T,N>& a,
               H5File& hf,
               const char* dset_name,
               PredType output_type = pred_type<T>::type())
{
  // Dataset dimensions
  hsize_t dims[N];
  std::copy(a.dimensions().cbegin(), a.dimensions().cend(), dims);
  DataSpace dspace(N, dims);

  // Create the dataset
  auto dset = hf.createDataSet(dset_name, output_type, dspace);

  // Copy the dataset
  dset.write((a.contiguous() ? a : a.copy()).data(), pred_type<T>::type());
}

/**
 * Store a array in a single-dataset HDF5 file.
 *
 * \ingroup Codecs
 * \warning If the HDF5 file already exists, it will be erased.
 */
template <typename T, ArrayIndex N>
void hdf5_save(const Array<T,N>& a,
               const char* path,
               const char* dset,
               PredType output_type = pred_type<T>::type())
{
  // Create the HDF5 file
  H5File hf(path, H5F_ACC_TRUNC);

  // Store the dataset
  hdf5_save<T,N>(a, hf, dset, output_type);
}

} // namespace cuiloa
