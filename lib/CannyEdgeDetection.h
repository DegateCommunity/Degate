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

#ifndef __CANNYEDGEDETECTION_H__
#define __CANNYEDGEDETECTION_H__

#include <Image.h>

/*
#include <IPPipe.h>
#include <ImageHelper.h>

#include <IPCopy.h>
#include <IPConvolve.h>
#include <IPNormalize.h>
#include <IPThresholding.h>
#include <ImageManipulation.h>
#include <IPImageWriter.h>
#include <MedianFilter.h>

#include <QuadTree.h>
#include <algorithm>
#include <float.h>
#include <fstream>
*/

#include <EdgeDetection.h>

namespace degate {


  class CannyEdgeDetection : public EdgeDetection {

  private:

    double hysteresis_min;
    double hysteresis_max;

  private:

    void hysteresis(TileImage_GS_DOUBLE_shptr sup_edge_image);



    void non_maximum_supression(TileImage_GS_DOUBLE_shptr horizontal_edges,
				TileImage_GS_DOUBLE_shptr vertical_edges,
				TileImage_GS_DOUBLE_shptr edge_image,
				TileImage_GS_DOUBLE_shptr sup_edge_image);


    // returns the direction in degrees
    int get_gradient_direction(TileImage_GS_DOUBLE_shptr horizontal_edges,
			       TileImage_GS_DOUBLE_shptr vertical_edges,
			       TileImage_GS_DOUBLE_shptr edge_mag_image,
			       unsigned int x, unsigned int y);

  public:

    CannyEdgeDetection(unsigned int min_x, unsigned int max_x,
		       unsigned int min_y, unsigned int max_y,
		       unsigned int wire_diameter = 5,
		       unsigned int median_filter_width = 3,
		       unsigned int blur_kernel_size = 10,
		       double sigma = 0.5,
		       double _hysteresis_min = 0.28,
		       double _hysteresis_max = 0.40);

    ~CannyEdgeDetection();

    TileImage_GS_DOUBLE_shptr run(ImageBase_shptr img_in,
				  TileImage_GS_DOUBLE_shptr probability_map,
				  std::string const& directory);

    TileImage_GS_DOUBLE_shptr run(ImageBase_shptr img_in,
				  TileImage_GS_DOUBLE_shptr probability_map);


  };

}

#endif

