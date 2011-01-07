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

#ifndef __GATE_H__
#define __GATE_H__

#include <tr1/memory>

#include <degate.h>
#include <Layer.h>

#include "GatePort.h"
#include "PlacedLogicModelObject.h"
#include "Rectangle.h"
#include "GateTemplate.h"
#include "GateTemplatePort.h"
#include <set>

namespace degate {

  /**
   * The class Gate defines and implements a physically placed gate.
   * @todo The template type id makes only sense, if ports have the
   *  same thing with remplate ports. Maybe removing this is an option, because there are
   *  easier ways to relaod the gate library.
   */

  class Gate : public Rectangle, public PlacedLogicModelObject {

  public:

    enum ORIENTATION {
      ORIENTATION_UNDEFINED = 0,
      ORIENTATION_NORMAL = 1,
      ORIENTATION_FLIPPED_UP_DOWN = 2,
      ORIENTATION_FLIPPED_LEFT_RIGHT = 3,
      ORIENTATION_FLIPPED_BOTH = 4
    };

    typedef std::set< GatePort_shptr >::iterator port_iterator;
    typedef std::set< GatePort_shptr >::const_iterator port_const_iterator;

  private:

    GateTemplate_shptr gate_template;

    std::set<GatePort_shptr> gate_ports;

    ORIENTATION orientation;

    object_id_t template_type_id;

  public:


    /**
     * Create a gate.
     */
    Gate(int _min_x, int _max_x, int _min_y, int _max_y,
	 ORIENTATION _orientation = ORIENTATION_UNDEFINED);

    /**
     * Create a gate.
     */

    Gate(BoundingBox const& bounding_box,
	 ORIENTATION _orientation = ORIENTATION_UNDEFINED);


    /**
     * Destroy a gate.
     */

    virtual ~Gate();

    /**
     * Add a gate port.
     *
     * If you add a gate port to a gate you have to make some changes to the logic model.
     * Please call method LogicModel::update_ports()
     * @exception InvalidObjectIDException This exception is
     *   thrown, if the port has no valid object ID.
     * @exception DegateLogicException Is thrown if \p gate_port has
     *   no valid GateTemplatePort or if the gate has no valid
     *   orientation.
     * @see LogicModel::update_ports()
     */

    virtual void add_port(GatePort_shptr gate_port) throw(InvalidObjectIDException, DegateLogicException);

    /**
     * Remove a gate port.
     * @se add_port()
     * @see LogicModel::update_ports()
     */

    virtual void remove_port(GatePort_shptr gate_port) throw(CollectionLookupException);

    /**
     * Get a gate port by a template port.
     */

    virtual GatePort_shptr get_port_by_template_port(GateTemplatePort_shptr template_port)
      throw(CollectionLookupException);


    /**
     * Set the ID of the corresponding template.
     *
     * This is useful, if there is a cell type defined for this
     * gate, but the cell library is not available, e.g. if
     * the gate library is unloaded in order to load another
     * version of the gate library. In this case we can't work
     * with (shared) pointers and need way to remember the cell
     * type.
     *
     * If you use method set_gate_template() you don't need to call
     * this method.
     *
     * @param template_type_id The ID of the template (cell type). A
     *      value of zero indicates, that there is no template for this
     *      gate.
     * @see set_gate_template()
     */

    virtual void set_template_type_id(object_id_t template_type_id);

    /**
     * Get the ID of the corresponding template.
     * @see set_template_type_id()
     */

    virtual object_id_t get_template_type_id() const;


    /**
     * A gate usually has a gate template. The gate template specifies common
     * properties. Use that function to set the gate template.
     * If the template has other dimensions than the gate, the gate shape is
     * adjusted, so that the gate's dimension complies with the template's one.
     * The attributes min_x and min_y are preserved in that case.
     *
     * This method updates the template type ID as well.
     * @see set_template_type_id()
     */

    virtual void set_gate_template(std::tr1::shared_ptr<GateTemplate> gate_template);

    /**
     * Get the gate template.
     * @see set_gate_template()
     */

    virtual std::tr1::shared_ptr<GateTemplate> get_gate_template() const;

    /**
     * Check if the gate has a template type set.
     * @return Returns true, if there is a template for this gate.
     *    A template is present, if the gate has a shared pointer to
     *    the template. That is that there is a object you can work with.
     *    Just having a template ID is not sufficient. If there is no
     *    shared pointer to the template, false is returned.
     */

    virtual bool has_template() const;


    /**
     * Remove template from gate. This will destroy all gate ports, too.
     */

    virtual void remove_template();

    /**
     * Check if a gate object has a port, that references a given template port.
     * This method bases on shared pointers not on simple port IDs.
     */

    virtual bool has_template_port(GateTemplatePort_shptr template_port);

    /**
     * If a physically placed gate has a template. This template
     * defines the appearance of the gate in terms of an image.
     * Because the physically placed gate can have another orientation than
     * the template image, you need to set the image orientation in
     * relation to the master image.
     */

    virtual void set_orientation(ORIENTATION _orientation);

    /**
     * Get the orientation relative to the master image.
     */

    virtual ORIENTATION get_orientation() const;


    /**
     * Check if the gate has a valid orientation.
     */

    bool has_orientation() const;

     /**
     * Get the orientation as a human readable string.
     */

    virtual std::string get_orienation_type_as_string() const;


    /**
     * Get an iterator to iterated over ports.
     */

    virtual port_iterator ports_begin();

    /**
     * Get an iterator to iterated over ports.
     */

    virtual port_const_iterator ports_begin() const;

    /**
     * Get an end iterator.
     * @see ports_begin()
     */

    virtual port_iterator ports_end();

    /**
     * Get an end iterator.
     * @see ports_begin()
     */

    virtual port_const_iterator ports_end() const;

    /**
     * Get the x-coordinate relative to min_x depending on the gate's orientation.
     * @param rel_x The x-coordinate realtive to min_x.
     * @exception DegateRuntimeException This exception is thrown if the object has an
     *   undefined gate orientation.
     * @see has_orientation()
     */

    virtual unsigned int get_relative_x_position_within_gate(int rel_x) const
      throw(DegateRuntimeException);

    /**
     * Get the y-coordinate relative to min_y depending on the gate's orientation.
     * @param rel_y The y-coordinate realtive to min_y.
     * @exception DegateRuntimeException This exception is thrown if the object has an
     *   undefined gate orientation.
     * @see has_orientation()
     */

    virtual unsigned int get_relative_y_position_within_gate(int rel_y) const
      throw(DegateRuntimeException);

    /**
     * Get a human readable string that describes the whole
     * logic model object. The string should be unique in order
     * to let the user identify the concrete object. But that
     * is not a must.
     */

    virtual const std::string get_descriptive_identifier() const;

    /**
     * Get a human readable string that names the object type.
     * Here it is "Gate".
     */

    virtual const std::string get_object_type_name() const;

    /**
     * Print gate.
     */

    void print(std::ostream & os = std::cout, int n_tabs = 0) const;



    bool in_shape(int x, int y, int max_distance = 0) const {
      return Rectangle::in_shape(x, y, max_distance);
    }

    void shift_x(int delta_x) {
      Rectangle::shift_x(delta_x);
    }

    void shift_y(int delta_y) {
      Rectangle::shift_y(delta_y);
    }


    virtual bool in_bounding_box(BoundingBox const& bbox) const {
      return Rectangle::in_bounding_box(bbox);
    }

    virtual BoundingBox const& get_bounding_box() const {
      return Rectangle::get_bounding_box();
    }


  };


  typedef std::tr1::shared_ptr<Gate> Gate_shptr;

}

#endif
