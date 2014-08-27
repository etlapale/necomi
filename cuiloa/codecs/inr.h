/*
 * Copyright © 2014     University of California, Irvine
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <stdexcept>

#include "../base/array.h"

/**
 * \file inr.h INR images utilities.
 * \ingroup Codecs
 * 
 * This file defines codecs for INR images.
 * INR images always contains four dimensions, though some can be
 * equals to 1. The codecs define a function to get an array from
 * an INR input file, cuiloa::inr_load(const char*) and a function to save
 * an entire array to an INR file,
 * cuiloa::inr_save(const cuiloa::Array<T>&, const char*)
 * The array to be saved should not contains more than four dimensions.
 * In addition an INR image can be saved frame by frame using an 
 * instance of cuiloa::INRWriter thus saving memory having only one frame
 * at a time loaded.
 */

namespace cuiloa
{
  /**
   * Load an INR image into a multi-dimensional array.
   * The INR image is loaded from the given file.
   * The resulting cuiloa::Array will always have four dimensions
   * although some of them can be set to 1.
   * The template parameter determines the type of the resulting
   * array so array type should already be known.
   *
   * \param path File from where to load the array.
   * \ingroup Codecs
   */
  template <typename T>
    Array<T,4> inr_load(const char* path)
    {
      int fp;
      struct stat stbuf;
      char* ptr;
      char* pixels;
      char* str;
      
      fp = open(path, O_RDONLY);
      if (fp == -1)
	throw std::runtime_error(std::string("Could not fopen: ") + strerror(errno));
      if (fstat(fp, &stbuf) != 0)
	throw std::runtime_error("Could not fstat()");
      ptr = (char*) mmap(NULL, stbuf.st_size, PROT_READ, MAP_PRIVATE, fp, 0);
      if (ptr == NULL)
	throw std::runtime_error("Could not mmap()");
      pixels = ptr + 256;
      
      // Check the magic
      if (strncmp (ptr, "#INRIMAGE-4#{", 13) != 0)
	throw std::runtime_error("Not an INR file");
      
      // Read the header infos
      int width = 0, height = 0, depth = 0, channels = 0;
      int pixsize = 0;
      bool signed_ = true;
      bool fixed = false;
      str = ptr + 14;
      while (str - ptr < 257 && strncmp (str, "##}", 3) != 0 && *str != '\n')
	{
	  char *eqsym = strchr (str, '=');
	  char *next_line = strchr (str, '\n');
	  
	  if (eqsym == NULL || next_line == NULL)
	    throw std::runtime_error("Corrupted INR header");
	  if (eqsym - str == 4 && strncmp ("XDIM", str, 4) == 0)
	    width = atol(eqsym + 1);
	  else if (eqsym - str == 4 && strncmp ("YDIM", str, 4) == 0)
	    height = atol(eqsym + 1);
	  else if (eqsym - str == 4 && strncmp ("ZDIM", str, 4) == 0)
	    depth = atol(eqsym + 1);
	  else if (eqsym - str ==  4 && strncmp ("VDIM", str, 4) == 0)
	    channels = atol(eqsym + 1);
	  else if (eqsym - str == 7 && strncmp ("PIXSIZE", str, 7) == 0)
	    pixsize = atoi(eqsym + 1) / 8;
	  else if (eqsym - str == 4 && strncmp ("TYPE", str, 4) == 0)
	    {
	      if (strncmp (eqsym + 1, "unsigned ", 9) == 0)
		{
		  signed_ = false;
		  eqsym += 9;
		}
	      
	      if (strncmp(eqsym + 1, "float", 5) == 0
		  || strncmp(eqsym + 1, "double", 6) == 0)
		fixed = false;
	      else if (strncmp(eqsym + 1, "fixed", 5) == 0
		       || strncmp(eqsym + 1, "int", 3) == 0)
		fixed = true;
	    }
	  
	  str = next_line + 1;
	}
      if (width == 0 || height == 0 || depth == 0 || channels == 0 || pixsize == 0)
	throw std::runtime_error("Missing information in the header");
      
      // Create the array
      //int dimz[4] = (int[4]) {depth, channels, height, width};
      int dimz[4];
      dimz[0] = depth; dimz[1] = channels; dimz[2] = height; dimz[3] = width;
      Array<T,4> a(dimz);
      for (int t = 0; t < depth; t++)
	for (int v = 0; v < channels; v++)
	  for (int y = 0; y < height; y++)
	    for (int x = 0; x < width; x++)
	      {
		T val;
		void *pixel = pixels + pixsize * (v + channels * (x + width * (y + height * t)));
		if (fixed)
		  {
		    if (sizeof(char) == pixsize)
		      val = signed_ ? (double) *((char *) pixel) : (double) *((unsigned char *) pixel);
		    else if (sizeof (int) == pixsize)
		      val = signed_ ? (double) *((int *) pixel) : (double) *((unsigned int *) pixel);
		    else if (sizeof (short) == pixsize)
		      val = signed_ ? (double) *((short *) pixel) : (double) *((unsigned short *) pixel);
		    else if (sizeof (long) == pixsize)
		      val = signed_ ? (double) *((long *) pixel) : (double) *((unsigned long *) pixel);
		    else
		      throw std::runtime_error("Unknown pixel type");
		  }
		else
		  {
		    if (sizeof (double) == pixsize)
		      val = *((double *) pixel);
		    else if (sizeof (float) == pixsize)
		      val = *((float *) pixel);
		    else
		      throw std::runtime_error("Unknown pixel type");
		  }
		//a[t][v][y][x] = val;
		a(t,v,y,x) = val;
	      }
      
      munmap(ptr, stbuf.st_size);
      close(fp);      

      return a;
    }
  
#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

