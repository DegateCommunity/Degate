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

#ifndef __LINE_H__
#define __LINE_H__

#include <BoundingBox.h>
#include <Shape.h>
#include <Point.h>

namespace degate {

  class Line : public AbstractShape {
	
  private:
    int from_x, from_y, to_x, to_y;
    unsigned int diameter;
    
    double d_x, d_y;
	
    BoundingBox bounding_box;

  private:

    /**
     * Recalulate the bounding box of a line.
     * If the line is either horizontally nor vertically
     * aligned, the bounding box corresponds exactly to
     * the area, the line spans. If the line is horizontal
     * or vertical, the bounding box also depends on the
     * diameter.
     */
    void calculate_bounding_box();

  public:
	
    Line();
    Line(int from_x, int from_y, int to_x, int to_y, unsigned int diameter);
	
    virtual ~Line() {}
	
    virtual bool in_shape(int x, int y) const;
    virtual bool in_bounding_box(BoundingBox const& bbox) const;
    virtual BoundingBox const& get_bounding_box() const;

	
    virtual int get_from_x() const;
    virtual int get_to_x() const;
    virtual int get_from_y() const;
    virtual int get_to_y() const;
	
    virtual void set_from_x(int min_x);
    virtual void set_from_y(int min_y);
    virtual void set_to_x(int max_x);
    virtual void set_to_y(int max_y);
	
    virtual void shift_x(int delta_x);
    virtual void shift_y(int delta_y);
	
    virtual unsigned int get_diameter() const;
    virtual void set_diameter(unsigned int diameter);
	
    virtual bool is_vertical() const;
    virtual bool is_horizontal() const;

    virtual unsigned int get_length() const;

    virtual Point get_p1() const;
    virtual Point get_p2() const;

    virtual void set_p1(Point const& p);
    virtual void set_p2(Point const& p);

  };

}

#endif
