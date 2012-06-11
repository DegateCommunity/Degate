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

#ifndef __WIREMATCHING_H__
#define __WIREMATCHING_H__

#include <Image.h>
#include <Project.h>
#include <TemplateMatching.h>

namespace degate {

  class WireMatching : public Matching {

  private:

    Layer_shptr layer;
    LogicModel_shptr lmodel;
    unsigned int wire_diameter, median_filter_width;
    double sigma, min_edge_magnitude;
    BackgroundImage_shptr img;

    BoundingBox bounding_box;

  public:

    WireMatching();

    /**
     * @exception InvalidPointerException
     * @exception DegateRuntimeException
     */
    virtual void init(BoundingBox const& bounding_box, Project_shptr project);

    virtual void run();

    void set_wire_diameter(unsigned int wire_diameter);
    void set_median_filter_width(unsigned int median_filter_width);
    void set_sigma(double sigma);
    void set_min_edge_magnitude(double min_edge_magnitude);

  };

  typedef std::shared_ptr<WireMatching> WireMatching_shptr;
}

#endif
