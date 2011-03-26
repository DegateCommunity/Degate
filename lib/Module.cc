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

#include <degate.h>
#include <Module.h>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

using namespace degate;


Module::Module(std::string const& module_name, 
	       std::string const& _entity_name, 
	       bool _is_root) :
  entity_name(_entity_name), 
  is_root(_is_root) {

  set_name(module_name);
}

Module::~Module() {}

bool Module::is_main_module() const {
  return is_root;
}

void Module::set_main_module() {
  is_root = true;
}


void Module::set_entity_name(std::string const& name) {
  entity_name = name;
}

std::string Module::get_entity_name() const {
  return entity_name;
}


void Module::add_gate(Gate_shptr gate, bool detect_ports) {
  if(gate == NULL)
    throw InvalidPointerException("Invalid pointer passed to add_gate().");

  gates.insert(gate);
  if(!is_root && detect_ports) determine_module_ports();
}

bool Module::remove_gate(Gate_shptr gate) {
  if(gate == NULL)
    throw InvalidPointerException("Invalid pointer passed to remove_gate().");


  gate_collection::const_iterator g_iter = gates.find(gate);
  if(g_iter != gates.end()) {
    gates.erase(g_iter);
    if(!is_root) determine_module_ports();
    return true;
  }
  else {

    for(module_collection::iterator iter = modules.begin();
	iter != modules.end(); ++iter) {

      Module_shptr child = *iter;
      if((*iter)->remove_gate(gate) == true) return true;
    }
  }

  return false;
}


void Module::add_module(Module_shptr module) {
  if(module == NULL)
    throw InvalidPointerException("Invalid pointer passed to add_modue().");

  modules.insert(module);
}



bool Module::remove_module(Module_shptr module) {

  if(module == NULL)
    throw InvalidPointerException("Invalid pointer passed to remove_module().");

  for(module_collection::iterator iter = modules.begin();
      iter != modules.end(); ++iter) {

    Module_shptr child = *iter;

    if(child == module) {
      child->move_gates_recursive(this);
      modules.erase(iter);
      return true;
    }
    else if((*iter)->remove_module(module) == true)
      return true;
  }

  return false;
}

void Module::move_gates_recursive(Module * dst_mod) {

  if(dst_mod == NULL)
    throw InvalidPointerException("Invalid pointer passed to move_all_child_gates_into_current_module().");

  for(gate_collection::iterator g_iter = gates_begin();
      g_iter != gates_end(); ++g_iter) {
    std::cout << "Add gate " << (*g_iter)->get_name() << " to module " << dst_mod->get_name() << std::endl;

    dst_mod->add_gate(*g_iter);
  }

  for(module_collection::iterator iter = modules.begin();
      iter != modules.end(); ++iter)
    (*iter)->move_gates_recursive(dst_mod);
}

Module::module_collection::iterator Module::modules_begin() {
  return modules.begin();
}

Module::module_collection::iterator Module::modules_end() {
  return modules.end();
}

Module::gate_collection::iterator Module::gates_begin() {
  return gates.begin();
}

Module::gate_collection::iterator Module::gates_end() {
  return gates.end();
}

Module::port_collection::iterator Module::ports_begin() {
  return ports.begin();
}

Module::port_collection::iterator Module::ports_end() {
  return ports.end();
}

Module::module_collection::const_iterator Module::modules_begin() const {
  return modules.begin();
}

Module::module_collection::const_iterator Module::modules_end() const {
  return modules.end();
}

Module::gate_collection::const_iterator Module::gates_begin() const {
  return gates.begin();
}

Module::gate_collection::const_iterator Module::gates_end() const {
  return gates.end();
}

Module::port_collection::const_iterator Module::ports_begin() const {
  return ports.begin();
}

Module::port_collection::const_iterator Module::ports_end() const {
  return ports.end();
}


void Module::automove_gates() {
  /*
    from top to down in the module hierarchy:

    for each module: iterate over gates:

      iterate over gate ports:
      - port unconnected - ignore
      - port connected - check all related

   */
}


