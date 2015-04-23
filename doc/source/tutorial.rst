Tutorial
========

.. highlight:: c++

This tutorial will guide you through the most common features of the
necomi library. It assumes that the reader have a decent knowledge of
C++ and knows how to use namespaces and templates. Usage of C++11/14 features
includes links to external documentation.

Array construction
------------------
The most common objects manipulated in necomi are multidimensional
arrays. Each array contains a number of elements, which may or may no
be present in memory, and which are accessible through their coordinates.
Both arrays and coordinates are C++ types available in the ``necomi``
namespace:

.. code:: c++

   #include <necomi/necomi.h>
   using namespace necomi;
   Array<double,2> a(5,4);

Here, we declared a two dimensional array of 5×4 elements. We also
specified that the array elements are floating point numbers of type
``double``. Yet, the elements in this array have undefined value, so
you may prefer to use one of the construction functions available in
the ``necomi::delayed`` namespace:

.. code:: c++

   using namespace necomi::delayed;
   Array<int,1> b = range(10);
	  
Here we create an 1D array of 10 elements whose elements take the
successive values between 0 and 9. The ``range`` function can accept
additional arguments to further define the numerical range, and
several other construction function are defined. For instance, to
create an array of random values taken from a normal distribution:

.. code:: c++

   // Mersenne-Twister with a 19937 bits seed taken from the
   // operating system random device
   RandomDevSeedSequence rdss;
   std::mt19937_64 prng(rdss);
   // Create a 2D array of 5×4 normally distributed numbers
   Array<double,2> c = normal({5,4}, prng);

Finally, an array can also be created from a larger expression:

.. code:: c++

   Array<double,1> d = sqrt(0.01)*normal(1000, prng);

Elements and views
------------------

Provided the elements are stored contiguously, an array can be
reshaped to any dimensions, as long as the overall size, i.e. the number
of elements in it, is not changed:

.. code:: c++

   Array<int,1> e = range(20);
   // e := [0 1 2 3 … 17 18 19]
   Array<int,2> f = reshape(e, {5, 4});
   /* f := [ 0  1  2  3  4
             5  6  7  8  9
            10 11 12 13 14
            15 16 17 18 19] */

To access any of the elements in an array, you can use the ``()``
operator, which takes ``N`` coordinates as argument, where ``N`` is
the dimensionality of the array:


.. code:: c++

   // Will print 0 and 11
   cout << "elem at   (0) in e: " <<   e(0) << endl;
   cout << "elem at (2,1) in f: " << f(2,1) << endl;

Where the coordinates are 0-based, that is ``0`` is the first
coordinate in a given dimension, following the standard C++ convention.

You can create *views* on restricted portions of arrays.  To do so,
you have to pass a slice object to the operator ``()``. A slice object
represent, for each coordinate, the beginning of a range, the number
of elements, and, optionally, a step size.

.. code:: c++

   Array<int,1> e_view = e(slice(3, 3));
   // e_view := [3 4 5]
   Array<int,2> f_view = e((slice(1, 3),slice(1,2)));
   /* f_view := [ 6  7
	         11 12
		 16 17 ] */

Note that views are shared, so if you modify an element of ``e_view``
or ``f_view``, the same element will be modified in ``e`` or ``f``.

.. code:: c++

   // Also change e(4) to 42
   e_view(1) = 42;

Array expressions
-----------------
In C++ it is pretty easy to create long expressions to perform various
computations, such as ``double y = 2*pi + sqrt(x);``. necomi allows
to use the same syntax, where each operation is performed on all array
elements. If you type ``a + b``, where ``a`` and ``b`` are arrays with
the same dimensions, a resulting array is return where each element is
the sum of the corresponding elements in ``a`` and ``b``.

.. code:: c++

   Array<double,3> g(1000,1000,16);
   Array<double,3> h(1000,1000,16);
   Array<double,3> k = g*3.14 + exp(h*g + sqrt(h));

When manipulating large multidimensional arrays, one would like to
avoid creating intermediate results. In the last example, there is no
need to create an array containing the result of ``g*3.14``, another
for ``h*g``, another for ``sqrt(h)`` and so on. Indeed, necomi
store those operations in *delayed arrays*, and only evaluate them
when required. Indeed, had we written the last line of the example as:

.. code:: c++

   auto k = g*3.14 + exp(…);


then ``k`` would not have been an instance of
``Array<double,3>>``, but an instance of the ``DelayedArray`` template
class. Here auto_ is the C++11 keyword to let the compiler infer the
type of a variable.

Delayed arrays, as opposed to the ``Array`` instances that we
would hereafter refer to as *immediate arrays*, do not contain their
elements. Indeed they only contain the computations required to
compute each element. When we use ``Array<double,3>`` instead of
``auto``, we force a conversion to an immediate array, effectively
computing the value of each element.

Note that you may create a delayed array without refering to any
existing array, for instance:

.. code:: c++

   auto m = make_delayed<int,1>({13}, [](auto&) { return 42; });

will create a delayed 1D array of 13 integer elements each containing
the value 42. Here ``[](){}`` is the syntax to create a `lambda
function`_ in C++11.

Note that delayed arrays do not implement any form of memoization, so
if you want to get their values multiple times, you should convert
them to an immediate array.

More information about delayed arrays can be found on
the `dedicated page </code/necomi/delayed>`__

Broadcasting
------------
Broadcasting allows you to create expressions with arrays of different
dimensionality, on the condition that all the dimensions of the
smaller dimensionality array are the same as the last dimensions of
the larger dimensionality array. Broadcasting operations are
available in the ``necomi::delayed::broadcasting`` namespace:

.. code:: c++

   auto a = range(4);                    // 1D array of size 4
   auto b = reshape(range(8), 2, 4); // 2D array of size 2×4
   // Create a 2D array with c(i,j) = a(j) + b(i,j);
   auto c = a * b;

.. _auto: http://en.cppreference.com/w/cpp/language/auto
.. _lambda function: http://en.cppreference.com/w/cpp/language/lambda
