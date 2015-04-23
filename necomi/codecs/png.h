// necomi/codecs/png.h – PNG codecs based on libpng
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <fstream>

#include <png.h>

namespace necomi {

  class png_exception : public std::runtime_error
  {
  public:
    png_exception(const std::string& what_arg)
      : std::runtime_error(what_arg)
    {
    }
  };

  static inline void libpng_read_callback(png_structp ps,
					  png_bytep data,
					  png_size_t length)
  {
    auto is = static_cast<std::istream*>(png_get_io_ptr(ps));
    is->read((char*) data, length);
    // TODO: add some error handling here
  }

  static inline void libpng_write_callback(png_structp ps,
					   png_bytep data,
					   png_size_t length)
  {
    auto os = static_cast<std::ostream*>(png_get_io_ptr(ps));
    os->write((char*) data, length);
    // TODO: add some error handling here
  }

  static inline void libpng_flush_callback(png_structp ps)
  {
    auto os = static_cast<std::ostream*>(png_get_io_ptr(ps));
    os->flush();
    // TODO: add some error handling here
  }

  inline Array<unsigned char,3> png_load(std::istream&& is)
  {
    // Read the PNG signature
    png_byte sig[8];
    is.read((char*)sig, sizeof(sig));
    // Check it
    if (png_sig_cmp(sig, 0, sizeof(sig)))
      throw png_exception("invalid PNG signature");

    // Create a PNG read structure
    png_structp ps = png_create_read_struct(PNG_LIBPNG_VER_STRING,
					    nullptr, nullptr, nullptr);
    if (! ps)
      throw png_exception("could not create a PNG read structure");
    auto pi = png_create_info_struct(ps);
    if (! pi) {
      png_destroy_read_struct(&ps, 0, 0);
      throw png_exception("could not create a PNG info structure");
    }

    // Error handling
    if (setjmp(png_jmpbuf(ps))) {
      png_destroy_read_struct(&ps, &pi, 0);
      throw png_exception("error while reading the PNG image");
    }

    // Callbacks
    png_set_read_fn(ps, (png_voidp) &is, &necomi::libpng_read_callback);

    // Parse image metadata
    png_set_sig_bytes(ps, sizeof(sig));
    png_read_info(ps, pi);

    auto width = png_get_image_width(ps, pi);
    auto height = png_get_image_height(ps, pi);
    auto depth = png_get_bit_depth(ps, pi);
    auto channels = png_get_channels(ps, pi);
    auto color_type = png_get_color_type(ps, pi);
    auto interlace_type = png_get_interlace_type(ps, pi);

    /*std::cout << "width: " << width << std::endl
	      << "height: " << height << std::endl
	      << "depth: " << (int) depth << std::endl
	      << "channels: " << (int) channels << std::endl
	      << "color type: " << (int) color_type << std::endl
	      << "interlace type: " << (int) interlace_type << std::endl;*/

    switch (color_type) {
    case PNG_COLOR_TYPE_PALETTE:
      png_set_palette_to_rgb(ps);
      break;
    case PNG_COLOR_TYPE_GRAY:
      if (depth < 8)
	png_set_expand_gray_1_2_4_to_8(ps);
      break;
    }

    // TODO: strip 16-bits images

    if (depth != 8 || channels != 3 ||  color_type != PNG_COLOR_TYPE_RGB) {
      png_destroy_read_struct(&ps, &pi, 0);
      throw png_exception("only 3 channels RGB images are supported");
    }
    
    // Read the image
    Array<unsigned char,3> a(height, width, channels);

    // Get the address of each row
    png_bytep rows[height];
    for (unsigned y = 0; y < height; y++)
      rows[y] = (png_bytep) a[y].data();

    // Read the whole image
    png_read_image(ps, rows);

    // Cleanup
    png_destroy_read_struct(&ps, &pi, 0);

    return a;
  }

  inline Array<unsigned char,3> png_load(const std::string& filename)
  {
    return png_load(std::ifstream(filename));
  }

  inline void png_save(const Array<unsigned char,3>& a,
		       std::ostream&& os)
  {
    auto ps = png_create_write_struct(PNG_LIBPNG_VER_STRING,
				      nullptr, nullptr, nullptr);
    if (! ps)
      throw png_exception("could not create a PNG write structure");
    auto pi = png_create_info_struct(ps);
    if (! pi) {
      png_destroy_write_struct(&ps, 0);
      throw png_exception("could not create a PNG info structure");
    }

    // Error handling
    if (setjmp(png_jmpbuf(ps))) {
      png_destroy_write_struct(&ps, &pi);
      throw png_exception("error while writing the PNG image");
    }
    
    // Callbacks
    png_set_write_fn(ps, (png_voidp) &os,
		     &necomi::libpng_write_callback,
		     &necomi::libpng_flush_callback);

    // Write the header
    auto& dims = a.dimensions();
    png_set_IHDR(ps, pi, dims[1], dims[0], 8,
		 PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
		 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_write_info(ps, pi);

    // Write the rows
    for (unsigned int y = 0; y < dims[0]; y++)
      png_write_row(ps, a[y].data());

    // Cleanup
    png_write_end(ps, pi);
    png_destroy_write_struct(&ps, &pi);
  }

  inline void png_save(const Array<unsigned char,3>& a,
		       const std::string& filename)
  {
    png_save(a, std::ofstream(filename));
  }

} // namespace necomi

// Local Variables:
// mode: c++
// End:
