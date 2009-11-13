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

#ifndef __CIRCLE_H__
#define __CIRCLE_H__

#include "Shape.h"
#include "BoundingBox.h"

namespace degate {

  class Circle : public AbstractShape {

  private:
    int x, y;
    unsigned int diameter;

    BoundingBox bounding_box;

    void calculate_bounding_box();
    
  public:
    
    Circle();
    Circle(int x, int y, unsigned int diameter);
    
    virtual ~Circle() {}
    
    virtual bool in_shape(int x, int y) const;
    
    virtual bool in_bounding_box(BoundingBox const& bbox) const;
    virtual BoundingBox const& get_bounding_box() const;

    virtual bool operator==(const Circle& other) const;
    virtual bool operator!=(const Circle& other) const;
    
    virtual int get_x() const;
    virtual int get_y() const;
    virtual unsigned int get_diameter() const;
    
    virtual void set_x(int x);
    virtual void set_y(int y);
    virtual void set_diameter(unsigned int diameter);
    
    virtual void shift_x(int delta_x);
    virtual void shift_y(int delta_y);
    
  };

}

#endif
