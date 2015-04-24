libPNG
======

.. cpp:function:: Array<unsigned char,3> png_load(const std::string& path)
		  
   Load a 3 channels RGB PNG image as three-dimensional array.
   
.. cpp:function:: Array<unsigned char,3> png_load(std::istream& is)

   Idem but load the PNG from an input stream.

.. cpp:function:: void png_save(const Array<unsigned char,3>& a, const std::string& path)

   Save a three-dimensional array as a color PNG image.

.. cpp:function:: void png_save(const Array<unsigned char,3>& a, std::ostream& os)

   Idem but save into an output stream.
