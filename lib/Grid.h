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

#ifndef __GRID_H__
#define __GRID_H__

#include <list>

namespace degate {

  /**
   * Base class for grid types.
   */
  class Grid {

  public:

    /**
     * Enum to declare the type of a grid.
     */
    
    enum ORIENTATION {
      HORIZONTAL = 1,
      VERTICAL = 2
    };

  private:

    ORIENTATION orientation;
    bool enabled;

  public:
    typedef std::list<int> grid_set;
    typedef grid_set::const_iterator grid_iter;
    
    Grid(ORIENTATION _orientation, bool _enabled = false) : 
      orientation(_orientation), 
      enabled(_enabled) {}

    virtual ~Grid() {}

    virtual grid_iter begin() const = 0;
    virtual grid_iter end() const = 0;
    virtual void clear() = 0;
    
    virtual void set_enabled(bool state = true) { enabled = state; }
    virtual bool is_enabled() const { return enabled; }
    
    virtual int snap_to_grid(int pos) const = 0;

    virtual ORIENTATION get_orientation() const { return orientation; }
    virtual bool is_horizontal() const { return orientation == HORIZONTAL; }
    virtual bool is_vertical() const { return orientation == VERTICAL; }
  };

}

#endif
