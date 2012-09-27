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

#ifndef __EMARKER_H__
#define __EMARKER_H__

#include "globals.h"
#include "LogicModelObjectBase.h"
#include "Net.h"
#include "Layer.h"
#include "LogicModel.h"
#include "ConnectedLogicModelObject.h"
#include "degate_exceptions.h"
#include "Circle.h"
#include <RemoteObject.h>

#include <memory>

namespace degate {

  /**
   * Representation of an electrically connectable marker object.
   */
  class EMarker : public Circle, public ConnectedLogicModelObject, public RemoteObject {


  public:

    explicit EMarker() {};
    
    /**
     * Constructor for a via object.
     */

    EMarker(int _x, int _y, diameter_t _diameter = 5);

    /**
     * Destructor for a via object.
     */

    virtual ~EMarker();

    //@{
    DeepCopyable_shptr cloneShallow() const;
    void cloneDeepInto(DeepCopyable_shptr destination, oldnew_t *oldnew) const;
    //@}

    /**
     * Get a human readable string that describes the whole
     * logic model object. The string should be unique in order
     * to let the user identify the concrete object. But that
     * is not a must.
     */

    virtual const std::string get_descriptive_identifier() const;

    /**
     * Get a human readable string that names the object type.
     * Here it is "EMarker".
     */

    virtual const std::string get_object_type_name() const;

    /**
     * Print the object.
     */

    void print(std::ostream & os, int n_tabs) const;


    void shift_x(int delta_x);
    void shift_y(int delta_y);
    void set_x(int x);
    void set_y(int y);
    void set_diameter(unsigned int diameter);

    virtual bool in_bounding_box(BoundingBox const& bbox) const {
      return Circle::in_bounding_box(bbox);
    }

    virtual BoundingBox const& get_bounding_box() const {
      return Circle::get_bounding_box();
    }

    virtual bool in_shape(int x, int y, int max_distance = 0) const {
      return Circle::in_shape(x, y, max_distance);
    }

  protected:

    virtual object_id_t push_object_to_server(std::string const& server_url);

  };


}

#endif
