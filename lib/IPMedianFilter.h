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

#ifndef __IPMEDIANFILTER_H__
#define __IPMEDIANFILTER_H__

#include <string>
#include <ImageProcessorBase.h>
#include <MedianFilter.h>

namespace degate {
  
  /**
   * Processor: Median filter a single channel image.
   */

  template<typename ImageTypeIn, typename ImageTypeOut>
  class IPMedianFilter : public ImageProcessorBase {
    
  private:
    
    unsigned int median_filter_width;

  public:

    /**
     * The constructor.
     */

    IPMedianFilter(unsigned int _median_filter_width = 3) : 
      ImageProcessorBase("IPNormalize", 
			 "Normalize an image.",
			 false,
			 typeid(typename ImageTypeIn::pixel_type), 
			 typeid(typename ImageTypeOut::pixel_type)),
      median_filter_width(_median_filter_width) { }

    
    virtual ImageBase_shptr run(ImageBase_shptr _in) {

      assert(_in != NULL);

      std::tr1::shared_ptr<ImageTypeIn> img_in = 
	std::tr1::dynamic_pointer_cast<ImageTypeIn>(_in);

      std::tr1::shared_ptr<ImageTypeOut> 
	img_out(new ImageTypeOut(_in->get_width(), _in->get_height()));

      assert(img_in != NULL);
      assert(img_out != NULL);
      
      median_filter<ImageTypeOut, ImageTypeIn>(img_out, img_in, median_filter_width);

      return img_out;
    }


  };

}

#endif

