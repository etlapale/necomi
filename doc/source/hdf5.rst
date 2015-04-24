HDF5
====

.. highlight:: c++

HDF5_ is a widely-used file format to store multi-dimensional
arrays. We provide functions to facilitate the interface to the `HDF5
C++ library`_. Those functions are only available if ``HAVE_HDF5`` is
defined and if the HDF5 C++ library is installed.

.. _HDF5: http://www.hdfgroup.org/HDF5/
.. _HDF5 C++ library: http://www.hdfgroup.org/HDF5/doc/cpplus_RM/index.html

High-level
----------

The high-level functions are one-liners to save and load arrays but
they require the full dataset to be stored in memory, and will delete
any existing data on save.

.. cpp:function:: Array<T,N> hdf5_load(const char* path, const char* dset)

   Read a full dataset into a new contiguous array.

   If the dimensionality of the dataset and of the returned array
   ``N`` do not match, an exception is thrown. The original values are
   converted to ``T`` by the HDF5 library.

   ::
      
      // Load a 3D array as double elements
      auto a = hdf5_load<double,3>("myfile.h5", "mydset");

.. cpp:function:: void hdf5_save(const char* path, const char* dset, const Array<T,N>& a)

   Store a full dataset in a new HDF5 file.

   **Warning:** If the file already exists, it is destroyed.

.. cpp:function:: void hdf5_save(const char* path, const char* dset, const Array<T,N>& a, PredType type)

   Idem, but specify an element type for the stored values. Mostly used to
   specify a portable format instead of the native ones inferred by
   default.

   ::
      
      hdf5_save(a, "myfile.h5", "mydset", PredType::IEEE_F64LE);

Dataset manipulations
---------------------
   
.. cpp:function:: void hdf5_create_dataset(H5File& hf, const char* dset, const Dimensions<N>& dims)

   Create an uninitialized dataset in an opened HDF5 file.

   You can use a template parameter ``hdf5_create_dataset<T>`` to
   specify a native type (``double`` by default), or use an explicit
   HDF5 data type with the next overload.

   ::

      H5File hf("myfile.h5", H5F_ACC_TRUNC);
      auto dset = hdf5_create_dataset<float>(hf, "mydset", dims);

.. cpp:function:: void hdf5_create_dataset(H5File& hf, const char* dset, const Dimensions<N>& dims, PredType type)

   Idem but with a specific HDF5 data type for the dataset.

   ::

      hdf5_create_dataset(hf, "mydset", dims, PredType::IEEE_F64BE);
		  
.. cpp:function:: void hdf5_store_slice(const Array<T,N>& a, DataSet& dset, hsize_t slice)

   Store a full array as a single slice in an existing HDF5 dataset.

   This function allows to write a dataset slice by slice when it is
   too large to fit in memory.

   ::
      
      auto dset = hdf5_create_dataset(hf, "mydset", dset_dims);
      for (int i = 0; i < n; i++) {
        auto a = large_array();
        hdf5_store_slice(dset, i, a);
      }

.. cpp:function:: void hdf5_save(H5File& hf, const char* dset, const Array<T,N>& a)

   Same as :cpp:func:`hdf5_save` but with an opened HDF5 file.

.. cpp:function:: void hdf5_save(H5File& hf, const char* dset, const Array<T,N>& a, PredType type)

   Idem but with a dataset type.
