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

#ifndef __MODULE_H__
#define __MODULE_H__

#include <map>
#include <tr1/memory>

#include <LogicModelObjectBase.h>

namespace degate {

  /**
   * Implements a container to build up higher level entities.
   */

  class Module : public LogicModelObjectBase {
  public:
    
    typedef std::set<Module_shptr> module_collection;
    typedef std::set<Gate_shptr> gate_collection;

    /**
     * This map defines module ports.
     * A port is identified by a name. A module port is 'connected' to 
     * a gate ports. The relationship between port name and gate port is 
     * one to many in order to use it as a signal bus type.
     */
    typedef std::map<std::string, /* port name */
		     std::list<GatePort_shptr> > port_collection;

  private:
	
    module_collection modules;
    gate_collection gates;
    port_collection ports;

    std::string entity_name;


  private:

    void move_gates_recursive(Module * dst_mod) 
      throw(InvalidPointerException);

    void automove_gates();

    /**
     * Check if there is a gate is the current module or any child module
     * that has a gate port with the object ID \p oid .
     */
    bool exists_gate_port_recursive(object_id_t oid) const;

    /**
     * Determine ports of a module.
     */
    void determine_module_ports();

  public:

    /**
     * Construct a new module.
     */

    Module(std::string const& module_name = "", std::string const& _entity_name = "");
	
    /**
     * Destroy the module.
     */

    virtual ~Module();


    /**
     * Set an identifier for the module type.
     */

    void set_entity_name(std::string const& name);


    std::string get_entity_name() const;

    
    void add_gate(Gate_shptr gate) throw(InvalidPointerException);


    /**
     * Remove a gate from a module.
     * This method even works if the gate is not a direct child. Afterwards
     * the gate is completely removed from the module hierarchy (if you run this
     * method for the "right" root node). In most cases
     * you want to add the gate to the module hierarchy again. Therefore you have
     * to call method add_gate() on your own.
     * @return Returns true if a module was removed, else false.
     * @see add_gate()
     */

    bool remove_gate(Gate_shptr gate) throw(InvalidPointerException);


    void add_module(Module_shptr module) throw(InvalidPointerException);

    /**
     * Remove a submodule.
     * This method even works if the submodule is not a direct child. If you remove
     * a child module that contains gates, the gates are moved one layer above.
     * @return Returns true if a module was removed, else false.
     */

    bool remove_module(Module_shptr module) throw(InvalidPointerException);


    module_collection::iterator modules_begin();
    module_collection::iterator modules_end();
    gate_collection::iterator gates_begin();
    gate_collection::iterator gates_end();
    port_collection::iterator ports_begin();
    port_collection::iterator ports_end();
  };


}

#endif
