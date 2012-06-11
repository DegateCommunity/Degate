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

#ifndef __SUBIMAGEANALYZER_H__
#define __SUBIMAGEANALYZER_H__


#include <vector>
#include <algorithm>
#include <iostream>

#include <Image.h>
#include <TypeConstraints.h>

namespace degate {

  template<typename ImageType>
  class SubImageAnalyzer {

  private:
    std::shared_ptr<ImageType> img;
    unsigned int width;

  public:
    SubImageAnalyzer(std::shared_ptr<ImageType> _img,
		     unsigned int width) :
      img(_img),
      width(_width) {

      assert_is_multi_channel_image<ImageType>();
    }

    void run() {

      std::vector<rgba_pixel_t> v(width * width);

      int radius = width >> 1;
      unsigned int i = 0;

      for(unsigned int y = width; y < img->get_height() - radius; y++)
	for(unsigned int x = width; x < img->get_width() - radius; x++, i++) {

	  for(int _y = -radius; _y < radius; _y++)
	    for(int _x = -radius; _x < radius; _x++) {
	      rgba_pixel_t p = img->get_pixel(x + _x, y + _y);
	      vector[i] = p;
	    }
	}

      virtual bool calc(std::vector<rgba_pixel_t> const& v) const = 0;
    }

  };


}

#endif
