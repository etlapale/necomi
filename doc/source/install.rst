Installation
============

Necomi is a C++14 library requiring a modern compiler to build your
programs. The clang_ compiler is currently the recommended option.
All other dependencies are optional.

Necomi is a header-only library, but we provide a CMake_ script to
facilitate unit testing and installation.

.. _clang: http://clang.llvm.org
.. _GCC: http://gcc.gnu.org

Testing
-------

The CMake_ script will try to auto-detect the libraries available on
your system, and disable some tests when the dependencies are not
found. You can ensure that the tests will cover the dependencies by
installing them before running ``cmake`` and, if necessary, give their
installation path to the configuration script.

Currently the two optional dependencies are the HDF5_ and the libpng_
libraries.

To configure, build and run the tests, run the following commands::

  cmake -DCMAKE_CXX_COMPILER=/usr/bin/clang++ .
  make check

where we manually specified the clang_ compiler.

If they are not present in the source directories, the configuration
script will try to fetch some test dependencies from the web: Catch_,
and some PNG images if libpng_ has been found.

.. _Catch: https://github.com/philsquared/Catch
.. _CMake: http://cmake.org
.. _HDF5: http://www.hdfgroup.org/HDF5/
.. _libpng: http://libpng.org/pub/png/

Installation
------------

To install the headers, use the ``install`` target::

  make install

which will try to copy the headers and the pkgconfig_ file in the
``CMAKE_INSTALL_PREFIX`` directory (``/usr/local`` by default).

.. _pkgconfig: http://pkg-config.freedesktop.org

Documentation
-------------

To regenerate the HTML documentation, go in the ``doc/`` directory and
use the ``html`` target::

  cd doc
  make html

The entry point of the documentation will be in
``doc/build/html/index.html``.  You will need to have Sphinx_ available
on your system to generate the documentation from sources.

The fonts Awesome_ and Fira_ should be present in
``doc/source/_static/fonts/`` before running ``make`` to ensure
correct typography.

.. _Awesome: http://fortawesome.github.io/Font-Awesome/
.. _Fira: https://www.mozilla.org/en-US/styleguide/products/firefox-os/typeface/
.. _Sphinx: http://sphinx-doc.org

Usage
-----

If you used the ``make install`` command to copy the header files at a
compiler-accessible location, you can start using Necomi without
further configuration:

.. code:: c++

  #include <cuiloa/cuiloa.h>

If did not install the headers, or copied them at a non-standard
location, you may have to specify it to your compiler::

  $ clang++ -I/path/to/cuiloa […]

When you want to enable the optional dependencies, you must specify
them as compiler definitions::

  $ clang++ -DHAVE_HDF5 -DHAVE_LIBPNG […]
