Arrays
======

Concepts
--------

Necomi defines several C++ concepts, named set of requirements, to be
implemented by the actual array classes.

Array
    Elementary array with an element type and a set of dimensions.

IndexableArray
    Array in which each element can be accessed through a set of
    coordinates.

ReferencableArray
    IndexableArray in which a reference to an element can be returned.

Although the C++ standard does
`not yet <http://en.cppreference.com/w/cpp/language/constraints>`_
include a formal way to verify that those concepts are matched by specific
classes, we provide a set a of utilities to check for those
constraints in ``necomi/base/concepts.h``.

Array classes
-------------

StridedArray
    A standard form of in-memory arrays in which elements are
    either contiguously stored or separated by regular offsets.

DelayedArray
    A functional array which stores only a function to return the
    value of (or reference to) each element. There is no requirement
    for the elements to exist in memory.

.. toctree::
   :maxdepth: 4

   delayed