bool Module::net_feeded_internally(Net_shptr net) {
  for(Net::connection_iterator c_iter = net->begin(); c_iter != net->end(); ++c_iter) {

    object_id_t oid = *c_iter;   
    GatePort_shptr gport = lookup_gate_port_recursive(oid);

    if(gport != NULL) { // internal entity
      GateTemplatePort_shptr tmpl_port = gport->get_template_port();
      if(tmpl_port->is_outport()) return true;
    }
  }
  return false;
}


std::string gate_port_already_named(degate::Module::port_collection const& ports, degate::GatePort_shptr gate_port) {
  for(Module::port_collection::const_iterator iter = ports.begin(); iter != ports.end(); ++iter) {
    if(iter->second == gate_port) return iter->first;
  }
  return "";
}


void Module::determine_module_ports() {

  int pnum = 0;
  port_collection new_ports;
  std::set<Net_shptr> known_net;

  for(gate_collection::iterator g_iter = gates_begin(); g_iter != gates_end(); ++g_iter) {

    Gate_shptr gate = *g_iter;
    assert(gate != NULL);

    for(Gate::port_const_iterator p_iter = gate->ports_begin(); p_iter != gate->ports_end(); ++p_iter) {

      GatePort_shptr gate_port = *p_iter;
      assert(gate_port != NULL);

      Net_shptr net = gate_port->get_net();
      std::cout << "Check net for object gate port " << gate_port->get_descriptive_identifier() << "?\n";

      bool net_already_processed = known_net.find(net) != known_net.end();
      if((net != NULL) && !net_already_processed) {

	bool is_a_port = false;
 
	for(Net::connection_iterator c_iter = net->begin(); c_iter != net->end() && !is_a_port; ++c_iter) {

	  object_id_t oid = *c_iter;

	  if(!exists_gate_port_recursive(oid)) { // outbound connection

	    // Now we check, wheather the connection is feeded by an ouside entity or feeded
	    // from this module.
	
	    // Problem: We can't see the object outside this module, because we only have an
	    // object ID and no logic model object to look up the object ID. Therefore we have
	    // to derive the state of feeding from the obtjects we have in this or any sub-module.

	    // If we see only in-ports in the net, the module port must be driven by an outside
	    // port.

	    GateTemplatePort_shptr tmpl_port = gate_port->get_template_port();
	    assert(tmpl_port != NULL); // if a gate has no standard cell type, the gate cannot have a port
	    
	    if(net_feeded_internally(net) && tmpl_port->is_inport()) {
	      std::cout << "  Net feeded internally, but port is inport. Will check where the net is driven.\n";	
	    }
	    else {
	      std::string mod_port_name = gate_port_already_named(ports, gate_port);
	      if(mod_port_name == "") {

		// generate a new port name and check if the port name is already in use
		do {
		  pnum++;
		  boost::format f("p%1%");
		  f % pnum;
		  mod_port_name = f.str();		
		}while(ports.find(mod_port_name) != ports.end());
	      }
	      
	      std::cout << "  New module port: " << gate_port->get_descriptive_identifier() << " == "
			<< mod_port_name << "\n";
	      new_ports[mod_port_name] = gate_port;
	      
	      is_a_port = true;
	      std::cout << "  Set net as known net: " << net->get_descriptive_identifier() << "\n";
	      known_net.insert(net);
	    }
	  }
	}
      }
      else if(net == NULL) {
	std::cout << "  Will not check net -- no net.\n";
      }
      else if(net_already_processed) {
	std::cout << "  Will not check net -- already processed " << net->get_descriptive_identifier() << ".\n";
      }
    }
  }

  ports = new_ports;

}

void Module::add_module_port(std::string const& module_port_name, GatePort_shptr adjacent_gate_port) {
  ports[module_port_name] = adjacent_gate_port;
}

bool Module::exists_gate_port_recursive(object_id_t oid) const {
  return lookup_gate_port_recursive(oid) != NULL;
}

GatePort_shptr Module::lookup_gate_port_recursive(object_id_t oid) const {
  assert(oid != 0);

  for(gate_collection::const_iterator g_iter = gates.begin();
      g_iter != gates.end(); ++g_iter) {

    Gate_shptr gate = *g_iter;

    for(Gate::port_const_iterator p_iter = gate->ports_begin();
	p_iter != gate->ports_end(); ++p_iter)
      if((*p_iter)->get_object_id() == oid) return *p_iter;
  }

  for(module_collection::const_iterator iter = modules.begin();
      iter != modules.end(); ++iter)
    if(GatePort_shptr gport = (*iter)->lookup_gate_port_recursive(oid)) return gport;

  return GatePort_shptr();
}




