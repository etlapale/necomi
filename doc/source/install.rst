Installation
============

Necomi is a C++14 library requiring a modern compiler to build your
programs. The clang_ compiler is currently the recommended option.

Although a header-only library, it is recommended that you build and
run the unit test programs, generate the documentation and install the
headers using the CMake_ script shiped with the source code.

You may also want to install the optional codec libraries, HDF5_ and
libpng_, before building the tests.

To configure, build and run the tests, run the following commands::

  cmake -DCMAKE_CXX_COMPILER=/usr/bin/clang++ . && make check

where we manually specified the clang_ compiler.

To install the headers, use the ``install`` target::

  make install

which will try to copy the headers and the pkgconfig_ file in the
`CMAKE_INSTALL_PREFIX` directory (`/usr/local` by default).

To generate the documentation, use the ``doc`` target.

.. _clang: http://clang.llvm.org
.. _GCC: http://gcc.gnu.org
.. _CMake: http://cmake.org
.. _HDF5: http://www.hdfgroup.org/HDF5/
.. _libpng: http://libpng.org/pub/png/
.. _pkgconfig: http://pkg-config.freedesktop.org
