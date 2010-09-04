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

#ifndef __ZEROCROSSINGEDGEDETECTION_H__
#define __ZEROCROSSINGEDGEDETECTION_H__

#include <EdgeDetection.h>

namespace degate {

  class ZeroCrossingEdgeDetection : public EdgeDetection {

  private:

    unsigned int min_d;
    unsigned int max_d;
    double edge_threshold, zero_threshold;

  private:

    bool trace(TileImage_GS_DOUBLE_shptr edge_image,
	       int _x, int _y,
	       int inc_x, int inc_y,
	       int * start_x, int * stop_x, 
	       int * start_y, int * stop_y, 
	       double * mag,
	       double edge_threshold,
	       double zero_threshold,
	       unsigned int min_d, unsigned int max_d);

    TileImage_GS_DOUBLE_shptr analyze_edge_image(TileImage_GS_DOUBLE_shptr edge_image,
						 TileImage_GS_DOUBLE_shptr probability_map,
						 unsigned int min_d, unsigned int max_d);

    void overlay_result(TileImage_GS_DOUBLE_shptr zc, 
			TileImage_GS_DOUBLE_shptr bg, 
			//TileImage_RGBA_shptr bg, 
			std::string const& directory) const;
  public:

    ZeroCrossingEdgeDetection(unsigned int min_x, unsigned int max_x, 
			      unsigned int min_y, unsigned int max_y,
			      unsigned int median_filter_width = 3,
			      unsigned int blur_kernel_size = 10,
			      double sigma = 0.5,
			      unsigned int _min_d = 1, unsigned int _max_d = 10,
			      double _edge_threshold = 0.25, double _zero_threshold = 0.4);

    TileImage_GS_DOUBLE_shptr run(ImageBase_shptr img_in, 
				  TileImage_GS_DOUBLE_shptr probability_map,
				  std::string const& directory);

    TileImage_GS_DOUBLE_shptr run(ImageBase_shptr img_in, 
				  TileImage_GS_DOUBLE_shptr probability_map);


  };

}


#endif
