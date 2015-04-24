Necomi
======

.. highlight:: c++

Necomi is a C++ template library for multidimensional arrays. It
allows easy manipulation of arrays including slices, shared views,
I/O, and provides multiple numerical functions.

::
	  
    // Load a 3D double dataset from a HDF5 file
    auto a = hdf5_load<double,3>("myfile.h5", "dataset");
    // Get a shared 2D slice of the dataset
    auto b = a[1];
    // Blur the 2D image `b`
    deriche(b, 2.0);

Necomi is released under the `Simplified BSD License`_.

.. _Simplified BSD License: /data/necomi/LICENSE

Documentation
-------------

.. toctree::
   :maxdepth: 2

   install
   tutorial
   concepts
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
