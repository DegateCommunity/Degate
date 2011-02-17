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
#include <LogicModel.h>

namespace degate {

  
  /**
   * Implements a container to build up higher level entities.
   */

  class Module : public LogicModelObjectBase {

    friend void determine_module_ports_for_root(LogicModel_shptr lmodel);

  public:

    typedef std::set<Module_shptr> module_collection;
    typedef std::set<Gate_shptr> gate_collection;

    /**
     * This map defines module ports.
     * A port is identified by a name. A module port is 'connected' to
     * a list of gate ports.
     */
    typedef std::map<std::string, /* port name */
		     std::list<GatePort_shptr> > port_collection;

  private:

    module_collection modules;
    gate_collection gates;
    port_collection ports;

    std::string entity_name; // name of a type
    bool is_root;
    
  private:

    /**
     * @throw InvalidPointerException This exception is thrown if the parameter is a NULL pointer.
     */
    void move_gates_recursive(Module * dst_mod);

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

    Module(std::string const& module_name = "", 
	   std::string const& _entity_name = "", 
	   bool is_root = false);

    /**
     * Destroy the module.
     */

    virtual ~Module();

    /**
     * Check if module is the main module.
     */
    
    bool is_main_module() const;

    /**
     * Set an identifier for the module type.
     */

    void set_entity_name(std::string const& name);


    /**
     * Get name of the entity type.
     */

    std::string get_entity_name() const;


    /**
     * Add a gate to a module.
     * @exception InvalidPointerException This exception is thrown, if \p gate is a NULL pointer.
     */

    void add_gate(Gate_shptr gate);


    /**
     * Remove a gate from a module.
     * This method even works if the gate is not a direct child. Afterwards
     * the gate is completely removed from the module hierarchy (if you run this
     * method for the "right" root node). In most cases
     * you want to add the gate to the module hierarchy again. Therefore you have
     * to call method add_gate() on your own.
     * @return Returns true if a module was removed, else false.
     * @see add_gate()
     * @exception InvalidPointerException This exception is thrown, if \p gate is a NULL pointer.
     */

    bool remove_gate(Gate_shptr gate);


    /**
     * Add a sub-module to a module.
     * @exception InvalidPointerException This exception is thrown, if \p gate is a NULL pointer.
     */

    void add_module(Module_shptr module);

    /**
     * Remove a submodule.
     * This method even works if the submodule is not a direct child. If you remove
     * a child module that contains gates, the gates are moved one layer above.
     * @return Returns true if a module was removed, else false.
     * @exception InvalidPointerException This exception is thrown, if \p gate is a NULL pointer.
     */

    bool remove_module(Module_shptr module);


    module_collection::iterator modules_begin();
    module_collection::iterator modules_end();
    gate_collection::iterator gates_begin();
    gate_collection::iterator gates_end();
    port_collection::iterator ports_begin();
    port_collection::iterator ports_end();

    module_collection::const_iterator modules_begin() const;
    module_collection::const_iterator modules_end() const;
    gate_collection::const_iterator gates_begin() const;
    gate_collection::const_iterator gates_end() const;
    port_collection::const_iterator ports_begin() const;
    port_collection::const_iterator ports_end() const;
  };


  /**
   * Determine ports of a root module.
   */
  void determine_module_ports_for_root(LogicModel_shptr lmodel);


}

#endif
