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

#ifndef __GRID_H__
#define __GRID_H__

#include <list>
#include <tr1/memory>

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
      UNDEFINED = 0,
      HORIZONTAL = 1,
      VERTICAL = 2
    };

  private:

    ORIENTATION orientation;
    bool enabled;

  public:
    typedef std::list<int> grid_set;
    typedef grid_set::const_iterator grid_iter;

    /**
     * Create a new grid.
     * @param _orientation This parameter defines the orientation. E.g. a horizontal
     *   grid that defines horizontal spacings. This means that lines are vertical.
     * @param _enabled You can enable/disable grids. With this parameter you can
     *   control whether the grid is enabled od disabled.
     */

    Grid(ORIENTATION _orientation, bool _enabled = false) :
      orientation(_orientation),
      enabled(_enabled) {}

    /**
     * The destructor.
     */

    virtual ~Grid() {}

    /**
     * Get an iterator to iterate over grid offsets.
     */

    virtual grid_iter begin() const = 0;

    /**
     * Get an end marker for ther iteration.
     */

    virtual grid_iter end() const = 0;

    /**
     * Clear the grid.
     */

    virtual void clear() = 0;

    /**
     * Set the state, whether a grid is enabled or not.
     */

    virtual void set_enabled(bool state = true) { enabled = state; }

    /**
     * Check the state, whether a grid is enabled or not.
     */

    virtual bool is_enabled() const { return enabled; }

    /**
     * Get the nearest offset, that is on grid.
     */

    virtual int snap_to_grid(int pos) const = 0;

    /**
     * Get the grid orientation.
     */

    virtual ORIENTATION get_orientation() const { return orientation; }

    /**
     * Check grid orientation: this is if the spacing is horizontal (vertical lines).
     */

    virtual bool is_horizontal() const { return orientation == HORIZONTAL; }

    /**
     * Check grid orientation: this is if the spacing is vertical (horizontal lines.)
     */

    virtual bool is_vertical() const { return orientation == VERTICAL; }

  };

  typedef std::tr1::shared_ptr<Grid> Grid_shptr;
}

#endif
