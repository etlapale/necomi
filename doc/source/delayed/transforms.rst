Transformations
===============

.. highlight:: c++

Those functions transform existing arrays.

Reordering
----------

.. cpp:function:: auto reshape(const Array& a, const Dimensions<M>& d)

   Reshape an existing array into a delayed one of shape ``d``.

   All the elements of ``a`` will be present in the returned array in
   the same (row-major_) order, but may appear at different
   coordinates. The product of the dimensions of ``a`` and the product
   of elements in ``d`` must therefore be equal.

   .. _row-major: https://en.wikipedia.org/wiki/Row-major_order

.. cpp:function:: auto reshape(const Array& a, ...)

   Idem but specify the resulting dimensions as separated arguments.
   
   ::

      // Construct a 2D array containing:
      //     {{ 0,  1,  2,  3,  4},
      //      { 5,  6,  7,  8,  9},
      //      {10, 11, 12, 13, 14}}
      auto a = reshape(range(15), 3, 5);
      
.. cpp:function:: auto roll(const Array& a, std::ssize_t shift, std::size_t dim)

   Shift elements along the ``dim`` dimension by ``shift``.

   ::

      // Returns {3, 4, 0, 1, 2}
      auto a = roll(range(5), 2);

   Last argument ``dim`` is optional for 1D array.

Slicing
-------

.. cpp:function:: auto fix_dimension(const Array& a, std::size_t dim, std::size_t val)

   Create an N-1 dimensional array from an N-dimensional one.
   
Combinations
------------

.. cpp:function:: auto stack(const Array1& a, ...)

   Combine N-dimensional arrays into a (N+1) dimensional delayed array.

   The first dimension in the resulting array will select one of the
   combined array, the others will select an element.

Modifications
-------------

.. cpp:function:: auto shifted(const Array& a, Dimensions<N> offset, T val=0)

   Offset the coordinates of each element by ``offset``.

   Coordinates outside the original elements are set to ``val``.
