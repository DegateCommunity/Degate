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

#ifndef __STATISTICS_H__
#define __STATISTICS_H__

#include <vector>
#include <algorithm>

namespace degate {

  /**
   * Calculate the median of a vector.
   * @exception DegateRuntimeException This exception is thrown if
   *   vector's size is 0.
   */
  template<typename T>
  inline T median(std::vector<T> & v) throw(DegateRuntimeException) {
    
    if(v.empty()) 
      throw DegateRuntimeException("Error in median(): The vector is empty.");
    
    std::sort(v.begin(), v.end());
    
    unsigned int center = v.size()/2;
    
    if(v.size() % 2 == 0) {
      return (v[center - 1] + v[center + 1]) / 2;
    }
    else 
      return v[center];
  }
  
  
  /**
   * Calculate the average of a vector.
   * @exception DegateRuntimeException This exception is thrown if
   *   vector's size is 0.
   */
  template<typename T>
  inline T average(std::vector<T> const& v) throw(DegateRuntimeException) {
    
    if(v.empty()) 
      throw DegateRuntimeException("Error in average(): The vector is empty.");
    
    double _sum = 0;

    for(typename std::vector<T>::const_iterator iter = v.begin();
	iter != v.end(); ++iter)
      _sum += *iter;
    
    return _sum / (double)v.size();
  }
  
  
  /**
   * Calculate the standard deviation of a vector.
   * @exception DegateRuntimeException This exception is thrown if
   *   vector's size is 0.
   */
  template<typename T>
  inline T standard_deviation(std::vector<T> const& v) throw(DegateRuntimeException) {
    
    if(v.empty()) 
      throw DegateRuntimeException("Error in standard_deviation(): The vector is empty.");
    
    T avg = average<T>(v);
    double sum = 0;
    for(typename std::vector<T>::const_iterator iter = v.begin(); iter != v.end(); ++iter)
      sum += pow(avg - *iter, 2);

    return sqrt(sum);
  }


  /**
   * Get the minimum pixel value of a single channel image.
   */
  template<typename ImageType>
  typename ImageType::pixel_type get_minimum(std::tr1::shared_ptr<ImageType> img) {

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
  typename ImageType::pixel_type get_maximum(std::tr1::shared_ptr<ImageType> img) {

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
   * Calulate the average pixel value of a single channel image.
   */
  template<typename ImageType>
  double average(std::tr1::shared_ptr<ImageType> img) {

    assert_is_single_channel_image<ImageType>();

    double sum = 0;

    for(unsigned int y = 0; y < img->get_height(); y++)
      for(unsigned int x = 0; x < img->get_width(); x++) {
	sum += img->get_pixel(x, y);
      }

    return sum / (double)(img->get_height() * img->get_width());
  }


}



#endif
