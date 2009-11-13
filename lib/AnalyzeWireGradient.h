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

#ifndef __ANALYZEWIREGRADIENT_H__
#define __ANALYZEWIREGRADIENT_H__

#include <vector>
#include <algorithm>
#include <iostream>

#include <Image.h>
#include <TypeConstraints.h>

namespace degate {

  template<typename ProbabilityMap, typename ImageType>
  class AnalyzeWireGradient {

  private:
    std::tr1::shared_ptr<ProbabilityMap> probaility_map;
    std::tr1::shared_ptr<ImageType> img;
    unsigned int wire_width_min;
    unsigned int wire_width_max;

  public:
    AnalyzeWireGradient(std::tr1::shared_ptr<ProbabilityMap> _probaility_map,
			std::tr1::shared_ptr<ImageType> _img,
			unsigned int _wire_width_min, 
			unsigned int _wire_width_max) :
      probaility_map(_probaility_map),
      img(_img),
      wire_width_min(_wire_width_min),
      wire_width_max(_wire_width_max) {

      assert_is_single_channel_image<ProbabilityMap>();
      assert_is_multi_channel_image<ImageType>();

    }

    void run() {
      std::vector<double> gradient_collector_red = calc_profile();

      int radius = wire_width_max >> 1;
      for(unsigned int y = wire_width_max; y < img->get_height(); y++)
	for(unsigned int x = wire_width_max; x < img->get_width(); x++) {

	  for(int _y = -radius; _y < radius; _y++)
	    for(int _x = -radius; _x < radius; _x++) {
	      rgba_pixel_t p = img->get_pixel(x + _x, y + _y);
	      MASK_R(p);
	    }

	}

    }

    std::vector<double> calc_profile() {
      std::vector<double> gradient_collector_red(wire_width_max);
      unsigned int counter = 0;

      for(unsigned int y = 0; y < probaility_map->get_height(); y++)
	for(unsigned int x = 0; x < probaility_map->get_width();) {

	  if(probaility_map->get_pixel(x, y) > 0) {
	

	    unsigned int width = 0;
	    while(x + width < probaility_map->get_width() && 
		  probaility_map->get_pixel(x + width, y) > 0) width++;


	    if(width >= wire_width_min && width <= wire_width_max) {

	      std::vector<double> gradient_r(width);

	      // possible wire from x to x+width

	      for(unsigned int _x = 0; _x < width; _x++) {
		gradient_r[_x] = MASK_R(img->get_pixel(_x + width, y)); 
	      }
	      
	      unsigned int center_idx = get_center(gradient_r);
	      if(center_idx != 0 && center_idx < gradient_r.size() &&
		 gradient_r.front() < gradient_r.at(center_idx) &&
		 gradient_r.back() < gradient_r.at(center_idx)) {
		sum_up(gradient_collector_red, gradient_r, center_idx);
		counter++;
	      }
	      else
		std::cout << "reject" << std::endl;
	    }

	    x += width;
	  }
	  else x++;
	}

      std::cout << "counts: " << counter <<std::endl;
      print_vector(gradient_collector_red);
      for(std::vector<double>::iterator iter = gradient_collector_red.begin();
	  iter != gradient_collector_red.end(); ++iter)
	*iter = *iter / (double)counter;
      print_vector(gradient_collector_red);

      return gradient_collector_red;
    }


  protected:

    void print_vector(std::vector<double> const& v) const {
      std::cout << "vector: ";
      for(unsigned int i = 0; i < v.size(); i++)
	std::cout << v.at(i) << " ";
      std::cout << std::endl;
    }

    inline unsigned int get_center(std::vector<double> const& v) const {
      if(v.size() == 0) return 0;
      
      print_vector(v);

      unsigned int max_pos = 0;
      double max_val = v.front();

      for(unsigned int i = 1; i < v.size(); i++)  {
	if(v.at(i) > max_val) {
	  max_val = v.at(i);
	  max_pos = i;
	}
      }

      std::cout << "center is: " << max_pos << std::endl;
      return max_pos;
    }


    inline void sum_up(std::vector<double> & gradient_collector, 
		       std::vector<double> const& gradient_r, 
		       unsigned int center_idx) const {

      unsigned int collector_center = gradient_collector.size() / 2;
      assert(collector_center > 0);
      assert(gradient_collector.size() >= gradient_r.size());

      for(unsigned int i = 0; i < gradient_r.size(); i++) {

	unsigned int offs = i + (collector_center > center_idx ? 
				 collector_center - center_idx : 
				 center_idx - collector_center);

	if(offs >= 0 && offs < gradient_collector.size())
	  gradient_collector[offs] += gradient_r.at(i);

      }

    }


  };
}

#endif
