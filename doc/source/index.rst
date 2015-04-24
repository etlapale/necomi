Necomi
======

.. highlight:: c++

Necomi is a C++ template library for multidimensional arrays. It
allows easy manipulation of slices, shared views, I/O, and provides
multiple numerical functions.

::
	  
    // Load a 3D double dataset from a HDF5 file
    auto a = hdf5_load<double,3>("myfile.h5", "dataset");
    // Get a shared 2D slice of the dataset
    auto b = a[1];
    // Blur the 2D slice
    deriche(b, 2.0);

Necomi is released under the :doc:`Simplified BSD License <license>`.

Documentation
-------------

.. toctree::
   :maxdepth: 1

   install
   tutorial
   arrays
   numerics
   license

Download
--------
`necomi-0.4.tar.xz <http://emilien.tlapale.com/data/necomi/necomi-0.4.tar.xz>`_
(2015-04-25) Renamed, documented with Sphinx_, removed CRTP, added
reshaping and numerical functions.

`archives <http://emilien.tlapale.com/data/necomi/>`_

.. _Sphinx: http://sphinx-doc.org
