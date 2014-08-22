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
 * 
 * This file defines codecs for HDF5 files.
 */

namespace cuiloa
{

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

template <typename T, ArrayIndex N>
  Array<T,N> hdf5_load(const char* filename, const char* dset_name)
  {
    H5File file(filename, H5F_ACC_RDONLY);
    DataSet dset = file.openDataSet (dset_name);
    DataSpace dspace = dset.getSpace();
    int rank = dspace.getSimpleExtentNdims();
    if (rank != N) {
      throw std::runtime_error("Invalid HDF5 dimensionality");
    }

    hsize_t* hdims = new hsize_t[rank];
    dspace.getSimpleExtentDims(hdims);
    int size = 1;
    int* dims = new int[rank];
    for (int i = 0; i < rank; i++)
      {
        dims[i] = hdims[i];
        size *= dims[i];
      }
    delete [] hdims;

    T* data = new T[size];

    PredType hptype = pred_type<T>::type();
    dset.read(data, hptype, H5S_ALL, H5S_ALL);

    Array<T,N> a(dims, data);
    delete [] dims;
    return a;
  }

/**
 * \warning If the HDF5 file already exists, it will be erased.
 */
template <typename T, ArrayIndex N>
void hdf5_save(Array<T,N>& a,
               const char* path,
               const char* dset_name,
               PredType output_type = pred_type<T>::type())
{
  // Create the HDF5 file
  H5File hf(path, H5F_ACC_TRUNC);

  // Dataset dimensions
  hsize_t dims[N];
  std::copy(a.dimensions().cbegin(), a.dimensions().cend(), dims);
  DataSpace dspace(N, dims);

  // Create the dataset
  auto dset = hf.createDataSet(dset_name, output_type, dspace);

  // TODO check for array contiguity!!!
  std::cerr << "WARNING: assuming array contiguity for hdf5_save!" << std::endl;
  dset.write(a.data(), pred_type<T>::type());
}

} // namespace cuiloa
