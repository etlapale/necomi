Array classes
=============

StridedArray
    A standard form of in-memory arrays in which elements are
    either contiguously stored or separated by regular offsets.

DelayedArray
    A functional array which stores only a function to return the
    value of (or reference to) each element. There is no requirement
    for the elements to exist in memory.

.. toctree::
   :maxdepth: 2

   delayed
