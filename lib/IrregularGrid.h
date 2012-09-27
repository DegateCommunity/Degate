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

#ifndef __IRREGULARGRID_H__
#define __IRREGULARGRID_H__

#include "Grid.h"

#include <memory>

namespace degate {

  /**
   * This class represents a grid type with non equidistant spacing between grid lines.
   */

  class IrregularGrid : public Grid {
  private:
    grid_set grid_offsets;

  public:

    /**
     * The ctor to construct a new irregular grid.
     */
    IrregularGrid(Grid::ORIENTATION orientation) : Grid(orientation) {}

    /**
     * The destructor.
     */
    virtual ~IrregularGrid() {}

    /**
     * Get an iterator to iterate over the offsets where the grid lines are.
     */
    virtual grid_iter begin() const { return grid_offsets.begin(); }

    /**
     * Get an end marker for the iteration over grid line offsets.
     */
    virtual grid_iter end() const { return grid_offsets.end(); }

    /**
     * Remove all grid lines.
     */
    virtual void clear() { grid_offsets.clear(); }

    /**
     * Add a new grid line starting at an given offset.
     */
    virtual void add_offset(int offset) {
      grid_offsets.push_back(offset);
      grid_offsets.sort();
    }

    /**
     * Remove a grid line that is placed at an offset.
     */
    virtual void remove_offset(int offset) {
      grid_offsets.remove(offset);
    }

    /**
     * Get the nearest offset where a grid line starts.
     */
    virtual int snap_to_grid(int pos) const;

  };

  typedef std::shared_ptr<IrregularGrid> IrregularGrid_shptr;
}

#endif
