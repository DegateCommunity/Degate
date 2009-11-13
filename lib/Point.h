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

#ifndef __POINT_H__
#define __POINT_H__

#include "Shape.h"

namespace degate {

  class Point {

  private:
    int x, y;
    
  public:
    
    Point();
    Point(int x, int y);
    
    bool operator==(const Point& other) const;
    bool operator!=(const Point& other) const;
    
    int get_x() const;
    int get_y() const;
    
    void set_x(int x);
    void set_y(int y);
    
    void shift_x(int delta_x);
    void shift_y(int delta_y);

  };

}

#endif
