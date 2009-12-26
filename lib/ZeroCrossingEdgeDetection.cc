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

#include <ZeroCrossingEdgeDetection.h>
#include <MorphologicalFilter.h>

using namespace degate;

ZeroCrossingEdgeDetection::ZeroCrossingEdgeDetection(unsigned int min_x, unsigned int max_x, 
						     unsigned int min_y, unsigned int max_y,
						     unsigned int wire_diameter,
						     unsigned int blur_kernel_size,
						     double sigma,
						     unsigned int _min_d, 
						     unsigned int _max_d,
						     double _edge_threshold, 
						     double _zero_threshold) : 
  EdgeDetection(min_x, max_x, min_y, max_y, wire_diameter, blur_kernel_size, sigma),
  min_d(_min_d), 
  max_d(_max_d),
  edge_threshold(_edge_threshold),
  zero_threshold(_zero_threshold) {
  }

TempImage_GS_DOUBLE_shptr ZeroCrossingEdgeDetection::run(ImageBase_shptr img_in, 
							 TempImage_GS_DOUBLE_shptr probability_map,
							 std::string const& directory) {
  set_directory(directory);
  run_edge_detection(img_in);
  TempImage_GS_DOUBLE_shptr edge_image = get_edge_image(probability_map);
  TempImage_GS_DOUBLE_shptr edge_magnitude_image = get_edge_magnitude_image(probability_map);
  
  TempImage_GS_DOUBLE_shptr zero_cross_img = analyze_edge_image(edge_image, probability_map, min_d, max_d);
  TempImage_GS_DOUBLE_shptr zero_cross_img2(new TempImage_GS_DOUBLE(get_width(), get_height()));
  
  morphological_close<TempImage_GS_DOUBLE, TempImage_GS_DOUBLE>(zero_cross_img2, zero_cross_img, 3, 1);
  
  thinning<TempImage_GS_DOUBLE>(zero_cross_img2);
  
  save_normalized_image<TempImage_GS_DOUBLE>(join_pathes(directory, 
							 "03_edge_zero_cross.tif"), 
					     zero_cross_img2);
  
  //overlay_result(zero_cross_img, std::tr1::dynamic_pointer_cast<TempImage_RGBA>(img_in), directory);
  overlay_result(zero_cross_img2, edge_image, directory);
  return zero_cross_img2;
}


bool ZeroCrossingEdgeDetection::trace(TempImage_GS_DOUBLE_shptr edge_image,
				      int _x, int _y,
				      int inc_x, int inc_y,
				      int * start_x, int * stop_x, 
				      int * start_y, int * stop_y, 
				      double * mag,
				      double edge_threshold,
				      double zero_threshold,
				      unsigned int min_d, unsigned int max_d) {
  
  if(start_x == NULL || start_y == NULL || 
     stop_x == NULL || stop_y == NULL || mag == NULL) return false;
  
  enum STATE {BEFORE, POS_EDGE, NEG_EDGE, END};
  STATE s = BEFORE;
  int x = _x, y = _y;
  double max_pix = 0, min_pix = 0;
  
  while(s != END) {
    double p = edge_image->get_pixel(x, y);
    
    if(s == BEFORE && p >= edge_threshold) {
      max_pix = p;
      *start_x = x;
      *start_y = y;
      s = POS_EDGE;
    }
    else if(s == POS_EDGE) {
      if(p > max_pix) {
	*start_x = x; 
	*start_y = y;
	max_pix = p;
      }
      if(p <= -edge_threshold) {
	*stop_x = x; 
	*stop_y = y;
	min_pix = p; 
	s = NEG_EDGE;
      }
    }
    else if(s == NEG_EDGE) {
      if(p < min_pix) {
	min_pix = p;
	*stop_x = x;
	*stop_y = y;
      }
      if(p >= 0 ||
	 x > *stop_x || y > *stop_y) { // that is at least one step beyond the maximum
	
	unsigned int
	  d_x = abs(*stop_x - *start_x),
	  d_y = abs(*stop_y - *start_y);
	
	
	if( !((d_x > min_d && d_x < max_d) ||
	      (d_y > min_d && d_y < max_d))) return false;
	
	s = END;
	*mag = sqrt(pow(max_pix,2) + pow(min_pix, 2));
	
	double p_center = edge_image->get_pixel(*start_x + (*stop_x - *start_x)/2,
						*start_y + (*stop_y - *start_y)/2);
	
	return fabs(p_center) < zero_threshold;
	
      }
    }
    x += inc_x;
    y += inc_y;
    if(x >= edge_image->get_width() || 
       y >= edge_image->get_height() ||
       x == 0 || y == 0) s = END;
  }
  return false;
}


