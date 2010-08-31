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

#ifndef __WIRE_H__
#define __WIRE_H__

#include <tr1/memory>

#include <globals.h>
#include <LogicModelObjectBase.h>
#include <ConnectedLogicModelObject.h>
#include <Line.h>
#include <RemoteObject.h>

namespace degate {

  /**
   * Represents a wire.
   */
  class Wire : public Line, public ConnectedLogicModelObject, public RemoteObject {
	
  public:

    /**
     * The constructor for a wire object.
     */
    Wire(int _from_x, int _from_y, int _to_x, int _to_y, diameter_t _diameter);
  
    /**
     * Destructor for a wire object.
     */
    virtual ~Wire() {}

    /**
     * Get a human readable string that describes the whole
     * logic model object. The string should be unique in order
     * to let the user identify the concrete object. But that
     * is not a must.
     */
    
    virtual const std::string get_descriptive_identifier() const;

    /**
     * Get a human readable string that names the object type.
     * Here it is "Wire".
     */

    virtual const std::string get_object_type_name() const;

    /**
     * Print wire.
     */
    void print(std::ostream & os = std::cout, int n_tabs = 0) const;





    void shift_x(int delta_x) {
      Line::shift_x(delta_x);
      notify_shape_change();
    }

    void shift_y(int delta_y) {
      Line::shift_y(delta_y);
      notify_shape_change();
    }

    virtual bool in_bounding_box(BoundingBox const& bbox) const {
      return in_bounding_box(bbox);
    }

    virtual BoundingBox const& get_bounding_box() const {
      return Line::get_bounding_box();
    }

    virtual bool in_shape(int x, int y) const {
      return Line::in_shape(x, y);
    }

  protected:

    virtual object_id_t push_object_to_server(std::string const& server_url);

  };


}

#endif
