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

#ifndef __PLACEDLOGICMODELOBJECT_H__
#define __PLACEDLOGICMODELOBJECT_H__

#include <globals.h>
#include <LogicModelObjectBase.h>
#include <Layer.h>
#include <Shape.h>

namespace degate {

  /**
   * Represents a placeable object.
   *
   * Any class that represents a somewhere placeable objects should inherit
   * from that base class. The term "placed objects" is related to real
   * existent physical structures on the chip surface
   * (e.g. wire, vias, ...) as well as logical objects (e.g. annotations).
   */

  class PlacedLogicModelObject : public AbstractShape,
				 public LogicModelObjectBase,
				 public ColoredObject {

  public:

    enum HIGHLIGHTING_STATE {
      HLIGHTSTATE_NOT = 0,
      HLIGHTSTATE_DIRECT = 1,
      HLIGHTSTATE_ADJACENT = 2
    };

  private:

    HIGHLIGHTING_STATE highlight_state;
    std::shared_ptr<Layer> layer;

  protected:

    /**
     * Ajust the position in the quadtree.
     */

    void notify_shape_change();

  public:

    /**
     * The constructor.
     */

    PlacedLogicModelObject();

    /**
     * The destructor.
     */

    virtual ~PlacedLogicModelObject();

    /**
     * A placed object is highlightable. You can ask for its
     * state with this method.
     */

    virtual HIGHLIGHTING_STATE get_highlighted() const;

    /**
     * Check if an object is highlighted at all.
     */
    virtual bool is_highlighted() const;

    /**
     * Set the selection state.
     */

    virtual void set_highlighted(HIGHLIGHTING_STATE state);

    /**
     * Set the layer on which the object is placed.
     */

    virtual void set_layer(std::shared_ptr<Layer> layer);

    /**
     * Get the layer on which the object is placed.
     */

    virtual std::shared_ptr<Layer> get_layer();


    /**
     * Print the object.
     */

    virtual void print(std::ostream & os = std::cout, int n_tabs = 0) const = 0;

  };

}

#endif
