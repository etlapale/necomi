Necomi
======

.. highlight:: c++

Necomi is a C++ template library for multidimensional arrays. It
allows easy manipulation of slices, shared views, :doc:`I/O <io>`, and
provides multiple :doc:`numerical functions <numerics>`.

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
   io
   
*  :ref:`genindex`
*  :ref:`search`

.. toctree::
   :hidden:

   license

Download
--------
`necomi-0.4.tar.xz <http://necomi.atelo.org/data/necomi-0.4.tar.xz>`_
(2015-04-24) Renamed, documented with Sphinx_, removed CRTP, added
reshaping and numerical functions.

`archives <http://necomi.atelo.org/data/>`_

`Project website <https://git.atelo.org/etlapale/necomi>`_

.. _Sphinx: http://sphinx-doc.org
