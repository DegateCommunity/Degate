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

#ifndef __TIFFWRITER_H__
#define __TIFFWRITER_H__

#include <list>
#include <memory>
#include "tiffio.h"

#include <globals.h>
#include <degate_exceptions.h>
#include <PixelPolicies.h>
#include <StoragePolicies.h>
#include <ImageWriterBase.h>
#include <FileSystem.h>
#include <Image.h>

#include <errno.h>

namespace degate {


  /**
   * The TIFFWriter parses tiff images.
   */

  template<class ImageType>
  class TIFFWriter : public ImageWriterBase<ImageType> {

  public:

    using ImageWriterBase<ImageType>::get_filename;
    using ImageWriterBase<ImageType>::get_width;
    using ImageWriterBase<ImageType>::get_height;


    TIFFWriter(unsigned int width, unsigned int height,
	       std::string const& filename) :
      ImageWriterBase<ImageType>(width, height, filename) {}

    virtual ~TIFFWriter() { }

    /**
     * exception FileSystemException
     */
    bool write_image(std::shared_ptr<ImageType> img);
  };


  template<class ImageType>
  bool TIFFWriter<ImageType>::write_image(std::shared_ptr<ImageType> img) {

    TIFF * tif = TIFFOpen(get_filename().c_str(), "w");
    if(tif == NULL) {
      throw FileSystemException(strerror(errno));
    }

    size_t npixels = get_width() * get_height();

    char * raster = (char*) _TIFFmalloc(npixels * 3);
    if(raster == NULL) return false;

    for(unsigned int y = 0; y < get_height(); y++) {
      for(unsigned int x = 0; x < get_width(); x++) {

	rgba_pixel_t p =
	  img->template get_pixel_as<rgba_pixel_t>(x, y);

	raster[3*(y * get_width() + x)] = MASK_R(p);
	raster[3*(y * get_width() + x)+1] = MASK_G(p);
	raster[3*(y * get_width() + x)+2] = MASK_B(p);
      }
    }


    // Write the tiff tags to the file
    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, get_width());
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, get_height());
    TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);

    bool ret = true;

    // Actually write the image
    if(TIFFWriteEncodedStrip(tif, 0, raster, npixels * 3) == 0) {
      ret = false;
    }

    if(tif != NULL) TIFFClose(tif);
    _TIFFfree(raster);

    return ret;
  }


}

#endif
