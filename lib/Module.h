/* -*-c++-*-
 
 This file is part of the IC reverse engineering tool degate.
 
 Copyright 2008, 2009 by Martin Schobert
 
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

  private:
	
    module_collection modules;
    gate_collection gates;

    std::map<std::string, /* port name */
	     std::list<GatePort_shptr> > ports;

    std::string entity_name;

  public:

    /**
     * Construct a new module.
     */

    Module(std::string const& module_name = "", std::string const& _entity_name = "") :
      entity_name(_entity_name) {
      set_name(module_name);
    }
	
    /**
     * Destroy the module.
     */

    virtual ~Module() {}


    void set_enitity_name(std::string const& name) {
      entity_name = name;
    }

    std::string get_entity_name() const {
      return entity_name;
    }

    
    void add_gate(Gate_shptr gate) throw(InvalidPointerException) {
      if(gate == NULL) 
	throw InvalidPointerException("Invalid pointer passed to add_gate().");

      gates.insert(gate);
    }

    void remove_gate(Gate_shptr gate) throw(InvalidPointerException) {
      if(gate == NULL) 
	throw InvalidPointerException("Invalid pointer passed to add_gate().");

      //gates.insert(gate);
    }


    void add_module(Module_shptr module) throw(InvalidPointerException) {
      if(module == NULL) 
	throw InvalidPointerException("Invalid pointer passed to add_gate().");

      modules.insert(module);
    }


    module_collection::iterator modules_begin() {
      return modules.begin();
    }

    module_collection::iterator modules_end() {
      return modules.end();
    }

    gate_collection::iterator gates_begin() {
      return gates.begin();
    }

    gate_collection::iterator gates_end() {
      return gates.end();
    }


  };


}

#endif
