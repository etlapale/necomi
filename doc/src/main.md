# Necomi – A multi-dimensional array library	{#mainpage}

The core class of the library is necomi::Array, which defines a
multi-dimensional array with element type and dimensionality as template
parameters. Several general purpose functions are used to access
to the array elements, to modify them, and to create views on subsets
of the arrays sharing their elements.

~~~
// Create a 3D array of `int` elements
necomi::Array<int,3> a(4,5,6);

// Display the second element
std::cout << a(0,0,1) << std::endl;

// 2D view of type necomi::Array<int,2>
// and size 5×6, sharing the same elements
auto b = a[1];
~~~

More information is available of the project webpage at
http://emilien.tlapale.com/code/necomi.


## Download

Tarballs containing the source code are available in the
[releases](http://emilien.tlapale.com/data/necomi) web directory.
Please refer to [INSTALL.md](@ref install) for the installation notes.
Necomi is released under the
[Simplified BSD License](http://emilien.tlapale.com/data/licenses/BSD).
