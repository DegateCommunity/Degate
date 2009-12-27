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

#include <CannyEdgeDetection.h>
#include <IPPipe.h>
#include <IPCopy.h>
#include <IPConvolve.h>
#include <IPNormalize.h>
#include <IPThresholding.h>
#include <ImageManipulation.h>
#include <IPImageWriter.h>

using namespace degate;

CannyEdgeDetection::CannyEdgeDetection(unsigned int min_x, unsigned int max_x, 
				       unsigned int min_y, unsigned int max_y,
				       unsigned int wire_diameter,
				       unsigned int median_filter_width,
				       unsigned int blur_kernel_size,
				       double sigma,
				       double _hysteresis_min,
				       double _hysteresis_max) : 
  EdgeDetection(min_x, max_x, min_y, max_y, wire_diameter, median_filter_width, blur_kernel_size, sigma),
  hysteresis_min(_hysteresis_min),
  hysteresis_max(_hysteresis_max) {
  }

CannyEdgeDetection::~CannyEdgeDetection() {}


TileImage_GS_DOUBLE_shptr CannyEdgeDetection::run(ImageBase_shptr img_in, 
						  TileImage_GS_DOUBLE_shptr probability_map) {
  run_edge_detection(img_in);
  TileImage_GS_DOUBLE_shptr edge_image = get_edge_image(probability_map);
  TileImage_GS_DOUBLE_shptr edge_magnitude_image = get_edge_magnitude_image(probability_map);
  
  TileImage_GS_DOUBLE_shptr sup_edge_image(new TileImage_GS_DOUBLE(get_width(), get_height()));
  non_maximum_supression(get_horizontal_edges(), get_vertical_edges(), 
			 edge_magnitude_image, sup_edge_image);
  
  normalize<TileImage_GS_DOUBLE, TileImage_GS_DOUBLE>(sup_edge_image, sup_edge_image, 0, 1);
  
  hysteresis(sup_edge_image);
  
  return sup_edge_image;
}

TileImage_GS_DOUBLE_shptr CannyEdgeDetection::run(ImageBase_shptr img_in, 
						  TileImage_GS_DOUBLE_shptr probability_map,
						  std::string const& directory) {
  
  set_directory(directory);
  TileImage_GS_DOUBLE_shptr sup_edge_image = run(img_in, probability_map);
  
  save_normalized_image<TileImage_GS_DOUBLE>(join_pathes(directory, "06_hysteresis.tif"), 
					     sup_edge_image);

  return sup_edge_image;
}


void CannyEdgeDetection::hysteresis(TileImage_GS_DOUBLE_shptr sup_edge_image) {
  unsigned int x, y;
  for(y = get_border(); y < sup_edge_image->get_height() - get_border(); y++) {
    for(x = get_border(); x < sup_edge_image->get_width() - get_border(); x++) {
      if(sup_edge_image->get_pixel(x, y) >= hysteresis_max)
	sup_edge_image->set_pixel(x, y, 1);
      else if(sup_edge_image->get_pixel(x, y) <= hysteresis_min)
	sup_edge_image->set_pixel(x, y, 0);
      else
	sup_edge_image->set_pixel(x, y, 2);
    }
  }
  
  
  bool running = true;
  while(running) {
    running = false;
    
    for(y = get_border(); y < sup_edge_image->get_height() - get_border(); y++) {
      for(x = get_border(); x < sup_edge_image->get_width() - get_border(); x++) {
	if(sup_edge_image->get_pixel(x, y) == 2 && 
	   ( sup_edge_image->get_pixel(x-1, y-1) == 1 ||
	     sup_edge_image->get_pixel(x  , y-1) == 1 ||
	     sup_edge_image->get_pixel(x+1, y-1) == 1 ||
	     
	     sup_edge_image->get_pixel(x-1, y) == 1 ||
	     sup_edge_image->get_pixel(x+1, y) == 1 ||
	     
	     sup_edge_image->get_pixel(x-1, y+1) == 1 ||
	     sup_edge_image->get_pixel(x  , y+1) == 1 ||
	     sup_edge_image->get_pixel(x+1, y+1) == 1)) {
	  
	  sup_edge_image->set_pixel(x, y, 1);
	  running = true;
	}
      }
    }
  }
  
}


void CannyEdgeDetection::non_maximum_supression(TileImage_GS_DOUBLE_shptr horizontal_edges,
						TileImage_GS_DOUBLE_shptr vertical_edges,
						TileImage_GS_DOUBLE_shptr edge_image,
						TileImage_GS_DOUBLE_shptr sup_edge_image) {
  
  for(unsigned int y = get_border(); y < edge_image->get_height() - get_border(); y++) {
    for(unsigned int x = get_border(); x < edge_image->get_width() - get_border(); x++) {
      
      int gradient_direction = get_gradient_direction(horizontal_edges, 
						      vertical_edges, edge_image, x, y);
      
      gs_double_pixel_t pix = edge_image->get_pixel(x, y);
      if(pix > 0 && gradient_direction == -1) 
	sup_edge_image->set_pixel(x, y, pix);
      
    }
  }
  
  if(has_directory())
    save_normalized_image<TileImage_GS_DOUBLE>(join_pathes(get_directory(),
							   "04_non_max_suppression.tif"),
					       sup_edge_image);
}



    // returns the direction in degrees
int CannyEdgeDetection::get_gradient_direction(TileImage_GS_DOUBLE_shptr horizontal_edges,
					       TileImage_GS_DOUBLE_shptr vertical_edges,
					       TileImage_GS_DOUBLE_shptr edge_mag_image,
					       unsigned int x, unsigned int y) {
  double tangent = 5;
  // determine gradient direction
  if(vertical_edges->get_pixel(x, y) != 0) 
    tangent = horizontal_edges->get_pixel(x, y) / vertical_edges->get_pixel(x, y);
  
  double 
    nw = edge_mag_image->get_pixel(x - 1, y - 1),
    n  = edge_mag_image->get_pixel(x    , y - 1),
    ne = edge_mag_image->get_pixel(x + 1, y - 1),
    w  = edge_mag_image->get_pixel(x - 1, y    ),
    e  = edge_mag_image->get_pixel(x + 1, y    ),
    sw = edge_mag_image->get_pixel(x - 1, y + 1),
    s  = edge_mag_image->get_pixel(x    , y + 1),
    se = edge_mag_image->get_pixel(x + 1, y + 1),
    curr = edge_mag_image->get_pixel(x, y);
  
  // comment specifies gradient
  if(fabs(tangent) <= 0.4142) { // -	
    if(curr < w && curr < nw && curr < sw) return 180;
    if(curr < e && curr < ne && curr < se) return 90;
    if(curr < w) return 180;
    if(curr < e) return 90;
  }
  if(0.4142 < tangent && tangent <= 2.4142) { //  /
    if(curr < ne && curr < n && curr < e) return 45;
    if(curr < sw && curr < s && curr < w) return 225;
    if(curr < ne) return 45;
    if(curr < sw) return 225;
  }
  if(fabs(tangent) > 2.4142) { //  |
    if(curr < n && curr < nw && curr < ne) return 90;
    if(curr < s && curr < sw && curr < se) return 270;
    if(curr < n) return 90;
    if(curr < s) return 270;
  }
  if(-2.4142 < tangent && tangent <= -0.4142) { // 
    if(curr < nw && curr < n && curr < w) return 135;
    if(curr < se && curr < s && curr < e) return 315;
    if(curr < nw) return 135;
    if(curr < se) return 315;
  }	  
  return -1;
}

