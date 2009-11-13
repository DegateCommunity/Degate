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

#ifndef __WIREMATCHINGCANNY_H__
#define __WIREMATCHINGCANNY_H__

#include <Image.h>
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

namespace degate {


  class WireMatchingCanny {
  private:
    unsigned int min_x, max_x, min_y, max_y;
    IPPipe pipe;
    unsigned int wire_diameter;

    void hline(TempImage_GS_DOUBLE_shptr img, 
	       unsigned int x1, unsigned int x2, unsigned int y, double c) {
      assert(y < img->get_height());
      for(unsigned int _x = x1; _x < x2; _x++) {
	assert(_x < img->get_width());
	img->set_pixel(_x, y, c);
      }
    }

  public:

    WireMatchingCanny(unsigned int _min_x, unsigned int _max_x, 
		      unsigned int _min_y, unsigned int _max_y,
		      unsigned int _wire_diameter = 5) :
      min_x(_min_x), 
      max_x(_max_x), 
      min_y(_min_y), 
      max_y(_max_y),
      wire_diameter(_wire_diameter) {
    }

    ~WireMatchingCanny() {}

    void run(ImageBase_shptr img_in, TempImage_GS_DOUBLE_shptr probability_map,
	     std::string const& directory) {

      std::tr1::shared_ptr<IPCopy<TempImage_RGBA, TempImage_GS_DOUBLE> > copy_rgba_to_gs
	(new IPCopy<TempImage_RGBA, TempImage_GS_DOUBLE>(min_x, max_x, min_y, max_y) );


      std::tr1::shared_ptr<GaussianBlur> GaussianB(new GaussianBlur(5, 5));
      GaussianB->print();
      std::tr1::shared_ptr<IPConvolve<TempImage_GS_DOUBLE, TempImage_GS_DOUBLE> > gaussian_blur
	(new IPConvolve<TempImage_GS_DOUBLE, TempImage_GS_DOUBLE>(GaussianB) );
      
      
      std::tr1::shared_ptr<SobelYOperator> SobelY(new SobelYOperator());
      std::tr1::shared_ptr<IPConvolve<TempImage_GS_DOUBLE, TempImage_GS_DOUBLE> > edge_filter_x
	(new IPConvolve<TempImage_GS_DOUBLE, TempImage_GS_DOUBLE>(SobelY) );

      std::tr1::shared_ptr<SobelXOperator> SobelX(new SobelXOperator());
      std::tr1::shared_ptr<IPConvolve<TempImage_GS_DOUBLE, TempImage_GS_DOUBLE> > edge_filter_y
	(new IPConvolve<TempImage_GS_DOUBLE, TempImage_GS_DOUBLE>(SobelX) );


      std::tr1::shared_ptr<IPNormalize<TempImage_GS_DOUBLE, TempImage_GS_DOUBLE> > normalizer
	(new IPNormalize<TempImage_GS_DOUBLE, TempImage_GS_DOUBLE>(0, 1) );

      ImageBase_shptr i;

      i = copy_rgba_to_gs->run(img_in);
      save_normalized_image<TempImage_GS_DOUBLE>(join_pathes(directory, "00_input.tif"), 
						 std::tr1::dynamic_pointer_cast<TempImage_GS_DOUBLE>(i));
      //i = gaussian_blur->run(i);
      
      TempImage_GS_DOUBLE_shptr i1 = std::tr1::dynamic_pointer_cast<TempImage_GS_DOUBLE>(edge_filter_x->run(i));
      TempImage_GS_DOUBLE_shptr i2 = std::tr1::dynamic_pointer_cast<TempImage_GS_DOUBLE>(edge_filter_y->run(i));

      save_normalized_image<TempImage_GS_DOUBLE>(join_pathes(directory, "01_sobelx.tif"), i1);
      save_normalized_image<TempImage_GS_DOUBLE>(join_pathes(directory, "02_sobely.tif"), i2);


      unsigned int width = i1->get_width();
      unsigned int height = i1->get_height();
      unsigned int x,y;



      TempImage_GS_DOUBLE_shptr sobel_map(new TempImage_GS_DOUBLE(width, height));
      TempImage_GS_DOUBLE_shptr sobel_map2(new TempImage_GS_DOUBLE(width, height));
      for(y = 0; y < height; y++) {
	for(x = 0; x < width; x++) {
	  sobel_map->set_pixel(x, y, i1->get_pixel(x, y) + i2->get_pixel(x, y));
	}
      }
      median_filter<TempImage_GS_DOUBLE, TempImage_GS_DOUBLE>(sobel_map2, sobel_map, 3);
      save_normalized_image<TempImage_GS_DOUBLE>(join_pathes(directory, "03_sobelmap.tif"), sobel_map);
      save_normalized_image<TempImage_GS_DOUBLE>(join_pathes(directory, "03_sobelmap2.tif"), sobel_map2);






      TempImage_GS_DOUBLE_shptr edge_image(new TempImage_GS_DOUBLE(width, height));


      for(y = 0; y < height; y++) {
	for(x = 0; x < width; x++) {
	  double gx = i1->get_pixel(x, y);
	  double gy = i2->get_pixel(x, y);
	  //assert(gx != 0);
	  double g = sqrt(pow(gx,2) + pow(gy, 2));
	  //double theta = atan(gy / gx);
	  edge_image->set_pixel(x, y, g);
	}
      }
      
      normalize<TempImage_GS_DOUBLE, TempImage_GS_DOUBLE>(edge_image, edge_image, 0, 1);
      save_normalized_image<TempImage_GS_DOUBLE>(join_pathes(directory, "03_edge_magnitude.tif"), edge_image);


      




      TempImage_GS_DOUBLE_shptr sup_edge_image(new TempImage_GS_DOUBLE(width, height));

      //copy_image<TempImage_GS_DOUBLE, TempImage_GS_DOUBLE>(sup_edge_image, edge_image);



      for(y = 0; y < probability_map->get_height(); y++) {
	for(x = 0; x < probability_map->get_width(); x++) {
	  gs_double_pixel_t p = edge_image->get_pixel(x, y);
	  if(probability_map->get_pixel(x, y) <= 0) p = 0;
	  sup_edge_image->set_pixel(x, y, p);
	}
      }
      save_normalized_image<TempImage_GS_DOUBLE>(join_pathes(directory, "03.1_edge_magnitude.tif"), sup_edge_image);



      TempImage_GS_DOUBLE_shptr thresholded_image(new TempImage_GS_DOUBLE(width, height));
      thresholding_image<TempImage_GS_DOUBLE, TempImage_GS_DOUBLE>(thresholded_image, sup_edge_image, 0.2);
      save_normalized_image<TempImage_GS_DOUBLE>(join_pathes(directory, "03.2_thresholded.tif"), thresholded_image);

      TempImage_GS_DOUBLE_shptr thresholded_medianfiltered_image(new TempImage_GS_DOUBLE(width, height));
      median_filter<TempImage_GS_DOUBLE, TempImage_GS_DOUBLE>(thresholded_medianfiltered_image, thresholded_image, wire_diameter >> 1);
      save_normalized_image<TempImage_GS_DOUBLE>(join_pathes(directory, "03.3_medianfiltered.tif"), thresholded_medianfiltered_image);


      non_maximum_supression(i1, i2, sup_edge_image, directory);
      magnetic_edge(sup_edge_image, directory);


      normalize<TempImage_GS_DOUBLE, TempImage_GS_DOUBLE>(sup_edge_image, sup_edge_image, 0, 1);




      // repair
      /*
      double repair_thresh = 0.3;
      for(y = 1; y < sup_edge_image->get_height() - 1; y++) {
	for(x = 1; x < sup_edge_image->get_width() - 1; x++) {
	  if(sup_edge_image->get_pixel(x, y) == 0 &&
	     edge_image->get_pixel(x, y) >= repair_thresh) {

	    if((sup_edge_image->get_pixel(x, y-1) >= repair_thresh &&
		sup_edge_image->get_pixel(x, y+1) >= repair_thresh) ||
	       (sup_edge_image->get_pixel(x-1, y) >= repair_thresh &&
		sup_edge_image->get_pixel(x+1, y) >= repair_thresh)) {
	      sup_edge_image->set_pixel(x, y, edge_image->get_pixel(x, y));
	    }
	    
	  }
	}
	}

      save_normalized_image<TempImage_GS_DOUBLE>(join_pathes(directory, "05_contour_repair.tif"), sup_edge_image);
      */

      double hysteresis_min = 0.15;
      double hysteresis_max = 0.30;

      for(y = 1; y < sup_edge_image->get_height() - 1; y++) {
	for(x = 1; x < sup_edge_image->get_width() - 1; x++) {
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
	
	for(y = 1; y < sup_edge_image->get_height() - 1; y++) {
	  for(x = 1; x < sup_edge_image->get_width() - 1; x++) {
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

      save_normalized_image<TempImage_GS_DOUBLE>(join_pathes(directory, "06_hysteresis.tif"), sup_edge_image);
      
      // XXX
      /*
      BoundingBox fitted_bb = fit_single_rectangle(sup_edge_image, initial_extension(sup_edge_image, 39, 4));
      draw_rect(sup_edge_image, fitted_bb);

      fitted_bb = fit_single_rectangle(sup_edge_image, initial_extension(sup_edge_image, 66, 15));
      draw_rect(sup_edge_image, fitted_bb);
      */
      fit_rectangles(sup_edge_image);


      save_normalized_image<TempImage_GS_DOUBLE>(join_pathes(directory, "07_rect_fitting.tif"), sup_edge_image);
    }


    void magnetic_edge(TempImage_GS_DOUBLE_shptr sup_edge_image, std::string const& directory) {
      
      for(unsigned int y = 2; y < sup_edge_image->get_height() - 2; y++) {
	for(unsigned x = 2; x < sup_edge_image->get_width() - 2; x++) {
	  double 
	    curr = sup_edge_image->get_pixel(x, y),
	    nw = sup_edge_image->get_pixel(x - 2, y - 2),
	    n  = sup_edge_image->get_pixel(x    , y - 2),
	    ne = sup_edge_image->get_pixel(x + 2, y - 2),
	    w  = sup_edge_image->get_pixel(x - 2, y    ),
	    e  = sup_edge_image->get_pixel(x + 2, y    ),
	    sw = sup_edge_image->get_pixel(x - 2, y + 2),
	    s  = sup_edge_image->get_pixel(x    , y + 2),
	    se = sup_edge_image->get_pixel(x + 2, y + 2);

	  // dunkler pixel
	  if(curr < w && n < nw && s < sw) {
	    sup_edge_image->set_pixel(x - 1, y, w + curr);
	    sup_edge_image->set_pixel(x, y, 0);
	  }

	  if(curr < e && n < ne && s < se) {
	    sup_edge_image->set_pixel(x + 1, y, e + curr);
	    sup_edge_image->set_pixel(x, y, 0);
	  }

	  if(curr < n && w < nw && e < ne) {
	    sup_edge_image->set_pixel(x + 1, y, e + curr);
	    sup_edge_image->set_pixel(x, y, 0);
	  }

	}
      }

      save_normalized_image<TempImage_GS_DOUBLE>(join_pathes(directory, "04_magnetic_edge.tif"), sup_edge_image);
    }


    void non_maximum_supression(TempImage_GS_DOUBLE_shptr i1,
				TempImage_GS_DOUBLE_shptr i2,
				TempImage_GS_DOUBLE_shptr sup_edge_image,
				std::string const& directory) {
      
      unsigned int width = sup_edge_image->get_width();
      unsigned int height = sup_edge_image->get_height();
      unsigned int x,y;

      TempImage_GS_DOUBLE_shptr suppression_accu(new TempImage_GS_DOUBLE(width, height));
      
      unsigned int nms_counter = 0;
      while(nms_counter<10) {
	for(y = 2; y < sup_edge_image->get_height() - 2; y++) {
	  for(x = 2; x < sup_edge_image->get_width() - 2; x++) {
	    nms_counter++;
	    double tangent = 5;
	    
	    // determine gradient direction
	    if(i2->get_pixel(x, y) != 0) 
	      tangent = i1->get_pixel(x, y) / i2->get_pixel(x, y);
	    
	    double 
	      nw = sup_edge_image->get_pixel(x - 1, y - 1),
	      n  = sup_edge_image->get_pixel(x    , y - 1),
	      ne = sup_edge_image->get_pixel(x + 1, y - 1),
	      w  = sup_edge_image->get_pixel(x - 1, y    ),
	      e  = sup_edge_image->get_pixel(x + 1, y    ),
	      sw = sup_edge_image->get_pixel(x - 1, y + 1),
	      s  = sup_edge_image->get_pixel(x    , y + 1),
	      se = sup_edge_image->get_pixel(x + 1, y + 1),
	      curr = sup_edge_image->get_pixel(x, y);
	    
	    double curr_accu = suppression_accu->get_pixel(x,y);
	    double delta = (curr + curr_accu);
	    
	    if(fabs(tangent) <= 0.4142) { // -
	      if(curr < e) {
		sup_edge_image->set_pixel(x, y, 0);
		suppression_accu->set_pixel(x+1,y, delta); // e
		suppression_accu->set_pixel(x,y, 0);
	      }
	      
	      if(curr < w) {
		sup_edge_image->set_pixel(x, y, 0);
		suppression_accu->set_pixel(x-1,y, delta); // w
		suppression_accu->set_pixel(x,y, 0);
	      }
	      
	    }
	    
	    if(0.4142 < tangent && tangent <= 2.4142) { //  /
	      if(curr < ne) {
		sup_edge_image->set_pixel(x, y, 0);
		suppression_accu->set_pixel(x+1,y-1, delta); // ne
		suppression_accu->set_pixel(x,y, 0);
	      }
	      if(curr < sw) {
		sup_edge_image->set_pixel(x, y, 0);
		suppression_accu->set_pixel(x-1,y+1, delta); // sw
		suppression_accu->set_pixel(x,y, 0);
	      }
	    }
	    
	    if(fabs(tangent) > 2.4142) { //  |
	      
	      if(curr < n) {
		sup_edge_image->set_pixel(x, y, 0);
		suppression_accu->set_pixel(x,y-1, delta); // n
		suppression_accu->set_pixel(x, y, 0);
	      }
	      
	      if(curr < s) {
		sup_edge_image->set_pixel(x, y, 0);
		suppression_accu->set_pixel(x,y+1, delta); // s
		suppression_accu->set_pixel(x,y, 0);
	      }
	    }
	    
	    
	    if(2.4142 < tangent && tangent <= -0.4142) { // 
	      if(curr < nw) {
		sup_edge_image->set_pixel(x, y, 0);
		suppression_accu->set_pixel(x-1,y-1, delta); // nw
		suppression_accu->set_pixel(x, y, 0);
	      }
	      
	      if(curr < se) {
		sup_edge_image->set_pixel(x, y, 0);
		suppression_accu->set_pixel(x+1,y+1, delta); // se
		suppression_accu->set_pixel(x, y, 0);
	      }
	    }
	  }
	}
	

      for(y = 2; y < sup_edge_image->get_height() - 2; y++) {
	for(x = 2; x < sup_edge_image->get_width() - 2; x++) {
	  if(sup_edge_image->get_pixel(x, y) > 0)
	    sup_edge_image->set_pixel(x, y, 
				      sup_edge_image->get_pixel(x, y) + 
				      suppression_accu->get_pixel(x,y));
	}
      }

      }
      
      save_normalized_image<TempImage_GS_DOUBLE>(join_pathes(directory, "04_non_max_suppression.tif"), sup_edge_image);

      save_normalized_image<TempImage_GS_DOUBLE>(join_pathes(directory, "03.4_suppression_accu.tif"), suppression_accu);
      
      //suppression_accu->clear();
      
      
      
      //save_normalized_image<TempImage_GS_DOUBLE>(join_pathes(directory, "04_non_max_suppression_with_accu.tif"), sup_edge_image);
      
    }











    void non_maximum_supression2(TempImage_GS_DOUBLE_shptr i1,
				TempImage_GS_DOUBLE_shptr i2,
				TempImage_GS_DOUBLE_shptr sup_edge_image,
				std::string const& directory) {
      
      unsigned int width = sup_edge_image->get_width();
      unsigned int height = sup_edge_image->get_height();
      unsigned int x,y;

      TempImage_GS_DOUBLE_shptr suppression_accu(new TempImage_GS_DOUBLE(width, height));
      
      unsigned int nms_counter = 0;
      while(nms_counter<1) {
	for(y = 2; y < sup_edge_image->get_height() - 2; y++) {
	  for(x = 2; x < sup_edge_image->get_width() - 2; x++) {
	    nms_counter++;
	    double tangent = 5;
	    
	    // determine gradient direction
	    if(i2->get_pixel(x, y) != 0) 
	      tangent = i1->get_pixel(x, y) / i2->get_pixel(x, y);
	    
	    double 
	      nw = sup_edge_image->get_pixel(x - 1, y - 1),
	      n  = sup_edge_image->get_pixel(x    , y - 1),
	      ne = sup_edge_image->get_pixel(x + 1, y - 1),
	      w  = sup_edge_image->get_pixel(x - 1, y    ),
	      e  = sup_edge_image->get_pixel(x + 1, y    ),
	      sw = sup_edge_image->get_pixel(x - 1, y + 1),
	      s  = sup_edge_image->get_pixel(x    , y + 1),
	      se = sup_edge_image->get_pixel(x + 1, y + 1),
	      curr = sup_edge_image->get_pixel(x, y);
	    
	    double curr_accu = suppression_accu->get_pixel(x,y);
	    double delta = (curr + curr_accu);
	    double delta_3 = delta / 3.0;
	    
	    if(fabs(tangent) <= 0.4142) { // -
	      if(curr < e && curr < ne && curr < se) {
		sup_edge_image->set_pixel(x, y, 0);
		suppression_accu->set_pixel(x+1,y-1, delta_3); // ne
		suppression_accu->set_pixel(x+1,y, delta_3); // e
		suppression_accu->set_pixel(x+1,y+1, delta_3); // se
		suppression_accu->set_pixel(x,y, 0);
	      }
	      
	      if(curr < w && curr < nw && curr < sw) {
		sup_edge_image->set_pixel(x, y, 0);
		suppression_accu->set_pixel(x-1,y-1, delta_3); // nw
		suppression_accu->set_pixel(x-1,y, delta_3); // w
		suppression_accu->set_pixel(x-1,y+1, delta_3); // sw
		suppression_accu->set_pixel(x,y, 0);
	      }
	      if(curr < e) {
		sup_edge_image->set_pixel(x, y, 0);
		suppression_accu->set_pixel(x+1,y, delta); // e
		suppression_accu->set_pixel(x,y, 0);
	      }
	      
	      if(curr < w) {
		sup_edge_image->set_pixel(x, y, 0);
		suppression_accu->set_pixel(x-1,y, delta); // w
		suppression_accu->set_pixel(x,y, 0);
	      }
	      
	    }
	    
	    if(0.4142 < tangent && tangent <= 2.4142) { //  /
	      if(curr < ne && curr < n && curr < e) {
		sup_edge_image->set_pixel(x, y, 0);
		suppression_accu->set_pixel(x,y-1, delta_3); // n
		suppression_accu->set_pixel(x+1,y-1, delta_3); // ne
		suppression_accu->set_pixel(x+1,y, delta_3); // e
		suppression_accu->set_pixel(x,y, 0);
	      }
	      if(curr < sw && curr < s && curr < w) {
		sup_edge_image->set_pixel(x, y, 0);
		suppression_accu->set_pixel(x-1,y+1, delta_3); // sw
		suppression_accu->set_pixel(x,y+1, delta_3); // s
		suppression_accu->set_pixel(x-1,y, delta_3); // w
		suppression_accu->set_pixel(x,y, 0);
	      }
	      if(curr < ne) {
		sup_edge_image->set_pixel(x, y, 0);
		suppression_accu->set_pixel(x+1,y-1, delta); // ne
		suppression_accu->set_pixel(x,y, 0);
	      }
	      if(curr < sw) {
		sup_edge_image->set_pixel(x, y, 0);
		suppression_accu->set_pixel(x-1,y+1, delta); // sw
		suppression_accu->set_pixel(x,y, 0);
	      }
	    }
	    
	    if(fabs(tangent) > 2.4142) { //  |
	      
	      if(curr < n && curr < nw && curr < ne) {
		sup_edge_image->set_pixel(x, y, 0);
		suppression_accu->set_pixel(x,y-1, delta_3); // n
		suppression_accu->set_pixel(x-1,y-1, delta_3); // nw
		suppression_accu->set_pixel(x+1,y-1, delta_3); // ne
		suppression_accu->set_pixel(x, y, 0);
	      }
	      
	      if(curr < s && curr < sw && curr < se) {
		sup_edge_image->set_pixel(x, y, 0);
		suppression_accu->set_pixel(x,y+1, delta_3); // s
		suppression_accu->set_pixel(x-1,y+1, delta_3); // sw
		suppression_accu->set_pixel(x+1,y+1, delta_3); // se
		suppression_accu->set_pixel(x,y, 0);
	      }
	      if(curr < n) {
		sup_edge_image->set_pixel(x, y, 0);
		suppression_accu->set_pixel(x,y-1, delta); // n
		suppression_accu->set_pixel(x, y, 0);
	      }
	      
	      if(curr < s) {
		sup_edge_image->set_pixel(x, y, 0);
		suppression_accu->set_pixel(x,y+1, delta); // s
		suppression_accu->set_pixel(x,y, 0);
	      }
	    }
	    
	    
	    if(2.4142 < tangent && tangent <= -0.4142) { // 
	      if(curr < nw && curr < n && curr < w) {
		sup_edge_image->set_pixel(x, y, 0);
		suppression_accu->set_pixel(x-1,y-1, delta_3); // nw
		suppression_accu->set_pixel(x,y-1, delta_3); // n
		suppression_accu->set_pixel(x-1,y, delta_3); // w
		suppression_accu->set_pixel(x, y, 0);
	      }
	      
	      if(curr < se && curr < s && curr < e) {
		sup_edge_image->set_pixel(x, y, 0);
		suppression_accu->set_pixel(x+1,y+1, delta_3); // se
		suppression_accu->set_pixel(x,y+1, delta_3); // s 
		suppression_accu->set_pixel(x+1,y, delta_3); // e 
		suppression_accu->set_pixel(x, y, 0);
	      }

	      if(curr < nw) {
		sup_edge_image->set_pixel(x, y, 0);
		suppression_accu->set_pixel(x-1,y-1, delta); // nw
		suppression_accu->set_pixel(x, y, 0);
	      }
	      
	      if(curr < se) {
		sup_edge_image->set_pixel(x, y, 0);
		suppression_accu->set_pixel(x+1,y+1, delta); // se
		suppression_accu->set_pixel(x, y, 0);
	      }
	    }
	  }
	}
	

      }
      
      save_normalized_image<TempImage_GS_DOUBLE>(join_pathes(directory, "04_non_max_suppression.tif"), sup_edge_image);

      save_normalized_image<TempImage_GS_DOUBLE>(join_pathes(directory, "03.4_suppression_accu.tif"), suppression_accu);
      
      //suppression_accu->clear();
      
      for(y = 2; y < sup_edge_image->get_height() - 2; y++) {
	for(x = 2; x < sup_edge_image->get_width() - 2; x++) {
	  sup_edge_image->set_pixel(x, y, 
				    sup_edge_image->get_pixel(x, y) + 
				    suppression_accu->get_pixel(x,y));
	}
      }
      
      
      save_normalized_image<TempImage_GS_DOUBLE>(join_pathes(directory, "04_non_max_suppression_with_accu.tif"), sup_edge_image);
      
    }








    void fit_rectangles(TempImage_GS_DOUBLE_shptr img) {
      BoundingBox b(img->get_width(), img->get_height());
      QuadTree<BoundingBox> qt(b);

      for(unsigned int y = 1; y < img->get_height() - 1; y++) {
	std::cout << "fit for y = " << y << std::endl;
	for(unsigned int x = 1; x < img->get_width() - 1; x++) {

	  if(img->get_pixel(x, y) > 0 && img->get_pixel(x, y) <= 1) {

	    if(qt.region_iter_begin(x, x+1, y, y+1) == 
	       qt.region_iter_end()) {

	      
	      BoundingBox fitted_bb = fit_single_rectangle(img, initial_extension(img, x, y));

	      if(fitted_bb.get_width() >= wire_diameter / 2 &&
		 fitted_bb.get_height() >= wire_diameter / 2 &&
		 (fitted_bb.get_width() < 2 * wire_diameter  ||
		  fitted_bb.get_height() < 2 * wire_diameter ) &&
		 qt.region_iter_begin(fitted_bb) == qt.region_iter_end()
		 ) {
		
		draw_rect(img, fitted_bb);
		qt.insert(fitted_bb);
	      
	      }
	    }
	  }
	}
      }
    }

    BoundingBox fit_single_rectangle(TempImage_GS_DOUBLE_shptr img, BoundingBox bb) {

      double global_best_matching_value = -1.0 * img->get_width() * img->get_height();
      assert(global_best_matching_value < 0);
      BoundingBox global_best_bb = bb;
      int running_count = 3;

      std::list<BoundingBox> bb_list;

      
      int i, min_i = -2, max_i = 2;

      while(running_count != 0) {
	
	bb_list.push_back(global_best_bb);

	// extend for single direction

	for(i = min_i; i <= max_i; i++)
	  if(i != 0)
	    bb_list.push_back(BoundingBox(global_best_bb.get_min_x()+i,  
					  global_best_bb.get_max_x(), 
					  global_best_bb.get_min_y(),  
					  global_best_bb.get_max_y() ));
	
	
	for(i = min_i; i <= max_i; i++)
	  if(i != 0)
	    bb_list.push_back(BoundingBox(global_best_bb.get_min_x(),  
					  global_best_bb.get_max_x()+i, 
					  global_best_bb.get_min_y(),  
					  global_best_bb.get_max_y() ));
	

	for(i = min_i; i <= max_i; i++)
	  if(i != 0)
	    bb_list.push_back(BoundingBox(global_best_bb.get_min_x(),  
					  global_best_bb.get_max_x(), 
					  global_best_bb.get_min_y()+i,  
					  global_best_bb.get_max_y() ));
	
	for(i = min_i; i <= max_i; i++)
	  if(i != 0)
	    bb_list.push_back(BoundingBox(global_best_bb.get_min_x(),  
					  global_best_bb.get_max_x(), 
					  global_best_bb.get_min_y(),  
					  global_best_bb.get_max_y() +i));
	

	// expand in two directions

	for(i = min_i; i <= max_i; i++)
	  if(i != 0)
	    bb_list.push_back(BoundingBox(global_best_bb.get_min_x(),  
					  global_best_bb.get_max_x() + i, 
					  global_best_bb.get_min_y(),  
					  global_best_bb.get_max_y() + i));
	for(i = min_i; i <= max_i; i++)
	  if(i != 0)
	    bb_list.push_back(BoundingBox(global_best_bb.get_min_x() + i,  
					  global_best_bb.get_max_x(), 
					  global_best_bb.get_min_y() + i,  
					  global_best_bb.get_max_y() ));



	

	bool better_matching_found = false;

	for(std::list<BoundingBox>::iterator iter = bb_list.begin();
	    iter != bb_list.end(); ++iter) {
	  BoundingBox b = adjust_bounding_box(*iter);
	  
	  if(b.get_min_x() < 0) b.set_min_x(0);
	  if(b.get_max_x() < 0) b.set_max_x(0);
	  if(b.get_min_y() < 0) b.set_min_y(0);
	  if(b.get_max_y() < 0) b.set_max_y(0);

	  if(b.get_min_x() >= img->get_width()) b.set_min_x( img->get_width() -1);
	  if(b.get_max_x() >= img->get_width()) b.set_max_x(img->get_width() - 1);
	  if(b.get_min_y() >= img->get_height()) b.set_min_y(img->get_height() - 1);
	  if(b.get_max_y() >= img->get_height()) b.set_max_y(img->get_height() - 1);

	  double mq = calc_matching_quality(img, b);
#ifdef DEBUG_FITTING	  
	  std::cout << "Bounding box matches with " << mq << ": "
		    << b.to_string() << std::endl;
#endif	  

	  if(mq > global_best_matching_value) {
#ifdef DEBUG_FITTING
	    std::cout << "This is the best local fitting." << std::endl;
#endif
	    global_best_matching_value = mq;
	    global_best_bb = b;
	    running_count = 3;
	    better_matching_found = true;
	  }
	}

	if(!better_matching_found) running_count--;
#ifdef DEBUG_FITTING	  
	std::cout << std::endl;
#endif
	bb_list.clear();
      }
#ifdef DEBUG_FITTING	        
      std::cout << "best matching with " << global_best_matching_value << " is: "
		<< global_best_bb.to_string() << std::endl;
#endif      
      return global_best_bb;

    }

    double calc_matching_quality(TempImage_GS_DOUBLE_shptr img, BoundingBox const& bb) const {
      double penalty = -0.5;
      double award = 1;
      double p;

      double sum = 0;
      for(unsigned int x = bb.get_min_x(); x <= bb.get_max_x(); x++) {
	sum -=  1 - img->get_pixel(x, bb.get_min_y());

	if(bb.get_min_y() != bb.get_max_y())
	  sum -= 1 - img->get_pixel(x, bb.get_max_y());
      }

      for(unsigned int y = bb.get_min_y(); y <= bb.get_max_y(); y++) {
	sum -= 1 - img->get_pixel(bb.get_min_x(), y);

	if(bb.get_min_x() != bb.get_max_x())
	  sum -= 1 - img->get_pixel(bb.get_max_x(), y);
      }
      return sum;
    }


    BoundingBox initial_extension(TempImage_GS_DOUBLE_shptr img, unsigned int x, unsigned int y) const {

      unsigned int min_x = trace_h(img, x, y, -1);
      unsigned int max_x = trace_h(img, x, y, 1);

      unsigned int min_y = std::min(trace_v(img, min_x, y, -1),
				    std::min(trace_v(img, max_x, y, -1),
					     trace_v(img, x, y, -1)  ));

      unsigned int max_y = std::max(trace_v(img, min_x, y, 1),
				    std::min(trace_v(img, max_x, y, 1),
					     trace_v(img, x, y, 1) ));

      min_x = std::min(min_x, trace_h(img, max_x, max_y, -1));
      max_x = std::max(max_x, trace_h(img, min_x, max_y, 1));

      BoundingBox b(min_x, max_x, min_y, max_y);


      return adjust_bounding_box(b);
    }


    BoundingBox adjust_bounding_box(BoundingBox const& _b) const {

      BoundingBox b = _b;
      if(b.get_width() > b.get_height()) {
	if(b.get_height() < wire_diameter) {
	  b.set_max_y(min_y + wire_diameter);
	}
      }

      if(b.get_height() > b.get_width()) {  

	if(b.get_width() < wire_diameter) {
	  b.set_max_x(min_x + wire_diameter);
	}
      }
      return b;
    }



    unsigned int trace_h(TempImage_GS_DOUBLE_shptr img, 
			 unsigned int start_x, unsigned int y, 
			 int direction) const {

      if(y >= img->get_height() -1) return start_x;

      int state = 0;

      unsigned int x = start_x;
      bool done = false;

      while(!done && x < img->get_width() - 1 && x > 1) {

	while(img->get_pixel(x, y + state) > 0 && 
	      x < img->get_width() - 1 && x > 0) x += direction;

	if(state == 0) {
	  if(img->get_pixel(x, y - 1) > 0) state = -1;
	  else if(img->get_pixel(x, y + 1) > 0) state = 1;
	  else done = true;
	}
	else {
	  if(img->get_pixel(x, y) > 0) state = 0;
	  else done = true;
	}
	if(!done) x += direction;
      }
      return x;
    }

    unsigned int trace_v(TempImage_GS_DOUBLE_shptr img, 
			 unsigned int x, unsigned int start_y, 
			 int direction) const {

      if(x >= img->get_width() -1) return start_y;

      int state = 0;

      unsigned int y = start_y;
      bool done = false;

      while(!done && y < img->get_height() - 1 && y > 1) {

	while(img->get_pixel(x + state, y) > 0 && 
	      y < img->get_height() - 1 && y > 0) y += direction;

	if(state == 0) {
	  if(img->get_pixel(x - 1, y) > 0) state = -1;
	  else if(img->get_pixel(x + 1, y) > 0) state = 1;
	  else done = true;
	}
	else {
	  if(img->get_pixel(x, y) > 0) state = 0;
	  else done = true;
	}
	if(!done) y += direction;
      }
      return y;
    }


    void draw_rect(TempImage_GS_DOUBLE_shptr img, BoundingBox const& bb) {
      for(int x = bb.get_min_x(); x <= bb.get_max_x(); x++) {
	img->set_pixel(x, bb.get_min_y(), 2);
	img->set_pixel(x, bb.get_max_y(), 2);
      }
      for(unsigned int y = bb.get_min_y(); y <= bb.get_max_y(); y++) {
	img->set_pixel(bb.get_min_x(), y, 2);
	img->set_pixel(bb.get_max_x(), y, 2);
      }

    }

  };

}

#endif





