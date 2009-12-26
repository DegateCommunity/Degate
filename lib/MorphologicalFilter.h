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

#ifndef __MORPHOLOGICALFILTER_H__
#define __MORPHOLOGICALFILTER_H__

namespace degate {

  /**
   * Policy class for image erosion.
   * This policy class can be used for any single channel image. Pixel values
   * evaluate to 0 if they are <= 0 and evaluated to 1 if they are > 0.
   */
  template<typename ImageType, typename PixelType>
  struct ErodeImagePolicy {

    static inline PixelType calculate(std::tr1::shared_ptr<ImageType> src,
				      unsigned int x, unsigned int y,
				      unsigned int min_x,
				      unsigned int max_x,
				      unsigned int min_y,
				      unsigned int max_y,
				      unsigned int erosion_threshold) {
      
      assert(min_x < max_x && min_y < max_y);
      assert(min_x < src->get_width());
      assert(max_x < src->get_width());
      assert(min_y < src->get_height());
      assert(max_y < src->get_height());
      
      unsigned int i = 0;
      
      for(unsigned int _y = min_y; _y < max_y; _y++)
	for(unsigned int _x = min_x; _x < max_x; _x++)
	  if(src->get_pixel(_x, _y) > 0) i++;

      return i <= erosion_threshold ? 0 : src->get_pixel(x, y);
    }
  };


  /**
   * Filter an image with an erosion filter.
   */

  template<typename ImageTypeDst, typename ImageTypeSrc>
  void erode_image(std::tr1::shared_ptr<ImageTypeDst> dst,
		   std::tr1::shared_ptr<ImageTypeSrc> src,
		   unsigned int kernel_width = 3,
		   unsigned int erosion_threshold = 3) {
  
    filter_image<ImageTypeDst, ImageTypeSrc,
      ErodeImagePolicy<ImageTypeSrc, typename ImageTypeSrc::pixel_type> >
      (dst, src, kernel_width, erosion_threshold);
  }


  /**
   * Policy class for image dilation.
   * This policy class can be used for any single channel image. Pixel values
   * evaluate to 0 if they are <= 0 and evaluated to 1 if they are > 0.
   */
  template<typename ImageType, typename PixelType>
  struct DilateImagePolicy {

    static inline PixelType calculate(std::tr1::shared_ptr<ImageType> src,
				      unsigned int x, unsigned int y,
				      unsigned int min_x,
				      unsigned int max_x,
				      unsigned int min_y,
				      unsigned int max_y,
				      unsigned int dilation_threshold) {
      
      assert(min_x < max_x && min_y < max_y);
      assert(min_x < src->get_width());
      assert(max_x < src->get_width());
      assert(min_y < src->get_height());
      assert(max_y < src->get_height());
      
      unsigned int i = 0;
      
      for(unsigned int _y = min_y; _y < max_y; _y++)
	for(unsigned int _x = min_x; _x < max_x; _x++)
	  if(src->get_pixel(_x, _y) > 0) i++;

      return i >= dilation_threshold ? 1 : src->get_pixel(x, y);
    }
  };


  /**
   * Filter an image with an erosion filter.
   */

  template<typename ImageTypeDst, typename ImageTypeSrc>
  void dilate_image(std::tr1::shared_ptr<ImageTypeDst> dst,
		    std::tr1::shared_ptr<ImageTypeSrc> src,
		    unsigned int kernel_width = 3,
		    unsigned int dilation_threshold = 3) {
  
    filter_image<ImageTypeDst, ImageTypeSrc,
      DilateImagePolicy<ImageTypeSrc, typename ImageTypeSrc::pixel_type> >
      (dst, src, kernel_width, dilation_threshold);
  }


  /**
   * Morphological open.
   */
  template<typename ImageTypeDst, typename ImageTypeSrc>
  void morphological_open(std::tr1::shared_ptr<ImageTypeDst> dst,
			  std::tr1::shared_ptr<ImageTypeSrc> src,
			  unsigned int kernel_width = 3,
			  unsigned int threshold = 3) {
    
    filter_image<ImageTypeDst, ImageTypeSrc,  
      ErodeImagePolicy<ImageTypeSrc, typename ImageTypeSrc::pixel_type> >(dst, src, kernel_width, threshold);

    filter_image<ImageTypeDst, ImageTypeSrc,  
      DilateImagePolicy<ImageTypeSrc, typename ImageTypeSrc::pixel_type> >(dst, src, kernel_width, threshold);
  }


