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

#include <WireMatching.h>
#include <ZeroCrossingEdgeDetection.h>
#include <CannyEdgeDetection.h>
#include <BinaryLineDetection.h>
#include <BoundingBox.h>
#include <LineSegmentExtraction.h>
#include <MedianFilter.h>
#include <boost/foreach.hpp>

using namespace degate;

WireMatching::WireMatching() :
  wire_diameter(5),
  median_filter_width(3),
  sigma(0.5),
  min_edge_magnitude(0.25) {
}


void WireMatching::init(BoundingBox const& bounding_box, Project_shptr project) {

  this->bounding_box = bounding_box;

  if(project == NULL)
    throw InvalidPointerException("Invalid pointer for parameter project.");

  lmodel = project->get_logic_model();
  assert(lmodel != NULL); // always has a logic model

  layer = lmodel->get_current_layer();
  if(layer == NULL) throw DegateRuntimeException("No current layer in project.");


  ScalingManager_shptr sm = layer->get_scaling_manager();
  assert(sm != NULL);

  img = sm->get_image(1).second;
  assert(img != NULL);
}


void WireMatching::set_wire_diameter(unsigned int wire_diameter) {
  this->wire_diameter = wire_diameter;
}

void WireMatching::set_median_filter_width(unsigned int median_filter_width) {
  this->median_filter_width = median_filter_width;
}

void WireMatching::set_sigma(double sigma) {
  this->sigma = sigma;
}

void WireMatching::set_min_edge_magnitude(double min_edge_magnitude) {
  this->min_edge_magnitude = min_edge_magnitude;
}

void WireMatching::run() {

//  ZeroCrossingEdgeDetection ed(bounding_box.get_min_x(),
//			       bounding_box.get_max_x(),
//			       bounding_box.get_min_y(),
//			       bounding_box.get_max_y(),
//			       median_filter_width,
//			       sigma > 0 ? 10 : 0,
//			       sigma,
//			       wire_diameter >> 1,
//			       wire_diameter + (wire_diameter >> 1),
//			       min_edge_magnitude, 0.5);
  CannyEdgeDetection ed(bounding_box.get_min_x(),
			bounding_box.get_max_x(),
			bounding_box.get_min_y(),
			bounding_box.get_max_y(),
			wire_diameter,
			median_filter_width,
			sigma > 0? 10 : 0,
			sigma, min_edge_magnitude, 0.5);
  TileImage_GS_DOUBLE_shptr i = ed.run(img, TileImage_GS_DOUBLE_shptr(), "/tmp");
  assert(i != NULL);

  BinaryLineDetection test(bounding_box.get_min_x(),
			   bounding_box.get_max_x(),
			   bounding_box.get_min_y(),
			   bounding_box.get_max_y(),
			   wire_diameter,
			   median_filter_width,
			   sigma > 0 ? 10 : 0,
			   sigma);
  TileImage_GS_DOUBLE_shptr j = test.run(img, TileImage_GS_DOUBLE_shptr(), "/tmp");
  assert(j != NULL);
  save_normalized_image<TileImage_GS_DOUBLE>(join_pathes(test.get_directory(), "line.tif"), j);

  LineSegmentExtraction<TileImage_GS_DOUBLE> extraction(i, wire_diameter/2, 2, ed.get_border());
  LineSegmentMap_shptr line_segments = extraction.run();
  assert(line_segments != NULL);

  assert(lmodel != NULL);
  assert(layer != NULL);

  BOOST_FOREACH(LineSegment_shptr ls, *line_segments) {
    debug(TM, "found  wire");
    Wire_shptr w(new Wire(bounding_box.get_min_x() + ls->get_from_x(),
			  bounding_box.get_min_y() + ls->get_from_y(),
			  bounding_box.get_min_x() + ls->get_to_x(),
			  bounding_box.get_min_y() + ls->get_to_y(),
			  wire_diameter));

    lmodel->add_object(layer->get_layer_pos(), w);
  }

}

