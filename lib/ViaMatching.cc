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

#include <ViaMatching.h>
#include <BoundingBox.h>
#include <MedianFilter.h>
#include <EdgeDetection.h>
#include <LogicModelHelper.h>
#include <boost/foreach.hpp>

using namespace degate;

ViaMatching::ViaMatching() :

  median_filter_width(3),
  sigma(0.5) {
}


void ViaMatching::init(BoundingBox const& bounding_box, Project_shptr project) {

  this->bounding_box = bounding_box;

  if(project == NULL)
    throw InvalidPointerException("Invalid pointer for parameter project.");

  lmodel = project->get_logic_model();
  assert(lmodel != NULL); // always has a logic model

  layer = lmodel->get_current_layer();
  if(layer == NULL)
    throw DegateRuntimeException("No current layer in project.");


  ScalingManager_shptr sm = layer->get_scaling_manager();
  assert(sm != NULL);

  img = sm->get_image(1).second;
  assert(img != NULL);
}


void ViaMatching::set_median_filter_width(unsigned int median_filter_width) {
  this->median_filter_width = median_filter_width;
}

void ViaMatching::set_sigma(double sigma) {
  this->sigma = sigma;
}


void ViaMatching::run() {

  unsigned int max_r = 0;

  // lists for images of vias on the current layer
  std::list<MemoryImage_shptr> vias_up, vias_down;

  // iterate over all placed vias (current layer) and determine their size
  for(LogicModel::via_collection::iterator viter = lmodel->vias_begin();
      viter != lmodel->vias_end(); ++viter) {
    Via_shptr via = viter->second;
    if(via->get_layer() == layer && via->get_diameter() > max_r) 
      max_r = via->get_diameter();
  }
  
  debug(TM, "via matching: max diameter for image averaging: %d", max_r);
  
  int max_count_up = 1, max_count_down = 1;
  max_r = (max_r + 1) / 2;

  // iterate over all placed vias (current layer) and calculate a mean-image
  for(LogicModel::via_collection::iterator viter = lmodel->vias_begin();
      viter != lmodel->vias_end(); ++viter) {
    Via_shptr via = viter->second;

    if(via->get_layer() == layer) {
      
      // calculate new bounding box
      BoundingBox bb(via->get_x() - max_r, via->get_x() + max_r,
		     via->get_y() - max_r, via->get_y() + max_r);

      if(layer->get_bounding_box().complete_within(bb)) {

	// grab via's image
	MemoryImage_shptr img = grab_image<MemoryImage>(lmodel, layer, bb);
	assert(img != NULL);
	
	// insert image into one of the lists
	if(via->get_direction() == Via::DIRECTION_UP && max_count_up-- > 0) {
	  vias_up.push_back(img);
	}
	else if(via->get_direction() == Via::DIRECTION_DOWN && max_count_down-- > 0)
	  vias_down.push_back(img);
      }
      else debug(TM, "via out of region");
    }
  }

  debug(TM, "via matching: size of vias_down=%d vias_up=%d", vias_down.size(), vias_up.size());

  // calculate the mean-image
  MemoryImage_shptr via_up = merge_images(vias_up);
  MemoryImage_shptr via_down = merge_images(vias_down);

  // save images for debugging
  if(via_up) save_image(join_pathes("/tmp", "01_via_up_merged.tif"), via_up);
  if(via_down) save_image(join_pathes("/tmp", "01_via_down_merged.tif"), via_down);

  // convert to greyscale
  MemoryImage_GS_BYTE_shptr via_up_gs, via_down_gs;

  if(via_up) {
    via_up_gs = MemoryImage_GS_BYTE_shptr(new MemoryImage_GS_BYTE(via_up->get_width(), via_up->get_height()));
    copy_image(via_up_gs, via_up);
  }

  if(via_down) {
    via_down_gs = MemoryImage_GS_BYTE_shptr(new MemoryImage_GS_BYTE(via_down->get_width(), via_down->get_height()));
    copy_image(via_down_gs, via_down);
  }

  // save images for debugging
  if(via_up_gs) save_image(join_pathes("/tmp", "02_via_up_gs.tif"), via_up_gs);
  if(via_down_gs) save_image(join_pathes("/tmp", "02_via_down_gs.tif"), via_down_gs);


  if(via_up_gs) scan(bounding_box, img, via_up_gs, Via::DIRECTION_UP);
  if(via_down_gs) scan(bounding_box, img, via_down_gs, Via::DIRECTION_DOWN);

  // ...
}

template<class BGImageType, class TemplateImageType>
double calc_xcorr(unsigned int start_x, unsigned int start_y,
		  std::tr1::shared_ptr<BGImageType> bg_img, double f_avg, double sigma_f,
		  std::tr1::shared_ptr<TemplateImageType> tmpl_img, double t_avg, double sigma_t) {

  double sum = 0;
  double n = tmpl_img->get_width() * tmpl_img->get_height();

  for(unsigned int y = 0; y < tmpl_img->get_height(); y++) {
    for(unsigned int x = 0; x < tmpl_img->get_width(); x++) {
      double f_xy = bg_img->get_pixel_as<double>(start_x + x, start_y + y);
      double t_xy = tmpl_img->get_pixel_as<double>(x, y);
      
      sum += (f_xy - f_avg) * (t_xy - t_avg) / (sigma_f * sigma_t); // extract commons
    }
  }

  sum *= 1/(n - 1);


  return sum;
}

void ViaMatching::scan(BoundingBox const& bbox, BackgroundImage_shptr bg_img,
		       MemoryImage_GS_BYTE_shptr tmpl_img, Via::DIRECTION direction) {

  debug(TM, "run scanning");
  double highest_xcorr = 0;
  double f_avg, sigma_f;
  double t_avg, sigma_t;
  average_and_stddev(tmpl_img, 0, 0, 
		     tmpl_img->get_width(), tmpl_img->get_height(), 
		     &t_avg, &sigma_t);

  int max_x = bbox.get_max_x() > tmpl_img->get_width() ? bbox.get_max_x() - tmpl_img->get_width() : bbox.get_min_x();
  int max_y = bbox.get_max_y() > tmpl_img->get_height() ? bbox.get_max_y() - tmpl_img->get_height() : bbox.get_min_y();

  for(int y = bbox.get_min_y(); y < max_y; y++) {
    for(int x = bbox.get_min_x(); x < max_x; x++) {

      average_and_stddev(bg_img, x, y, 
			 tmpl_img->get_width(), tmpl_img->get_height(), 
			 &f_avg, &sigma_f);

      double xcorr = calc_xcorr(x, y,
				bg_img, f_avg, sigma_f,
				tmpl_img, t_avg, sigma_t);

      if(xcorr > 0.01) {
	debug(TM, "%d,%d -> %f sigma-f=%f sigma-t=%f f_avg=%f t_avg=%f", x, y, xcorr, sigma_f,sigma_t, f_avg, sigma_f);
	printf("xcorr = %f\n", xcorr);
      }
      if(xcorr > highest_xcorr) highest_xcorr = xcorr;
    }
  }

  debug(TM, "highest xcorr=%f", highest_xcorr);
}