  /**
   * Morphological close.
   */
  template<typename ImageTypeDst, typename ImageTypeSrc>
  void morphological_close(std::tr1::shared_ptr<ImageTypeDst> dst,
			   std::tr1::shared_ptr<ImageTypeSrc> src,
			   unsigned int kernel_width = 3,
			   unsigned int threshold = 3) {
    
    filter_image<ImageTypeDst, ImageTypeSrc,  
      DilateImagePolicy<ImageTypeSrc, typename ImageTypeSrc::pixel_type> >(dst, src, kernel_width, threshold);

    filter_image<ImageTypeDst, ImageTypeSrc,  
      ErodeImagePolicy<ImageTypeSrc, typename ImageTypeSrc::pixel_type> >(dst, src, kernel_width, threshold);

  }


  /**
   * Helper function for the Zhang-Suen-Thinning.
   * @param img The image.
   * @param condition_switch If true, the first condition set is checked. If false,
   *   the second condition set is checked.
   */
  template<typename ImageType>
  bool zhang_suen_thinning_iteration(std::tr1::shared_ptr<ImageType> img, 
				     bool condition_switch) {
    assert_is_single_channel_image<ImageType>();

    bool running = false;
    unsigned int x, y;

    for(y = 1; y < img->get_height() - 1; y++) {
      for(x = 1; x < img->get_width() - 1; x++) {
	unsigned int 
	  p1 = img->get_pixel(x, y) > 0 ? 1 : 0;

	if(p1 > 0) {
	  unsigned int
	    p2 = img->get_pixel(x, y-1) > 0 ? 1 : 0,
	    p3 = img->get_pixel(x+1, y-1) > 0 ? 1 : 0,
	    p4 = img->get_pixel(x+1, y) > 0 ? 1 : 0,
	    p5 = img->get_pixel(x+1, y+1) > 0 ? 1 : 0,
	    p6 = img->get_pixel(x, y+1) > 0 ? 1 : 0,
	    p7 = img->get_pixel(x-1, y+1) > 0 ? 1 : 0,
	    p8 = img->get_pixel(x-1, y) > 0 ? 1 : 0,
	    p9 = img->get_pixel(x-1, y-1)> 0 ? 1 : 0;
	  
	  unsigned int connectivity =
	    (p2 == 0 && p3 == 1 ? 1 : 0) +
	    (p3 == 0 && p4 == 1 ? 1 : 0) +
	    (p4 == 0 && p5 == 1 ? 1 : 0) +
	    (p5 == 0 && p6 == 1 ? 1 : 0) +
	    (p6 == 0 && p7 == 1 ? 1 : 0) +
	    (p7 == 0 && p8 == 1 ? 1 : 0) +
	    (p8 == 0 && p9 == 1 ? 1 : 0) +
	    (p9 == 0 && p2 == 1 ? 1 : 0);
	  
	  unsigned int non_zero_neighbors = p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9;
	  
	  
	  if(2 <= non_zero_neighbors && non_zero_neighbors <= 6 &&
	     connectivity == 1) {
	    
	    if(condition_switch == true) {
	      
	      if(p2 * p4 * p6 == 0 && p4 * p6 * p8 == 0) {
		img->set_pixel(x, y, 0);
		running = true;
	      }
	    }
	    else {
	      if(p2 * p4 * p8 == 0 && p2 * p6 * p8 == 0) {
		img->set_pixel(x, y, 0);
		running = true;
	      }
	    }
	  }
	}

      }
    }
    
    return running;
  }

  /**
   * Zhang-Suen-Thinning of an image.
   */
  template<typename ImageType>
  void thinning(std::tr1::shared_ptr<ImageType> img) {
    assert_is_single_channel_image<ImageType>();

    bool running = true;

    do {
      running = zhang_suen_thinning_iteration(img, true);
      running = zhang_suen_thinning_iteration(img, false);
    }while(running);

  }

  
}
#endif
