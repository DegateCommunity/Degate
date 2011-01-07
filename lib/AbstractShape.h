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

#ifndef __ABSTRACTSHAPE_H__
#define __ABSTRACTSHAPE_H__

#include <BoundingBox.h>
#include <ColoredObject.h>

namespace degate {

  /**
   * Represents an abstract shape.
   */
  class AbstractShape {


  public:


    /**
     * Check in the Point with coordinates x and y is within the area of the shape.
     */

    virtual bool in_shape(int x, int y, int max_distance = 0) const = 0;


    /**
     * Check if this shape is in the bounding box.
     * Note: it is somhow unclear if this 'in' means complete within or if an intersection is sufficient.
     */

    virtual bool in_bounding_box(BoundingBox const& bbox) const = 0;

    /**
     * Get the bounding box.
     */

    virtual BoundingBox const& get_bounding_box() const = 0;

    /**
     * Shift the shape vertically.
     * Note: If you store this shape in a QuadTree, you have to manage the changes in your QuadTree by yourself.
     */

    virtual void shift_x(int delta_x) = 0;

    /**
     * Shift the shape horizontally.
     * Note: If you store this shape in a QuadTree, you have to manage the changes in your QuadTree by yourself.
     */

    virtual void shift_y(int delta_y) = 0;


  };

}

#endif
