/* -*-c++-*-
 
 This file is part of the IC reverse engineering tool degate.
 
 Copyright 2008, 2009 by Martin Schobert
 
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

#ifndef __MEDIANFILTER_H__
#define __MEDIANFILTER_H__

namespace degate {

  /**
   * Policy class for image region median calculation.
   */
  template<typename ImageType, typename PixelType>
  struct CalculateImageMedianPolicy {

    /**
     * Calculate the median for an image region.
     */
    static inline PixelType calculate(std::tr1::shared_ptr<ImageType> src,
				      unsigned int x, unsigned int y,
				      unsigned int min_x, 
				      unsigned int max_x,
				      unsigned int min_y,
				      unsigned int max_y,
				      unsigned int threshold) {
      
      assert(min_x < max_x && min_y < max_y);
      assert(min_x < src->get_width());
      assert(max_x < src->get_width());
      assert(min_y < src->get_height());
      assert(max_y < src->get_height());
      
      unsigned int kernel_size = (max_x - min_x) * (max_y - min_y);
      std::vector<PixelType> v(kernel_size);
      
      unsigned int i = 0;
      for(unsigned int _y = min_y; _y < max_y; _y++)
	for(unsigned int _x = min_x; _x < max_x; _x++, i++)
	  v[i] = src->get_pixel(_x, _y);

      return median<PixelType>(v);
    }
  };



  /**
   * Policy class for image region median calculation for RGB(A) images.
   */
  template<typename ImageType>
  struct CalculateImageMedianPolicy<ImageType, rgba_pixel_t> {

    /**
     * Calculate the median for an RGBA image region.
     */
    
    static inline rgba_pixel_t calculate(std::tr1::shared_ptr<ImageType> src,
					 unsigned int x, unsigned int y,
					 unsigned int min_x, 
					 unsigned int max_x,
					 unsigned int min_y,
					 unsigned int max_y,
					 unsigned int threshold) {
      
      assert(min_x < max_x && min_y < max_y);
      assert(min_x < src->get_width());
      assert(max_x < src->get_width());
      assert(min_y < src->get_height());
      assert(max_y < src->get_height());
      
      unsigned int kernel_size = (max_x - min_x) * (max_y - min_y);
      std::vector<unsigned int> 
	v_r(kernel_size), 
	v_g(kernel_size), 
	v_b(kernel_size);
      
      unsigned int i = 0;
      for(unsigned int _y = min_y; _y < max_y; _y++) {
	for(unsigned int _x = min_x; _x < max_x; _x++, i++) {
	  
	  rgba_pixel_t p = src->get_pixel(_x, _y);
	  v_r[i] = MASK_R(p);
	  v_g[i] = MASK_G(p);
	  v_b[i] = MASK_B(p);
	}
      }
      return MERGE_CHANNELS(median<unsigned int>(v_r), 
			    median<unsigned int>(v_g), 
			    median<unsigned int>(v_b), 255);
    }
  };

  /**
   * Filter an image with a median filter.
   */

  template<typename ImageTypeDst, typename ImageTypeSrc>
  void median_filter(std::tr1::shared_ptr<ImageTypeDst> dst,
		     std::tr1::shared_ptr<ImageTypeSrc> src,
		     unsigned int kernel_width = 3) {
  
    filter_image<ImageTypeDst, ImageTypeSrc, 
      CalculateImageMedianPolicy<ImageTypeSrc, typename ImageTypeSrc::pixel_type> >(dst, src, kernel_width);
  }

}
#endif
