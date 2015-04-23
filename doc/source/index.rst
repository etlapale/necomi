Necomi
======

.. highlight:: c++

Necomi is a C++ template library for multidimensional arrays. It
allows easy manipulation of arrays including slices, shared views,
I/O, or conversion to and from raw C arrays. It also implements
several algorithms such as multidimensional recursive filters.

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
   :maxdepth: 3

   install
   tutorial
   overview
   license

Releases
--------
`cuiloa-0.3.1.tar.xz <http://emilien.tlapale.com/data/necomi/cuiloa-0.3.1.tar.xz>`_
(2014-11-20) Added operators, trigonometrics and interpolation functions.

`archives <http://emilien.tlapale.com/data/necomi/>`_

