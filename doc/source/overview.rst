Overview
========

Concepts
--------

Necomi defines several C++ concepts, named set of requirements, to be
implemented by the actual array classes. The array concepts are as followed:

Array
    Elementary array with an element type and a set of dimensions.

IndexableArray
    Array in which each element can be accessed through a set of
    coordinates.

ModifiableArray
    IndexableArray in which a reference to an element ca be returned.

Although the C++ standard does
`not yet <http://en.cppreference.com/w/cpp/language/constraints>`_
include a formal way to verify that those concepts are matched by specific
classes, we provide a set a of utilities to check for those
constraints in `necomi/base/concepts.h`.

Array Classes
-------------

.. toctree::
   :maxdepth: 3

   delayed
