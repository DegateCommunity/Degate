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

#ifndef __IPIMAGEWRITER_H__
#define __IPIMAGEWRITER_H__

#include <string>
#include <ImageProcessorBase.h>
#include <ImageHelper.h>

namespace degate {
  
  /**
   * Processor: Write an image to file.
   */

  template<typename ImageType>
  class IPImageWriter : public ImageProcessorBase {
    
  private:
    std::string filename;

  public:

    /**
     * The constructor.
     */

    IPImageWriter(std::string _filename) : 
      ImageProcessorBase("IPImageWriter", 
			 "Write an image.",
			 false,
			 typeid(typename ImageType::pixel_type), 
			 typeid(typename ImageType::pixel_type)),
      filename(_filename) { }

    /**
     * The destructor.
     */

    virtual ~IPImageWriter() {}

    virtual ImageBase_shptr run(ImageBase_shptr _in) {

      assert(_in != NULL);

      std::tr1::shared_ptr<ImageType> img_in = 
	std::tr1::dynamic_pointer_cast<ImageType>(_in);


      std::tr1::shared_ptr<ImageType> 
	img_out(new ImageType(_in->get_width(), _in->get_height()));

      assert(img_in != NULL);
      assert(img_out != NULL);

      std::cout << "writing file: " << filename << std::endl;

      normalize<ImageType, ImageType>(img_out, img_in, 0, 255);

      save_image<ImageType>(filename, img_out);

      return img_in;
    }


  };

}

#endif

