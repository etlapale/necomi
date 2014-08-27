# Cuiloa – A multi-dimensional array library	{#mainpage}

The core class of the library is cuiloa::Array, which defines a
multi-dimensional array with element type and dimensionality as template
parameters. Several general purpose functions are used to access
to the array elements, to modify them, and to create views on subsets
of the arrays sharing their elements.

~~~
// Create a 3D array of `int` elements
cuiloa::Array<int,3> a(4,5,6);

// Display the second element
std::cout << a(0,0,1) << std::endl;

// 2D view of type cuiloa::Array<int,2>
// and size 5×6, sharing the same elments
auto b = a[1];
~~~

While having compile-time defined dimensionality take away some
flexibility, it ensures dimensionality safety without run-time checks

~~~
// Both are rejected by the compiler
cuiloa::Array<int,4> c = a[1];
std::cout << a(0,1) << std::endl;
~~~

and allows efficient traversal of non-contiguous memory

~~~
// Increment each element by a position-dependent value
b.map([](auto& path, auto& val) { val += path[0]; });
~~~

The dimension values are not template parameters, so the usual run-time
checks, with their associated cost, apply here. The library provides
unsafe methods to bypass them when performance is required.

Additionally, the library defines several [codecs](@ref Codecs) offering
input/ouput functionality to formats such as HDF5 or INR, and a few
[filtering algorithms](@ref Filters) defined on the arrays, in particular
recursive ones such as Canny-Deriche or Gamma filters.

~~~
// Blur the 2D image `b`
deriche_blur(b, 2.0);
~~~

## Downloads

Tarballs containing the source code are available in the
[releases](http://emilien.tlapale.com/data/cuiloa) web directory.
Please refer to [INSTALL.md](@ref install) for the installation notes.
Cuiloa is released under the
[Apache License, version 2.0](http://www.apache.org/licenses/LICENSE-2.0).
