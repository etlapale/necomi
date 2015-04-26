Constants and ranges
====================

.. highlight:: c++

Those functions construct delayed arrays from a few scalar values.
	       
Constant arrays
---------------

.. cpp:function:: auto zeros<T>(...)

   Create a multidimensional delayed array filled with zero values.

   The dimensions are given either as successive parameters to the
   function, each of them should be convertible to ``std::size_t``, or
   as a single ``Dimensions<N>`` parameter.

   ``T`` defaults to ``double``.

   ::

      // Create a 3D array of 7×5×9 double elements set to 0
      auto a = zeros(7,5,9);
      // Create a 2D array of float elements
      auto b = zeros<float>(11,67);
      // Create a 3D array
      Dimensions<3> d = {4,7,11};
      auto c = zeros(d);

.. cpp:function:: auto constants(const Dimensions<N>& dims, T value)

   Idem but for arrays filled with a single arbitrary value.

.. cpp:function:: auto zeros_like(const Array& a)

   Create a delayed array similar to ``a`` but zero-filled.

   The resulting delayed array will have the shape and element type of
   ``a``.

   ``Array`` must follow the :doc:`Array concept <../arrays>`.

.. cpp:function:: auto constants_like(const Array& a, T value)

   Idem for an arbitrary element value.

Ranges
------

.. cpp:function:: auto range(T n)

   Return a 1D delayed array filled with the integers in [0,n).

   The integers are casted to ``T``, the type of the resulting array.

   ::

      // Elements will be {0, 1, 2, 3}
      auto a = range(4);

.. cpp:function:: auto range(T start, T stop, T step=1)

   Create a 1D delayed array with integer values in [start,stop) with
   intervals of size ``step``.

   ::
      
      // Elements will be {3,7,11}
      auto b = range(3, 12, 4);

.. cpp:function:: auto linspace(T start, T stop, std::size_t size, bool endpoint=true)

   Create a 1D delayed array with ``size`` elements between ``start``
   and ``stop``.

   ``endpoint`` sets whether ``stop`` should be included or not in the
   resulting array.

   ::

      // Returns {0, 3.333, 6.667, …, 30}
      auto a = linspace(0, 30, 10);
      // Returns {0, 3, 6, …, 27}
      auto b = linspace(0, 30, 10, false);

Matrices
--------

.. cpp:function:: auto identity<T>(std::size_t dim)

   Return a ``dim×dim`` identity matrix (2D array).

   All elements along the diagonal will have a value of 1, the others
   of 0.

   ``T`` defaults to ``double``.
