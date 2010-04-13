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

#ifndef __GATETEMPLATEPORT_H__
#define __GATETEMPLATEPORT_H__

#include "LogicModelObjectBase.h"
#include "Circle.h"
#include "Point.h"

namespace degate {

  /**
   * This class represents a port of a gate template.
   */
  class GateTemplatePort : public LogicModelObjectBase, public ColoredObject {

  public:

    /**
     * Enum to define type of ports.
     */
    enum PORT_TYPE {
      PORT_TYPE_UNDEFINED = 0,
      PORT_TYPE_IN = 1,
      PORT_TYPE_OUT = 2,
      PORT_TYPE_TRISTATE = 3
    };

  private:

    Point point;
    PORT_TYPE port_type;
    
    bool position_defined;


  public:

    /**
     * Create a template port.
     */

    GateTemplatePort(int _x, int _y, PORT_TYPE _port_type = PORT_TYPE_UNDEFINED) : 
      point(_x, _y), port_type(_port_type), position_defined( true) {
    }

    /**
     * Create a template port.
     */

    GateTemplatePort(PORT_TYPE _port_type = PORT_TYPE_UNDEFINED) : 
      point(0, 0), port_type(_port_type), position_defined(false) {
    }

  
    virtual ~GateTemplatePort() {}
    
    /**
     * Check if a position is defined for the port.
     */

    virtual bool is_position_defined() const { return position_defined; }


    /**
     * Set the port position within the gate template.
     * The position is in relative coordinates
     *   related to the left upper corner of the gate template.
     */

    virtual void set_point(Point p) {
      position_defined = true;
      point = p;
    }

    /**
     * Check if a port is of type input port.
     */

    virtual bool is_inport() const { return port_type == PORT_TYPE_IN || is_tristate(); }

    /**
     * Check if a port is of type output port.
     */

    virtual bool is_outport() const { return port_type == PORT_TYPE_OUT || is_tristate(); }

    /**
     * Check if a port is of type tristate.
     */

    virtual bool is_tristate() const { return port_type == PORT_TYPE_TRISTATE; }

    /**
     * Check if a port is of type is undefined.
     */

    virtual bool has_undefined_port_type() const { return port_type == PORT_TYPE_UNDEFINED; }

    /**
     * Set the port type.
     */

    virtual void set_port_type(PORT_TYPE _port_type) { port_type = _port_type; }

    /**
     * Get the port type.
     */

    virtual PORT_TYPE get_port_type() const { return port_type; }
    
    /**
     * Get a human readable string, that describes the port type.
     * That is "in", "out" or "tristate".
     */

    virtual const std::string get_port_type_as_string() const {
      switch(port_type) {
      case PORT_TYPE_IN: return std::string("in");
      case PORT_TYPE_OUT: return std::string("out");
      case PORT_TYPE_TRISTATE: return std::string("tristate");
	
      case PORT_TYPE_UNDEFINED: 
      default: return std::string("undefined");
      }
    }
    
    /**
     * Get the port position within the gate template.
     * @return Returns the position in relative coordinates
     *   related to the left upper corner of the gate template.
     */

    virtual Point const& get_point() const { return point; }

    /**
     * Get the x position for a port.
     * @return Returns the postion relative to the left upper corner of
     *   the gate template.
     */
    
    virtual int get_x() const { return point.get_x(); }

    /**
     * Get the y position for a port.
     * @return Returns the postion relative to the left upper corner of
     *   the gate template.
     */

    virtual int get_y() const { return point.get_y(); }
        

    /**
     * Parse a port type type indicating string.
     * @exception DegateRuntimeException This exception is thrown if the string
     *   cannot be parsed.
     */

    static PORT_TYPE get_port_type_from_string(std::string const& port_type_str)
      throw(DegateRuntimeException) {

      if(port_type_str == "undefined") return GateTemplatePort::PORT_TYPE_UNDEFINED;
      else if(port_type_str == "in") return  GateTemplatePort::PORT_TYPE_IN;
      else if(port_type_str == "out") return GateTemplatePort::PORT_TYPE_OUT;
      else if(port_type_str == "tristate") return GateTemplatePort::PORT_TYPE_TRISTATE;
      else throw DegateRuntimeException("Can't parse port type.");
    }
	
  };

  typedef std::tr1::shared_ptr<GateTemplatePort> GateTemplatePort_shptr;

}

#endif
