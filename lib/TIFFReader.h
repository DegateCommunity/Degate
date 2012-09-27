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

#ifndef __TIFFREADER_H__
#define __TIFFREADER_H__

#include <list>
#include <memory>
#include "tiffio.h"

//#include "ImageReaderFactory.h"
#include "StoragePolicies.h"
#include "ImageReaderBase.h"

namespace degate {


  /**
   * The TIFFReader parses tiff images.
   */

  template<class ImageType>
  class TIFFReader : public ImageReaderBase<ImageType> {

  private:

    TIFF* tif;


  public:

    using ImageReaderBase<ImageType>::get_filename;
    using ImageReaderBase<ImageType>::set_width;
    using ImageReaderBase<ImageType>::set_height;
    using ImageReaderBase<ImageType>::get_width;
    using ImageReaderBase<ImageType>::get_height;

    TIFFReader(std::string const& filename) :
      ImageReaderBase<ImageType>(filename),
      tif(NULL) {}

    ~TIFFReader() {
      if(tif != NULL) TIFFClose(tif);
    }

    bool read();

    bool get_image(std::shared_ptr<ImageType>);


  };

  template<class ImageType>
  bool TIFFReader<ImageType>::read() {
    tif = TIFFOpen(get_filename().c_str(), "r");
    if(tif == NULL) return false;

    uint32 w, h;
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);

    set_width(w);
    set_height(h);

    return true;
  }

  template<class ImageType>
  bool TIFFReader<ImageType>::get_image(std::shared_ptr<ImageType> img) {

    size_t npixels = get_width() * get_height();
    uint32 * raster = (uint32*) _TIFFmalloc(npixels * sizeof (uint32));
    if(raster == NULL) return false;

    if(TIFFReadRGBAImage(tif, get_width(), get_height(), raster, 0)) {
      for(unsigned int y = 0; y < get_height(); y++) {
	for(unsigned int x = 0; x < get_width(); x++) {

	  uint32 v = raster[y * get_width() + x];

	  img->set_pixel(x, get_height() - y -1, v);

	}
      }

    }
    _TIFFfree(raster);
    return true;
  }


}

#endif
