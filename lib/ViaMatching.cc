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
#include <boost/foreach.hpp>

using namespace degate;

ViaMatching::ViaMatching() : 

  median_filter_width(3), 
  sigma(0.5) {
}


void ViaMatching::init(BoundingBox const& bounding_box, Project_shptr project) 
  throw(InvalidPointerException, DegateRuntimeException) {
  
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
  EdgeDetection ed(bounding_box.get_min_x(),
		   bounding_box.get_max_x(),
		   bounding_box.get_min_y(),
		   bounding_box.get_max_y(),
		   median_filter_width,
		   sigma > 0 ? 10 : 0,
		   sigma);

  ed.run_edge_detection(img);

  TileImage_GS_DOUBLE_shptr i = ed.get_edge_image(TileImage_GS_DOUBLE_shptr());

  assert(i != NULL);

}

