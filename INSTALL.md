Cuiloa installation guide	{#install}
=========================

Quick install
-------------

Cuiloa uses [CMake] as build system. You can simply configure the library
and generate the appropriate platform dependent make files with

    $ cmake .

passing it options to customize your build or installation.

Running the unit tests is recommended to make sure that everything is
working on your system, particularly if you are using the git repository.
Use the `check` target to build and run the unit tests:

    $ make check

Finally, you can install the library with the `install` target, assuming you
have the proper rights to write in the chosen install prefix.

    $ sudo make install

Building
--------

Cuiloa is distributed as a templatized library, so no code will be compiled
with a plain `make` call. The `check` target will build and run the unit
tests.

To use cuiloa, you need a compiler with support for C++14. The library
has been succesfully built with GCC 4.9.1 and clang 3.4.2.

Testing
-------

The unit tests are written using Freedesktop’s [CppUnit], which is required
to be found by CMake in order to create the `check` target.

Installing
----------

By default, CMake installs the package content in `/usr/local` on various
systems. You can customize the installation path by setting the standard
variable

    $ cmake -D -DCMAKE_INSTALL_PREFIX=$HOME/usr .

Be aware that some environement variable will then need to be modified
for the package to work properly. In particular if using `pkg-config`, you
may need to set `PKG_CONFIG_PATH` afterwards.

API documentation
-----------------

If [Doxygen] is installed and found by CMake during the configuration, you
can generate the documentation by running

    $ make doc

[CMake]: http://cmake.org
[CppUnit]: http://www.freedesktop.org/wiki/Software/cppunit/
[Doxygen]: http://www.stack.nl/~dimitri/doxygen/
