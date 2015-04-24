INR
===

The INRia image format used by CImg_.

High level
----------

.. cpp:function:: Array<T,4> inr_load(const char* path)

   Load a INR file into a four-dimensional array.

   There is no information in the INR format to know if the array
   really is three- or four-dimensional.
   
.. cpp:function:: void inr_save(const char* path, const Array<T,N>& a)

   Save an array into an INR file.

   The array must either be three- or a four-dimensional.

Frame by frame
--------------

.. cpp:class:: INRWriter

   Allows writing four-dimensional INR files 3D frame by 3D frame.

   .. cpp:function:: INRWriter<T>(const char* path, bool append=false, bool auto_flush=true)

      Create a new INRWriter for the given file.

      Set ``append`` to ``true`` to append to an existing file. Set
      ``auto_flush`` in case you want the file header to be updated on
      each added frame rather than at the destruction.

   .. cpp:function:: INRWriter<T>& append(const Array<T,3>& frame)

      Append a new frame to the INR file.

.. _Cimg: http://cimg.sf.net
