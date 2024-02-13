/**
 * This file is part of the IC reverse engineering tool Degate.
 *
 * Copyright 2008, 2009, 2010 by Martin Schobert
 * Copyright 2019-2020 Dorian Bachelot
 *
 * Degate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * Degate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with degate. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#define _CRT_SECURE_NO_WARNINGS 1
#include "Core/Matching/WireMatching.h"
#include "Core/Matching/ZeroCrossingEdgeDetection.h"
#include "Core/Matching/CannyEdgeDetection.h"
#include "Core/Matching/BinaryLineDetection.h"
#include "Core/Primitive/BoundingBox.h"
#include "Core/Matching/LineSegmentExtraction.h"
#include "Core/Image/Manipulation/MedianFilter.h"

using namespace degate;

WireMatching::WireMatching() :
    wire_diameter(5),
    median_filter_width(3),
    sigma(0.5),
    min_edge_magnitude(0.25)
{
}


void WireMatching::init(BoundingBox const& bounding_box, Project_shptr project)
{
    this->bounding_box = bounding_box;

    if (project == nullptr)
        throw InvalidPointerException("Invalid pointer for parameter project.");

    lmodel = project->get_logic_model();
    assert(lmodel != nullptr); // always has a logic model

    layer = lmodel->get_current_layer();
    if (layer == nullptr) throw DegateRuntimeException("No current layer in project.");


    ScalingManager_shptr sm = layer->get_scaling_manager();
    assert(sm != nullptr);

    img = sm->get_image(1).second;
    assert(img != nullptr);
}


void WireMatching::set_wire_diameter(unsigned int wire_diameter)
{
    this->wire_diameter = wire_diameter;
}

void WireMatching::set_median_filter_width(unsigned int median_filter_width)
{
    this->median_filter_width = median_filter_width;
}

void WireMatching::set_sigma(double sigma)
{
    this->sigma = sigma;
}

void WireMatching::set_min_edge_magnitude(double min_edge_magnitude)
{
    this->min_edge_magnitude = min_edge_magnitude;
}

void WireMatching::run()
{
    auto directory = create_temp_directory();

    ZeroCrossingEdgeDetection ed(bounding_box.get_min_x(),
                                 bounding_box.get_max_x(),
                                 bounding_box.get_min_y(),
                                 bounding_box.get_max_y(),
                                 median_filter_width,
                                 sigma > 0 ? 10 : 0,
                                 sigma,
                                 wire_diameter >> 1,
                                 wire_diameter + (wire_diameter >> 1),
                                 min_edge_magnitude, 0.5);

    TileImage_GS_DOUBLE_shptr i = ed.run(img, TileImage_GS_DOUBLE_shptr(), directory);
    assert(i != nullptr);

    LineSegmentExtraction<TileImage_GS_DOUBLE> extraction(i, wire_diameter / 2, 2, ed.get_border());
    LineSegmentMap_shptr line_segments = extraction.run();
    assert(line_segments != nullptr);

    assert(lmodel != nullptr);
    assert(layer != nullptr);

    for (auto ls : *line_segments)
    {
        debug(TM, "found wire");
        Wire_shptr w(new Wire(bounding_box.get_min_x() + ls->get_from_x(),
                              bounding_box.get_min_y() + ls->get_from_y(),
                              bounding_box.get_min_x() + ls->get_to_x(),
                              bounding_box.get_min_y() + ls->get_to_y(),
                              wire_diameter));

        lmodel->add_object(layer->get_layer_pos(), w);
    }

    remove_directory(directory);
}
