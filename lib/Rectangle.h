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

#ifndef __RECTANGLE_H__
#define __RECTANGLE_H__

#include "BoundingBox.h"
#include "Shape.h"

namespace degate {

  class Rectangle : public AbstractShape {

  private:
    int min_x, max_x, min_y, max_y;

    BoundingBox bounding_box;

    void calculate_bounding_box();
    
  public:
    
    Rectangle();
    
    Rectangle(int min_x, int max_x, int min_y, int max_y);
    Rectangle(const Rectangle&);
    
    virtual ~Rectangle();
    
    virtual bool in_shape(int x, int y, int max_distance = 0) const;
    virtual bool in_bounding_box(BoundingBox const& bbox) const;
    virtual BoundingBox const& get_bounding_box() const;
    
    virtual bool operator==(const Rectangle& other) const;
    virtual bool operator!=(const Rectangle& other) const;
    
    virtual bool intersects(Rectangle const & rect) const;
    virtual bool complete_within(Rectangle const & rect) const;
    
    virtual unsigned int get_width() const;
    virtual unsigned int get_height() const;
    
    virtual int get_min_x() const;
    virtual int get_max_x() const;
    virtual int get_min_y() const;
    virtual int get_max_y() const;
    
    virtual int get_center_x() const;
    virtual int get_center_y() const;
    
    virtual void set_min_x(int min_x);
    virtual void set_min_y(int min_y);
    virtual void set_max_x(int max_x);
    virtual void set_max_y(int max_y);
    
    virtual void set_position(int min_x, int max_x, int min_y, int max_y);
    
    virtual void shift_x(int delta_x);
    virtual void shift_y(int delta_y);

  };

}

#endif
