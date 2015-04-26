Broadcasting
============

.. highlight:: c++

Broadcasting allows to apply binary operations on arrays with
different sizes. Two arrays are compatible for broadcasting if the
dimensions of one of them are a prefix or a suffix of the dimensions
of the other. For instance, arrays with dimensions 3×7×8 and 2×3×7×8
are broadcastable.

To enable broadcasting use::

  using namespace cuiloa::delayed::broadcasting;


Widening
--------

.. cpp:function:: auto widen(const Dimensions<M>& dims, const Array& a)
		  
   Return a delayed array of dimensions ``dims`` whose elements
   replicates those of ``a``.

   The ``N`` dimensions of ``a`` must be a suffix of the ``M`` dimensions
   in ``dims``, hence ``M>N``. The element at coordinates
   ``(x0,x1,…,xM)`` in the returned array will be the one at
   coordinates ``(x(M-N),…,xM)`` in the original array.

   Throw a `std::length_error`_ if the array cannot be broadcasted.

   .. _`std::length_error`: http://en.cppreference.com/w/cpp/error/length_error

.. cpp:function:: auto widen_right(const Dimensions<M>& dims, const Array& a)

   Idem but with extra dimensions at the end instead of at the
   beginning.

Operators
---------

.. cpp:function:: auto operator*(const Array1& a, const Array2& b)

   Element-wise product of two arrays with broadcasting.

   If one of the arrays as a lower dimensionality than the other its
   dimensions will be broadcasted.
   
   Throw a `std::length_error`_ if the lower dimensionality array
   cannot be broadcasted.

.. cpp:function:: auto operator/(const Array1& a, const Array2& b)

   Idem but for operator ``/``.
   

.. cpp:function:: auto operator+(const Array1& a, const Array2& b)

   Idem but for operator ``+``.
   

.. cpp:function:: auto operator-(const Array1& a, const Array2& b)

   Idem but for operator ``-``.
   

