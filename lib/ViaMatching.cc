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

  // lists for images of vias on the current layer
  std::list<MemoryImage_shptr> vias_up, vias_down;

  // iterate over placed vias and calculate a mean-image
  for(LogicModel::via_collection::iterator viter = lmodel->vias_begin();
      viter != lmodel->vias_end(); ++viter) {
    Via_shptr via = viter->second;

    if(via->get_layer() == layer) {
      
      // get bounding box
      BoundingBox const& bb = via->get_bounding_box();

      // grab via's image
      MemoryImage_shptr img = grab_image<MemoryImage>(lmodel, layer, bb);
      assert(img != NULL);
      
      // insert image into one of the lists
      if(via->get_direction() == Via::DIRECTION_UP)
	vias_up.push_back(img);
      else if(via->get_direction() == Via::DIRECTION_DOWN)
	vias_down.push_back(img);
    }
  }

  // calculate the mean-image
  MemoryImage_shptr via_up = merge_images(vias_up);
  MemoryImage_shptr via_down = merge_images(vias_down);

  
  // ...
}

