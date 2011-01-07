/* -*-c++-*-

 This file is part of the IC reverse engineering tool degate.

 Copyright 2008, 2009, 2010 by Martin Schobert

 Degate is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.

 Degate is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with degate. If not, see <http://www.gnu.org/licenses/>.

 */

#ifndef __JPEGREADER_H__
#define __JPEGREADER_H__

#include <list>
#include <tr1/memory>
#include <jpeglib.h>

#include "StoragePolicies.h"
#include "ImageReaderBase.h"

namespace degate {


  /**
   * The JPEGReader parses jpeg images.
   */

  template<class ImageType>
  class JPEGReader : public ImageReaderBase<ImageType> {

  private:

    unsigned char * image_buffer;
    int depth;

  public:

    using ImageReaderBase<ImageType>::get_filename;
    using ImageReaderBase<ImageType>::set_width;
    using ImageReaderBase<ImageType>::set_height;
    using ImageReaderBase<ImageType>::get_width;
    using ImageReaderBase<ImageType>::get_height;

    JPEGReader(std::string const& filename) :
      ImageReaderBase<ImageType>(filename),
      image_buffer(NULL) {}

    ~JPEGReader() {
      if(image_buffer != NULL) free(image_buffer);
    }

    bool read();

    bool get_image(std::tr1::shared_ptr<ImageType>);


  };

  template<class ImageType>
  bool JPEGReader<ImageType>::read() {

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE * infile;     /* source file */
    depth = 0;

    JSAMPARRAY buffer; /* Output row buffer */
    int row_stride;    /* physical row width in output buffer */
    unsigned int p = 0;

    if ((infile = fopen(get_filename().c_str(), "rb")) == NULL) {
      debug(TM, "can't open %s\n", get_filename().c_str());
      return false;
    }

    cinfo.err = jpeg_std_error(&jerr);

    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);

    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    set_width(cinfo.output_width);
    set_height(cinfo.output_height);
    depth = cinfo.num_components;

    debug(TM, "Reading image with size: %d x %d", get_width(), get_height());

    image_buffer = (unsigned char *)malloc(depth * get_width() * get_height());

    if(image_buffer != NULL) {

      row_stride = cinfo.output_width * cinfo.output_components;
      buffer = (*cinfo.mem->alloc_sarray)
	((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

      printf("Row stride is %d\n",row_stride);
      while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, buffer, 1);

        memcpy(&image_buffer[p],buffer[0],row_stride);
        p+=row_stride;
        //        put_scanline_someplace(buffer[0], row_stride);
      }

      printf("Image read\n");
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    return true;
  }

  template<class ImageType>
  bool JPEGReader<ImageType>::get_image(std::tr1::shared_ptr<ImageType> img) {

    if(img == NULL) return false;

    for(unsigned int y = 0; y < get_height(); y++) {
      for(unsigned int x = 0; x < get_width(); x++) {

	uint8_t v1, v2, v3;
	if(depth == 1) {
	  v1 = v2 = v3 = image_buffer[(y * get_width() + x)];
	}
	else if(depth == 3) {
	  v1 = image_buffer[depth * (y * get_width() + x)];
	  v2 = image_buffer[depth * (y * get_width() + x) + 1];
	  v3 = image_buffer[depth * (y * get_width() + x) + 2];
	}
	else throw std::runtime_error("Unexpected number of channels in JPG file.");

	img->set_pixel(x, y, MERGE_CHANNELS(v1, v2, v3, 0xff));

      }
    }

    return true;
  }


}

#endif