  static inline bool
  big_endian()
  {
    const int x = 1;
    return ! (((unsigned char *) &x)[0]);
  }
  
  void
  inr_write_header(const char* type, const int* dims,
		   int ndim, std::ofstream& of)
  {
    char header[256];
    int width = MAX(1, dims[3]);
    int height = ndim < 2 ? 1 : MAX(1, dims[2]);
    int channels = ndim < 3 ? 1 : MAX(1, dims[1]);
    int depth = ndim < 4 ? 1 : MAX(1, dims[0]);
    int ans = sprintf(header,
		      "#INRIMAGE-4#{\nXDIM=%d\nYDIM=%d\nZDIM=%d\nVDIM=%d\n",
		      width, height, depth, channels);
    ans += sprintf(header + ans, "TYPE=%s\nCPU=%s\n",
		   type, big_endian() ? "sun" : "decm");
    memset(header + ans, '\n', 252 - ans);
    memcpy(header + 252, "##}\n", 4);
    
    of.write(header, 256);
  }

  template <typename T>
    const char*
    inr_type(const T* val __attribute__((unused)))
    {
      throw std::runtime_error("Invalid INR data type");
    }

  template <>
    const char*
    inr_type(const double* val __attribute__((unused)))
    {
      return "float\nPIXSIZE=64 bits";
    }

  template <>
    const char*
    inr_type(const float* val __attribute__((unused)))
    {
      return "float\nPIXSIZE=32 bits";
    }

  template <>
    const char*
    inr_type(const unsigned char* val __attribute__((unused)))
    {
      return "unsigned fixed\nPIXSIZE=8 bits\nSCALE=2**0";
    }

  template <>
    const char*
    inr_type(const char* val __attribute__((unused)))
    {
      return "fixed\nPIXSIZE=8 bits\nSCALE=2**0";
    }

  template <>
    const char*
    inr_type(const int* val __attribute__((unused)))
    {
      return "fixed\nPIXSIZE=32 bits\nSCALE=2**0";
    }

  template <>
    const char*
    inr_type(const unsigned int* val __attribute__((unused)))
    {
      return "unsigned fixed\nPIXSIZE=32 bits\nSCALE=2**0";
    }

/**
 * Save a complete Array as an INR sequence.
 * The array to be saved must have a dimension count smaller than four.
 *
 * \param a The array to be saved.
 * \param path Path to the output INR file.
 * \ingroup Codecs
 */
template <typename T,ArrayIndex N>
void
inr_save(const cuiloa::Array<T,N>& a, const char* path)
{
  static_assert(N == 3 || N == 4,
                "Only 3D and 4D arrays can be stored as INR");

  std::ofstream of(path, std::ios::out|std::ios::binary);
  if (of.fail())
    throw std::runtime_error("Could not open output file");
  inr_write_header(inr_type((const T*) NULL), a.dimensions(), N, of);

  // TODO Write in a single block for contiguous arrays
  a.map([&of](auto&path, auto&val) {
      (void) path;
      of.write(&val, sizeof(T));
    });
}

