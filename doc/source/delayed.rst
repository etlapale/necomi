Delayed arrays
==============

.. highlight:: c++
	       
necomi_ 0.3 adds support for delayed arrays to avoid allocation of
temporary ones and multiple loops while creating complex array
expressions. When activated, typically through:

.. code:: c++

   using namespace necomi::delayed;

various kind of algebraic expressions on arrays will in fact create
delayed arrays. For instance, when you write:

.. code:: c++

   a*b + c
  
where ``a``, ``b`` and ``c`` are regular arrays, no computation occurs, and
the result is a ``DelayedArray`` instance. To effectively calculate
the value of each element of the resulting array, you can convert it
with, for instance:

.. code:: c++

   Array<int,2> d = a*b + c;

At this point a regular 2D array is built, each of its elements
initialized from the given expression. There are several advantages to
this approach. First, no intermediate memory to store all the elements
of ``a*b`` is required. Second, the compiler is able to optimize the
required computations inside the easily parallelisable single loop.
Finally, this formulation allows a natural
writing which easily generalize. In the last example, ``a*b + c``
could be written identically for arrays of any shape.

Creating delayed arrays
-----------------------
Our delayed arrays are a form of `expression templates`_, with a
flexible evaluation based on C++11 lambda functions. You can simply
use the already defined functions and operators returning delayed
arrays, like ``+``, ``*`` or ``exp``. But if you want to create your
own functions, you will need to instantiate the ``DelayedArray``
class. Typically, you first define a function that gives the value of
each element in the array:

.. code:: c++
	  
   int my_const_func(const std::array<ArrayIndex,2>& path)
   { return 42; }

Here, our delayed array will associate the constant value ``42.0`` to
each of its elements. You can the proceed to the array creation:

.. code:: c++

   std::array<ArrayIndex,2> dimensions{{11,21}};
   DelayedArray<int,2,int(&)(const std::array<ArrayIndex,2>&)>
   a(dimensions, my_const_func);

Now, you can compute all its elements in an immediate array, or
compute the value of a single element using the ``()`` operator
accessible to all arrays:

.. code:: c++

   Array<int,2> b = a;
   assert(a(1,1) == b(1,1));

Using the features of modern C++, you can use a lambda function
instead of a named one, and use ``auto`` to infer types.  You can also
use ``make_delayed()`` to infer the template types. The resulting code
is the one-liner:

.. code:: c++

   auto a = make_delayed<int,2>({{11,21}}, [](auto&) { return 42; });

Note that here only the third template argument could be inferred. The
second, ``2``, cannot be inferred from ``{{11,21}}`` as it is an
``std::initializer_list``.

You can also refer to other arrays passing them as closure arguments
to your function. A simple code to rotate by 90 degrees an image would
then be:

.. code:: c++

   auto img = png_load("orig.png");
   auto new_img =
     make_delayed<unsigned char>(img.dimensions(), [&img](auto& path) {
       return img(path[1], path[0], path[2]);
     });
   png_save(new_img, "rotated.png");
   
.. image:: ../../share/bitmaps/baboon.png
   :width: 128
.. image:: ../../share/bitmaps/baboon-rot90.png
   :width: 128

where ``png_load`` and ``png_save`` are necomi_’s interface to
libpng_. Here, ``new_img`` is a delayed array that gets converted to
an immediate one during the call to ``png_save``. If you need access
to the immediate array, you can specify an explicit conversion with:

.. code:: c++

   Array<unsigned char,2> new_img = make_delayed(…);

or the less verbose C++14 equivalent ``decltype(img)``.

Memory management
-----------------
Functions returning delayed arrays in the necomi_ library always store
their closure as copy. That means that you can write a function like:

.. code:: c++

   auto my_function() {
     Array<double,2> a(5000,4000);
     Array<double,2> b(5000,4000);
     // …
     return a + b;
   };

Here, the delayed array returned maintains a copy of ``a`` and ``b``
in its closure, despite those array having been destructed at the end
of the function call. The copy of those immediate arrays is
lightweight since ``necomi::Array`` instances share their elements
and the 5000×4000 elements are never duplicated.

This is pretty much invisible to the user, but if you define your own
functions creating delayed arrays you will want to make sure that you
apply the same pattern. Returning to our previous example, if you want
to return the rotated image after loading it you have to make sure to
store the original data by copy in the closure. So write ``[img]``
instead of ``[&img]``:

.. code:: c++

   auto load_rotated_image(const std::string& path) {
     auto img = png_load("orig.png");
     auto new_img = make_delayed<unsigned char>(img.dimensions(),
         [img](auto& path) { /* … */ });
     return new_img;
   }

The default copy constructor is fine for immediate or delay arrays,
but when working with abstract arrays, derived of the CRTP class
``BaseArray``, you must ensure that you apply the correct copy
constructor, with ``shallow_copy``.

.. code:: c++

   make_delayed<T>(dims, [img=img.shallow_copy()](…){…});

Loop example
------------
A typical compiler will optimize out delayed operations such that a
single loop is generated. For instance with the following code:

.. code:: c++

   Array<double,1> res(1024);
   Array<double,1> a(1024), b(1024);
   res = a*b + b*b;

can generate the following single assembly loop which includes the
two multiplications, with ``vmulsd``, and the addition, with
``vaddsd``. Note that the calls to the lambda functions wrapping the
multiplications and additions are optimized out by the compiler.

.. code:: gas
	  
  4035c0:    mov    %r9,%rbp
  4035c3:    sar    $0x20,%rbp
  4035c7:    mov    0x10(%rsp),%rdx
  4035cc:    vmovsd (%rdx,%rbp,8),%xmm0
  4035d1:    mov    %rdi,%rbp
  4035d4:    sar    $0x20,%rbp
  4035d8:    mov    0x8(%rsp),%rdx
  4035dd:    vmulsd (%rdx,%rbp,8),%xmm0,%xmm0
  4035e2:    mov    %rbx,%rbp
  4035e5:    sar    $0x20,%rbp
  4035e9:    mov    (%rsp),%rdx
  4035ed:    vmovsd (%rdx,%rbp,8),%xmm1
  4035f2:    mov    %r8,%rbp
  4035f5:    sar    $0x20,%rbp
  4035f9:    vmulsd (%r15,%rbp,8),%xmm1,%xmm1
  4035ff:    mov    %rcx,%rbp
  403602:    sar    $0x20,%rbp
  403606:    vaddsd %xmm1,%xmm0,%xmm0
  40360a:    vmovsd %xmm0,(%r10,%rbp,8)
  403610:    add    %rsi,%r8
  403613:    add    %r11,%rbx
  403616:    add    %r13,%rdi
  403619:    add    %r12,%r9
  40361c:    add    %r14,%rcx
  40361f:    dec    %rax
  403622:    jne    4035c0

where ``%rax`` contains the number of elements in this contiguous
array, and compiled with clang 3.5.

More complex operations would probably require manual SIMD in the library.

.. _necomi: /code/necomi
.. _expression templates: http://en.wikipedia.org/wiki/Expression_templates
.. _libpng: http://libpng.org/pub/png/libpng.html
.. _Repa: http://repa.ouroborus.net
