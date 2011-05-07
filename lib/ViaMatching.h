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

#ifndef __VIAMATCHING_H__
#define __VIAMATCHING_H__

#include <Image.h>
#include <Project.h>
#include <TemplateMatching.h>
#include <Via.h>

namespace degate {

  class ViaMatching : public Matching {

  private:

    Layer_shptr layer;
    LogicModel_shptr lmodel;
    unsigned int median_filter_width;
    double sigma;
    BackgroundImage_shptr img;

    BoundingBox bounding_box;

  public:

    ViaMatching();

    /**
     * @exception InvalidPointerException This exception is thrown, if \p
     *   project is an invalid pointer.
     * @exception DegateRuntimeException This exception is thrown, if there
     *   is no current layer in the logic model. This should not happen.
     */
    virtual void init(BoundingBox const& bounding_box, Project_shptr project);

    virtual void run();

    void set_median_filter_width(unsigned int median_filter_width);
    void set_sigma(double sigma);

  private:
    void scan(BoundingBox const& bbox, BackgroundImage_shptr bg_img, 
	      MemoryImage_GS_BYTE_shptr tmpl_img, Via::DIRECTION direction);
  };

  typedef std::tr1::shared_ptr<ViaMatching> ViaMatching_shptr;
}

#endif