  /**
   * Write an INR sequence frame by frame.
   * \ingroup Codecs
   */
  template <typename T>
    class INRWriter
    {
    public:
      /**
       * Open a new INRWriter for the given file.
       * \param append \c true if you want to append to an already file.
       * \param auto_flush \c true to write the header for every added frame.
       */
      INRWriter(const char* path, bool append=false, bool auto_flush=true);
    
      /**
       * Close the INRWriter writing necessary remaining data.
       */
      ~INRWriter();
      
      /**
       * Append an image to the INR stream.
       */
      INRWriter<T>&
      append(const cuiloa::Array<T,3>& frame);
      
      /**
       * Append an image to the INR stream.
       */
      INRWriter<T>&
      operator<<(const cuiloa::Array<T,3>& frame);
      
      /**
       * Update the INR sequence header.
       */
      void
      write_header();

    protected:
      char* m_path;
      std::ofstream m_of;
      int m_width, m_height, m_channels, m_depth;
      bool m_auto_flush;
      bool m_first_frame;
    };

  template <typename T>
    INRWriter<T>&
    INRWriter<T>::append(const cuiloa::Array<T,3>& frame)
    {
      const int* dims = frame.dimensions();
        
      // Check if it is the first frame
      if (m_first_frame)
	{
	  m_width = dims[2]; //frame.dimx();
	  m_height = dims[1]; //frame.dimy();
	  m_channels = dims[0]; //frame.dimz();
	  m_depth = 0;
	  m_first_frame = false;
	}
      
      // Check image properties
      else if (m_width != dims[2] //frame.dimx()
	       || m_height != dims[1] //frame.dimy()
	       || m_channels != dims[0]) //frame.dimz())
	throw std::runtime_error("Cannot append different dimensions frames");
    
      // TODO Write in a single block for contiguous arrays
      frame.map([this](auto&path, auto&val) {
          (void) path;
          m_of.write(&val, sizeof(T));
        });

      m_depth++;
      
      if (m_auto_flush)
	this->write_header();
      
      return *this;
    }

  template <typename T>
    INRWriter<T>&
    INRWriter<T>::operator<<(const cuiloa::Array<T,3>& frame)
    {
      return this->append(frame);
    }

  template <typename T>
    INRWriter<T>::INRWriter(const char* filename, bool append, bool autoflush)
      : m_of(filename, std::ios::out|std::ios::binary),
	m_auto_flush(autoflush), m_first_frame(true)
    {
      m_path = strdup(filename);
      char header[256];
      
      // Open an already existing file in append mode
      if (append && access(filename, R_OK) == 0)
	{
	  FILE* f = fopen(filename, "rb");
	  fread(header, 1, 256, f);
	  fclose(f);
	  
	  if (strncmp(header, "#INRIMAGE-4#{\n", 14) != 0)
	    throw std::runtime_error("Not an INR image");
	  
	  //m_fp = fopen(m_filename, "wb");
	  //if (m_fp != NULL)
	  //fseek(fp, 0, SEEK_END);
	  m_of.seekp(0, std::ios::end);
	}

      else
	{
	  //m_fp = fopen(m_filename, "wb");
	  //if (m_fp != NULL)
	  //fwrite(header, 1, 156, m_fp);
	  //m_of.write(header, 256);
	  m_of.seekp(256, std::ios::beg);
	}

      //if (ferror(m_fp))
      if (m_of.fail())
	throw std::runtime_error("Error on opening the INR file");
    }

  template <typename T>
    INRWriter<T>::~INRWriter()
    {
      if (m_of.is_open())
	{
	  this->write_header();
	  //fclose(m_fp);
	  m_of.close();
	}
      
      free(m_path);
    }

  template <typename T>
    void
    INRWriter<T>::write_header()
    { 
      int pos = m_of.tellp();
      m_of.seekp(0, std::ios::beg);
      
      int dims[4] = {m_depth, m_channels, m_height, m_width};
      inr_write_header(inr_type((const T*) NULL), dims, 4, m_of);
      
      m_of.seekp(pos, std::ios::beg);
      if (m_auto_flush)
	m_of.flush();
    }
}
