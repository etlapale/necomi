#pragma once

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

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
    PredType
    hdf5_pred_type(T t)
    {
      throw std::runtime_error("Unmanaged primitive type for the HDF5 codec");
    }

    PredType
    hdf5_pred_type(double d)
    {
      (void) d;
      return PredType::NATIVE_DOUBLE;
    }

  template <typename T>
    Array<T> hdf5_load(const char* filename, const char* dset_name)
    {
      H5File file(filename, H5F_ACC_RDONLY);
      DataSet dset = file.openDataSet (dset_name);
      DataSpace dspace = dset.getSpace();
      int rank = dspace.getSimpleExtentNdims();
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

      PredType hptype = hdf5_pred_type(*data);
      dset.read(data, hptype, H5S_ALL, H5S_ALL);

      Array<T> a(rank, dims, data);
      delete [] dims;
      return a;
    }
}