TempImage_GS_DOUBLE_shptr 
ZeroCrossingEdgeDetection::analyze_edge_image(TempImage_GS_DOUBLE_shptr edge_image,
					      TempImage_GS_DOUBLE_shptr probability_map,
					      unsigned int min_d, unsigned int max_d) {
  int start_x = 0, start_y = 0, stop_x = 0, stop_y = 0, x, y;
  double mag = 0;
  
  normalize<TempImage_GS_DOUBLE, TempImage_GS_DOUBLE>(edge_image, edge_image, -1, 1);
  
  TempImage_GS_DOUBLE_shptr out_image(new TempImage_GS_DOUBLE(get_width(), get_height()));
  
  for(y = get_border() ; y < edge_image->get_height() - get_border(); y++) {
    for(x = get_border(); x < edge_image->get_width() - get_border();) {
      
      if(trace(edge_image, x, y, 1, 0, 
	       &start_x, &stop_x, &start_y, &stop_y, &mag,
	       edge_threshold, zero_threshold, 
	       min_d, max_d)) {
	out_image->set_pixel(start_x + (stop_x - start_x)/2, y, mag);
	x += (stop_x > start_x + 1) ? stop_x - start_x - 1 : stop_x - start_x;
      }
      else x++;
    }
  }
  
  for(x = get_border(); x < edge_image->get_width() - get_border(); x++) {
    for(y = get_border(); y < edge_image->get_height() - get_border();) {
      if(trace(edge_image, x, y, 0, 1, 
	       &start_x, &stop_x, &start_y, &stop_y, &mag,
	       edge_threshold, zero_threshold, min_d, max_d)) {
	out_image->set_pixel(x, start_y + (stop_y - start_y)/2, mag);
	y += (stop_y > start_y + 1) ? stop_y - start_y -1 : stop_y - start_y;
      }
      else y++;
    }  
  }
  
  
      /*
      for(x = get_border(); x < edge_image->get_width() - get_border(); x++) {
	for(y = get_border(); y < edge_image->get_height() - get_border(); y++) {
	  if(trace(edge_image, x, y, 1, 1, 
		   &start_x, &stop_x, &start_y, &stop_y, &mag, 
		   edge_threshold, zero_threshold, sqrt(2*min_d*min_d), sqrt(2*max_d*max_d))) {
	    int 
	      p_x = start_x + (stop_x - start_x)/2,
	      p_y = start_y + (stop_y - start_y)/2;
	    if(p_x >= 0 && p_y >= 0 && p_x < out_image->get_width() && p_y < out_image->get_height())
	      out_image->set_pixel(p_x, p_y, mag);
	  }
	}  
      }
      

      for(x = get_border(); x < edge_image->get_width() - get_border(); x++) {
	for(y = get_border(); y < edge_image->get_height() - get_border(); y++) {
	  if(trace(edge_image, x, y, -1, 1, 
		   &start_x, &stop_x, &start_y, &stop_y, &mag, 
		   edge_threshold, zero_threshold, sqrt(2*min_d*min_d), sqrt(2*max_d*max_d))) {
	    int 
	      p_x = start_x + (stop_x - start_x)/2,
	      p_y = start_y + (stop_y - start_y)/2;
	    if(p_x >= 0 && p_y >= 0 && p_x < out_image->get_width() && p_y < out_image->get_height())
	      out_image->set_pixel(p_x, p_y, mag);
	  }
	}  
      }
      */

  return out_image;
}

void ZeroCrossingEdgeDetection::overlay_result(TempImage_GS_DOUBLE_shptr zc, 
					       TempImage_GS_DOUBLE_shptr bg, 
					       //TempImage_RGBA_shptr bg, 
					       std::string const& directory) const {

  assert(bg != NULL && zc != NULL);

  if(bg != NULL && zc != NULL) {
    for(unsigned int y = 0; y < get_height(); y++)
      for(unsigned int x = 0; x < get_width(); x++) {
	if(zc->get_pixel(x, y) > 0)
	  bg->set_pixel(x, y, -1);
      }
    //save_image<TempImage_RGBA>(join_pathes(directory, "overlay.tif"),  bg);
    save_normalized_image<TempImage_GS_DOUBLE>(join_pathes(directory, "overlay.tif"),  bg);
  }
}

