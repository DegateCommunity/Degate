/* -*-c++-*-

 This file is part of the IC reverse engineering tool degate.

 Copyright 2008, 2009, 2010 by Martin Schobert
 Copyright 2012 Robert Nitsch

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

#ifndef __COLOREDOBJECT_H__
#define __COLOREDOBJECT_H__

#include "DeepCopyable.h"

namespace degate {

  /**
   * Represents an object that has a frame and a fill color.
   */

  class ColoredObject : public DeepCopyableBase {
  private:
    color_t fill_color;
    color_t frame_color;

  public:
    ColoredObject() : fill_color(0), frame_color(0) {}
    virtual ~ColoredObject() {}

    //@{
    void cloneDeepInto(DeepCopyable_shptr dest, oldnew_t *oldnew) const {
      auto clone = std::dynamic_pointer_cast<ColoredObject>(dest);
      clone->fill_color = fill_color;
      clone->frame_color = frame_color;
    };
    //@}
    
    /**
     * Is there a frame color definition.
     */
    virtual bool has_frame_color() const { return frame_color != 0; }

    /**
     * Is there a fill color definition.
     */
    virtual bool has_fill_color() const { return fill_color != 0; }

    /**
     * Get the frame color.
     */
    virtual color_t get_frame_color() const { return frame_color; }

    /**
     * Get the fill color.
     */
    virtual color_t get_fill_color() const { return fill_color; }

    /**
     * Set the frame color.
     */

    virtual void set_frame_color(color_t c) { frame_color = c; }

    /**
     * Set the fill color.
     */
    virtual void set_fill_color(color_t c) { fill_color = c; }

  };

}

#endif