void degate::determine_module_ports_for_root(LogicModel_shptr lmodel) {
  debug(TM, "Check for module ports.");
  /*
    Iterate over all gate and check their ports.
    Get the net for a port.
    Iterate over net.
    If a net contains a emarker with a description of 'module-port', use
    gate port as module port.
   */

  Module_shptr main_module = lmodel->get_main_module();

  main_module->ports.clear(); // reset ports

  for(Module::gate_collection::iterator g_iter = main_module->gates_begin(); 
      g_iter != main_module->gates_end(); ++g_iter) {

    Gate_shptr gate = *g_iter;
    assert(gate != NULL);

    for(Gate::port_const_iterator p_iter = gate->ports_begin(); p_iter != gate->ports_end(); ++p_iter) {

      GatePort_shptr gate_port = *p_iter;
      assert(gate_port != NULL);

      Net_shptr net = gate_port->get_net();
      bool is_a_port = false;

      if(net != NULL) {

	for(Net::connection_iterator c_iter = net->begin(); c_iter != net->end() && !is_a_port; ++c_iter) {
	  
	  object_id_t oid = *c_iter;
	  assert(oid != 0);
	  
	  PlacedLogicModelObject_shptr lmo = lmodel->get_object(oid);
	  if(EMarker_shptr em = std::tr1::dynamic_pointer_cast<EMarker>(lmo)) {
	    debug(TM, "Connected with emarker");

	    if(em->get_description() == "module-port") {
	      
	      GateTemplatePort_shptr tmpl_port = gate_port->get_template_port();
	      assert(tmpl_port != NULL); // if a gate has no standard cell type, the gate cannot have a port
	      
	      main_module->ports[em->get_name()] = gate_port;
	      is_a_port = true;
	    }
	  }
	} 
	    
      } // end of net-object-iteration
    } // end of gate-portiteration
  } // end of gate-iteration

}



Module_shptr Module::lookup_module(std::string const& module_path) const {


  std::list<std::string> path_elements;
  boost::split(path_elements, module_path, boost::is_any_of("/"));
  
  if(is_main_module()) {

    // handle absolut path
    while(path_elements.size() > 0 &&
	  get_name() != path_elements.front()) {
      path_elements.pop_front();
    }
  }

  if(path_elements.size() == 0)
    return Module_shptr();


  // leading path element should equals current module name
  if(get_name() != path_elements.front())
    return Module_shptr();
  else {
    path_elements.pop_front();
    return lookup_module(path_elements); // first real name is name of the main module, skipped
  }

}

Module_shptr Module::lookup_module(std::list<std::string> & path_elements) const {
  
  if(path_elements.size() > 0) {

    BOOST_FOREACH(Module_shptr m, modules) {

      if(m->get_name() == path_elements.front()) {

	if(path_elements.size() == 1)
	  return m;
	else {
	  path_elements.pop_front();
	  return m->lookup_module(path_elements);
	}
      }

    }
  }

  return Module_shptr();
}

void Module::set_module_port_name(std::string const& module_port_name, GatePort_shptr adjacent_gate_port) {

  for(Module::port_collection::const_iterator iter = ports.begin(); iter != ports.end(); ++iter) {
    if(iter->second == adjacent_gate_port) {
      ports.erase(iter->first);
      ports[module_port_name] = adjacent_gate_port;
      return;
    }
  }
  
}


boost::optional<std::string> Module::lookup_module_port_name(GatePort_shptr gate_port) {
  
  for(Module::port_collection::const_iterator iter = ports.begin(); iter != ports.end(); ++iter) {
    if(iter->second == gate_port) {
      return boost::optional<std::string>(iter->first);
    }
  }
  return boost::optional<std::string>();
}


bool Module::exists_module_port_name(std::string const& module_port_name) const {
  return ports.find(module_port_name) != ports.end();
}
