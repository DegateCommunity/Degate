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

#ifndef __IPCOPY_H__
#define __IPCOPY_H__

#include <string>
#include <ImageProcessorBase.h>

namespace degate {

  /**
   * Processor: Copy an image with auto conversion.
   */

  template<typename ImageTypeIn, typename ImageTypeOut>
  class IPCopy : public ImageProcessorBase {

  private:

    unsigned int min_x, max_x, min_y, max_y;
    bool work_on_region;

  public:

    /**
     * The constructor for processing the whole image.
     */

    IPCopy() :
      ImageProcessorBase("IPCopy",
			 "Copy an image with pixel type auto conversion",
			 false,
			 typeid(typename ImageTypeIn::pixel_type),
			 typeid(typename ImageTypeOut::pixel_type)),
      work_on_region(false) { }

    /**
     * The constructor for working on an image region.
     */

    IPCopy(unsigned int _min_x, unsigned int _max_x, unsigned int _min_y, unsigned int _max_y) :
      ImageProcessorBase("IPCopy",
			 "Copy an image with pixel type auto conversion",
			 false,
			 typeid(typename ImageTypeIn::pixel_type),
			 typeid(typename ImageTypeOut::pixel_type)),
      min_x(_min_x),
      max_x(_max_x),
      min_y(_min_y),
      max_y(_max_y),
      work_on_region(true) {
    }

    /**
     * The destructor.
     */

    virtual ~IPCopy() {}



    virtual ImageBase_shptr run(ImageBase_shptr _in) {

      assert(_in != NULL);

      std::tr1::shared_ptr<ImageTypeIn> img_in =
	std::tr1::dynamic_pointer_cast<ImageTypeIn>(_in);

      std::tr1::shared_ptr<ImageTypeOut> img_out
	(work_on_region ?
	 new ImageTypeOut(max_x - min_x, max_y - min_y) :
	 new ImageTypeOut(_in->get_width(), _in->get_height()));

      assert(img_in != NULL);
      assert(img_out != NULL);

      std::cout << "Copy image." << std::endl;

      if(work_on_region == true) {
	extract_partial_image<ImageTypeOut,
	  ImageTypeIn>(img_out, img_in,
		       min_x, max_x,
		       min_y, max_y);
      }
      else {
	copy_image<ImageTypeOut, ImageTypeIn>(img_out, img_in);
      }

      return img_out;
    }


  };

}

#endif

