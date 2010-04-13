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

#ifndef __IMAGEHELPER_H__
#define __IMAGEHELPER_H__

#include <ImageReaderFactory.h>
#include <ImageReaderBase.h>
#include <ImageWriterBase.h>
#include <TIFFWriter.h>
#include <PixelPolicies.h>
#include <StoragePolicies.h>
#include <Image.h>

namespace degate {


  /**
   * Load an image in a common image format, such as tiff.
   */

  template<typename ImageType>
  std::tr1::shared_ptr<ImageType> load_image(std::string const& path) 
    throw(InvalidPathException, DegateRuntimeException) {
    if(!file_exists(path)) {
      boost::format fmter("Error in load_image(): file %1% does not exist.");
      fmter % path;
      throw InvalidPathException(fmter.str());
    }
    else {
      boost::format fmter("Error in load_image(): The image file %1% cannot be loaded.");
      fmter % path;
      
      // get a reader
      ImageReaderFactory<ImageType> ir_factory;
      std::tr1::shared_ptr<ImageReaderBase<ImageType> > reader = ir_factory.get_reader(path);

      if(reader->read() == false) {
	throw DegateRuntimeException(fmter.str());
      }
      else {

	debug(TM, "reading image file: %s", path.c_str());

	// create an empty image
	std::tr1::shared_ptr<ImageType> img(new ImageType(reader->get_width(), 
							  reader->get_height()));
	if(reader->get_image(img) == true) 
	  return img;
	else
	  throw DegateRuntimeException(fmter.str());
      }
    }
  }

  /**
   * Load an image in a common image format, such as tiff, into an existing degate image.
   */

  template<typename ImageType>
  void load_image(std::string const& path, std::tr1::shared_ptr<ImageType> img) 
    throw(InvalidPathException, DegateRuntimeException) {
	
    std::tr1::shared_ptr<ImageType> i = load_image<ImageType>(path);
    copy_image<ImageType, ImageType>(img, i);
  }

  
  /**
   * Store an image in a common file format.
   * Only the tiff file format is supported.
   * @todo We should use a factory for writer objects.
   */

  template<typename ImageType>
  void save_image(std::string const& path, std::tr1::shared_ptr<ImageType> img) 
    throw(InvalidPathException, DegateRuntimeException) {
    
    TIFFWriter<ImageType> tiff_writer(img->get_width(), 
				      img->get_height(), path);
    if(tiff_writer.write_image(img) != true) {
      boost::format fmter("Error in save_image(): Canot write file %1%.");
      fmter % path;
      throw DegateRuntimeException(fmter.str());
    }
    
  }

  /**
   * Normalize a single channel image and store it in a common file format.
   * Only the tiff file format is supported.
   */

  template<typename ImageType>
  void save_normalized_image(std::string const& path, std::tr1::shared_ptr<ImageType> img) {
    
    std::tr1::shared_ptr<ImageType> normalized_img(new ImageType(img->get_width(), 
								 img->get_height()));

    normalize<ImageType, ImageType>(normalized_img, img, 0, 255);

    save_image<ImageType>(path, normalized_img);
  }

}

#endif
