Comparisons and arithmetics
===========================

.. highlight:: c++

Delayed arrays can be the result of arithmetic expressions between
arbitrary arrays.

Comparisons
-----------

.. cpp:function:: auto operator==(const Array1& a, const Array2& b)

   Element-wise comparison of two :doc:`indexable arrays <../arrays>`.

   The two arrays ``a`` and ``b`` must have the same shapes (you may
   want to consider :doc:`broadcasting <broadcasting>` otherwise), and
   have types for which the operator ``==`` is defined.

   The returned delayed array will have the same shape as ``a`` and
   ``b`` with a boolean value indicating whether the elements at the
   corresponding position in ``a`` and ``b`` are equals.
   
.. cpp:function:: auto operator!=(const Array1& a, const Array2& b)

   Idem but using operator ``!=`` to compare elements.
   
.. cpp:function:: auto operator>(const Array1& a, const Array2& b)

   Idem but using operator ``>`` to compare elements.
   
.. cpp:function:: auto operator<(const Array1& a, const Array2& b)

   Idem but for operator ``<``.

Arithmetics
-----------

.. cpp:function:: auto operator*(const Array1& a, const Array2& b)

   Element-wise product of two :doc:`indexable arrays <../arrays>`.

   The element type of the returned delayed array will be the `common
   type`_ among ``Array1::dtype`` and ``Array2::dtype``.

   .. _common type: http://en.cppreference.com/w/cpp/types/common_type

.. cpp:function:: auto operator/(const Array1& a, const Array2& b)

   Idem but for operator ``/``.
   

.. cpp:function:: auto operator+(const Array1& a, const Array2& b)

   Idem but for operator ``+``.
   

.. cpp:function:: auto operator-(const Array1& a, const Array2& b)

   Idem but for operator ``-``.
   
Global operations
-----------------

.. cpp:function:: T norm(const Array& a, Norm norm)

   Compute the norm of an array.

.. cpp:type:: Norm

   Order of a norm. Must be ``Infinity``.

Element operations
------------------

.. cpp:function:: auto abs(const Array& a)

   Return an equivalent delayed array whose elements are the absolute
   of those in ``a``.

.. cpp:function:: auto exp(const Array& a)

   Idem but calling ``exp`` on each element.

.. cpp:function:: auto sqrt(const Array& a)

   Idem but calling ``sqrt`` on each element.
