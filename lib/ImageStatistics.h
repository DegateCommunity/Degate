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

#ifndef __IMAGESTATISTICS_H__
#define __IMAGESTATISTICS_H__

#include <BoundingBox.h>
#include <ImageManipulation.h>

namespace degate {

  // We need a forward decleration here in order to use img->get_pixel_as<>().
  template<typename PixelTypeDst, typename ImageTypeSrc>
  inline PixelTypeDst get_pixel_as(typename std::shared_ptr<ImageTypeSrc> img,
				   unsigned int x, unsigned int y);

  /**
   * Get the minimum pixel value of a single channel image.
   */
  template<typename ImageType>
  typename ImageType::pixel_type get_minimum(std::shared_ptr<ImageType> img) {

    assert_is_single_channel_image<ImageType>();
    typename ImageType::pixel_type minimum = img->get_pixel(0, 0);

    for(unsigned int y = 0; y < img->get_height(); y++)
      for(unsigned int x = 0; x < img->get_width(); x++) {
	typename ImageType::pixel_type p = img->get_pixel(x, y);
	if(p < minimum) minimum = p;
      }
    return minimum;
  }

  /**
   * Get the maximum pixel value of a single channel image.
   */
  template<typename ImageType>
  typename ImageType::pixel_type get_maximum(std::shared_ptr<ImageType> img) {

    assert_is_single_channel_image<ImageType>();
    typename ImageType::pixel_type maximum = img->get_pixel(0, 0);

    for(unsigned int y = 0; y < img->get_height(); y++)
      for(unsigned int x = 0; x < img->get_width(); x++) {
	typename ImageType::pixel_type p = img->get_pixel(x, y);
	if(p > maximum) maximum = p;
      }
    return maximum;
  }


  /**
   * Calculate the average pixel value of a single channel image.
   * If the input image is a multi-channel image, data will be converted on-the-fly.
   */
  template<typename ImageType>
  double average(std::shared_ptr<ImageType> img) {
    return average(img, 0, 0, img->get_width(), img->get_height());
  }

  /**
   * Calculate the average pixel value of a single channel image.
   * If the input image is a multi-channel image, data will be converted on-the-fly.
   * @exception DegateRuntimeException This exception is thrown, if the image area is 0. 
   */
  template<typename ImageType>
  double average(std::shared_ptr<ImageType> img, 
		 unsigned int start_x, unsigned int start_y, 
		 unsigned int width, unsigned int height) {

    double sum = 0;

    if(height == 0 || width == 0) throw DegateRuntimeException("Can't calculate average for an image.");

    for(unsigned int y = start_y; y < height; y++)
      for(unsigned int x = start_x; x < width; x++) {
	sum += img->template get_pixel_as<double>(x, y);
      }

    return sum / (double)(height * width);
  }

  /**
   * Calculate the average pixel value of a single channel image.
   * If the input image is a multi-channel image, data will be converted on-the-fly.
   * @exception DegateRuntimeException This exception is thrown, if the image area is 0. 
   */
  template<typename ImageType>
  void average_and_stddev(std::shared_ptr<ImageType> img, 
			  unsigned int start_x, unsigned int start_y, 
			  unsigned int width, unsigned int height,
			  double * avg, double * stddev) {

    double sum = 0;
    
    if(height == 0 || width == 0)
      throw DegateRuntimeException("Can't calculate average for an image.");
    
    for(unsigned int y = start_y; y < start_y + height; y++)
      for(unsigned int x = start_x; x < start_x + width; x++) {
	sum += img->template get_pixel_as<gs_double_pixel_t>(x, y);
      }
    
    *avg = sum / (double)(height * width);
    
    sum = 0;
    
    for(unsigned int y = start_y; y < start_y + height; y++)
      for(unsigned int x = start_x; x < start_x + width; x++) {
	sum += pow(*avg - img->template get_pixel_as<gs_double_pixel_t>(x, y), 2);
      }
    
    *stddev = sqrt(sum/(double)(height * width));
  }


}

#endif
