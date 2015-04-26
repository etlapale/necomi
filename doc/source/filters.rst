Filtering
=========

.. highlight:: c++

Canny-Deriche
-------------

.. cpp:function:: auto deriche(Array& a, std::size_t dim, T sigma, DericheOrder order=DericheOrder::BLUR, bool cond=true)

   Filter a strided array of floating numbers in place with the
   Canny-Deriche algorithm.
  
   If the optional parameter ``dim`` is present, the algorithm will
   only be applied to along that dimension, otherwise all the
   dimensions will be filtered.

.. cpp:type:: DericheOrder

   Order at which to apply a Canny-Deriche filter. Can be one of
   ``BLUR``, ``FIRST_DERIVATIVE``, or ``SECOND_DERIVATIVE``.
