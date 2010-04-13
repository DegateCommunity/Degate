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

#ifndef __IPCONVOLVE_H__
#define __IPCONVOLVE_H__

#include <string>
#include <ImageProcessorBase.h>
#include <FilterKernel.h>

namespace degate {
  
  /**
   * Processor: Convolve an image.
   */

  template<typename ImageTypeIn, typename ImageTypeOut>
  class IPConvolve : public ImageProcessorBase {
    
  private:
    FilterKernel_shptr kernel;

  public:

    /**
     * The constructor.
     */

    IPConvolve(FilterKernel_shptr _kernel) : 
      ImageProcessorBase("IPConvolve", 
			 "Convolve an image.",
			 false,
			 typeid(typename ImageTypeIn::pixel_type), 
			 typeid(typename ImageTypeOut::pixel_type)),
	kernel(_kernel) { }

    /**
     * The destructor.
     */

    virtual ~IPConvolve() {}

    
    virtual ImageBase_shptr run(ImageBase_shptr _in) {

      assert(_in != NULL);

      std::tr1::shared_ptr<ImageTypeIn> img_in = 
	std::tr1::dynamic_pointer_cast<ImageTypeIn>(_in);

      std::tr1::shared_ptr<ImageTypeOut> 
	img_out(new ImageTypeOut(_in->get_width(), _in->get_height()));

      assert(img_in != NULL);
      assert(img_out != NULL);
      
      convolve<ImageTypeOut, ImageTypeIn>(img_out, img_in, kernel);

      return img_out;
    }


  };

}

#endif

