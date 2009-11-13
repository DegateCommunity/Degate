/*
 
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

#include <degate.h>
#include "globals.h"
#include "Layer.h"

#include "LogicModelObjectBase.h"
#include "Net.h"

#include "ConnectedLogicModelObject.h"
#include "degate_exceptions.h"

//#include "GateTemplate.h"
//#include "GateTemplatePort.h"
#include "GateLibrary.h"

#include "LogicModel.h"

using namespace std;
using namespace degate;


std::tr1::shared_ptr<Layer> LogicModel::get_create_layer(layer_position_t pos) {
  
  if(layers.size() <= pos || layers.at(pos) == NULL) {
    add_layer(pos);
  }
  
  return layers[pos];
}

void LogicModel::print(std::ostream & os) {


  os
    << endl
    << "--------------------------------[ Logic model ]--------------------------------" << endl;

  for(object_collection::iterator iter = objects.begin(); iter != objects.end(); ++iter) {
    os << "\t+ Object: " 
       << (*iter).second->get_object_type_name() << " " 
       << (*iter).second->get_object_id() << endl;

    // XXX dynamic cast and print

  }

  os << endl;

  os
    << endl
    << "--------------------------------[ Gate library ]--------------------------------" << endl;

  // print gate library
  if(gate_library) gate_library->print(os);

  os
    << endl
    << "--------------------------------[ Layers ]--------------------------------" << endl;
  
  // iterate over layers and print them

  for(layer_collection::iterator iter = layers.begin(); 
      iter != layers.end(); ++iter) {

    Layer_shptr layer = *iter;
    layer->print(os);
  }

}

object_id_t LogicModel::get_new_object_id() { 
  object_id_t new_id = ++object_id_counter;
  while(objects.find(new_id) != objects.end()) {
    new_id = ++object_id_counter;
  }
  return new_id;
}


LogicModel::LogicModel(unsigned int width, unsigned int height, unsigned int layers) : 
  bounding_box(width, height),
  object_id_counter(0) {

  for(unsigned int i = 0; i < layers; i++)
    get_create_layer(i);
  
  if(layers > 0)
    set_layer(0);

  gate_library = GateLibrary_shptr(new GateLibrary());
}

LogicModel::~LogicModel() {
}

int LogicModel::get_width() const {
  return bounding_box.get_width();
}

int LogicModel::get_height() const {
  return bounding_box.get_height();
}

PlacedLogicModelObject_shptr LogicModel::get_object(object_id_t object_id) 
  throw(CollectionLookupException) {

  std::map<object_id_t, PlacedLogicModelObject_shptr>::iterator found = 
    objects.find(object_id);

  if(found == objects.end()) {
    std::ostringstream stm;
    stm << "Can't find object with id " << object_id << " in logic model.";
    throw CollectionLookupException(stm.str());
  }
  else {
    return (*found).second;
  }
}

void LogicModel::add_wire(int layer_pos, Wire_shptr o) throw(InvalidPointerException) {

  if(o == NULL) throw InvalidPointerException();
  if(!o->has_valid_object_id()) o->set_object_id(get_new_object_id());
  wires[o->get_object_id()] = o;
}

void LogicModel::add_via(int layer_pos, Via_shptr o) throw(InvalidPointerException) {

  if(o == NULL) throw InvalidPointerException();
  if(!o->has_valid_object_id()) o->set_object_id(get_new_object_id());
  vias[o->get_object_id()] = o;
}

void LogicModel::add_gate(int layer_pos, Gate_shptr o) throw(InvalidPointerException) {

  if(o == NULL) throw InvalidPointerException();
  if(!o->has_valid_object_id()) o->set_object_id(get_new_object_id());
  gates[o->get_object_id()] = o;
  
  // iterate over ports and add them into the lookup table
  for(Gate::port_iterator iter = o->ports_begin(); iter != o->ports_end(); ++iter) {

    assert(*iter != NULL);
    assert((*iter)->has_valid_object_id() == true);
      
    add_object(layer_pos, std::tr1::dynamic_pointer_cast<PlacedLogicModelObject>(*iter));
  }
}

void LogicModel::remove_gate_ports(Gate_shptr o) throw(InvalidPointerException) {
  if(o == NULL) throw InvalidPointerException();
  // iterate over ports and remove them from the lookup table
  for(Gate::port_iterator iter = o->ports_begin(); iter != o->ports_end(); ++iter) {
    object_id_t port_id = (*iter)->get_object_id();
    remove_object(get_object(port_id));
  }
}

void LogicModel::remove_gate(Gate_shptr o) throw(InvalidPointerException) {
      
  if(o == NULL) throw InvalidPointerException();
  remove_gate_ports(o);
  debug(TM, "remove gate");
  gates.erase(o->get_object_id());
}

void LogicModel::remove_wire(Wire_shptr o) throw(InvalidPointerException) {
  if(o == NULL) throw InvalidPointerException();
  wires.erase(o->get_object_id());
}

void LogicModel::remove_via(Via_shptr o) throw(InvalidPointerException) {
  if(o == NULL) throw InvalidPointerException();
  vias.erase(o->get_object_id());
}




void LogicModel::add_object(int layer_pos, PlacedLogicModelObject_shptr o) 
  throw(DegateLogicException, InvalidPointerException) {

  if(o == NULL) throw InvalidPointerException();
  if(!o->has_valid_object_id()) o->set_object_id(get_new_object_id());
  object_id_t object_id = o->get_object_id();
  
  if(Gate_shptr gate = std::tr1::dynamic_pointer_cast<Gate>(o))
    add_gate(layer_pos, gate);
  else if(Wire_shptr wire = std::tr1::dynamic_pointer_cast<Wire>(o))
    add_wire(layer_pos, wire);
  else if(Via_shptr via = std::tr1::dynamic_pointer_cast<Via>(o))
    add_via(layer_pos, via);

  if(objects.find(object_id) != objects.end()) {
    std::ostringstream stm;
    stm << "Logic model object with id " << object_id << " is already stored in the logic model.";
    std::cout << stm.str() << std::endl;
    throw DegateLogicException(stm.str());
  }
  else {
    objects[object_id] = o;
    Layer_shptr layer = get_create_layer(layer_pos);
    assert(layer != NULL);
    o->set_layer(layer);
    layer->add_object(o);
  }
  assert(objects.find(object_id) != objects.end());

}




void LogicModel::remove_object(PlacedLogicModelObject_shptr o) throw(InvalidPointerException) {

  if(o == NULL) throw InvalidPointerException();
  Layer_shptr layer = o->get_layer();
  if(layer == NULL) {
    debug(TM, "warning: object has no layer");
  }
  else {

    if(Gate_shptr gate = std::tr1::dynamic_pointer_cast<Gate>(o))
      remove_gate(gate);
    else if(Wire_shptr wire = std::tr1::dynamic_pointer_cast<Wire>(o))
      remove_wire(wire);
    else if(Via_shptr via = std::tr1::dynamic_pointer_cast<Via>(o))
      remove_via(via);
    
    layer->remove_object(o);
  }
  objects.erase(o->get_object_id());
}






void LogicModel::add_gate_template(GateTemplate_shptr tmpl) 
  throw(DegateLogicException) {
  if(gate_library != NULL) {
    if(!tmpl->has_valid_object_id())  tmpl->set_object_id(get_new_object_id());
    gate_library->add_template(tmpl);
    //update_gate_ports(tmpl);
    
    // XXX iterate over gates and check tmpl-id -> update
  }
  else {
    throw DegateLogicException("You can't add a gate template, if there is no gate library.");
  }
}


void LogicModel::remove_gate_template(GateTemplate_shptr tmpl) throw(DegateLogicException) {
  if(gate_library == NULL) 
    throw DegateLogicException("You can't remove a gate template, if there is no gate library.");
  else {
    remove_gates_by_template_type(tmpl);
    gate_library->remove_template(tmpl);
  }
}

void LogicModel::remove_template_references(GateTemplate_shptr tmpl) throw(DegateLogicException) {
  if(gate_library == NULL) 
    throw DegateLogicException("You can't remove a gate template, if there is no gate library.");
  for(gate_collection::iterator iter = gates_begin();
      iter != gates.end(); ++iter) {
    Gate_shptr gate = (*iter).second;
    if(gate->get_gate_template() == tmpl) {
      remove_gate_ports(gate);
      gate->remove_template();
    }

  }
}


void LogicModel::remove_gates_by_template_type(GateTemplate_shptr tmpl) 
  throw(InvalidPointerException) {
  if(tmpl == NULL) throw InvalidPointerException("The gate template pointer is invalid.");

  std::list<Gate_shptr> gates_to_remove;

  for(gate_collection::iterator iter = gates_begin();
      iter != gates_end(); ++iter) {
    Gate_shptr gate = (*iter).second;
    
    if(gate->get_gate_template() == tmpl) 
      gates_to_remove.push_back(gate);
  }

  while(!gates_to_remove.empty()) {
    remove_object(gates_to_remove.front());
    gates_to_remove.pop_front();
  }

}

void LogicModel::add_template_port_to_gate_template(GateTemplate_shptr gate_template, 
						    GateTemplatePort_shptr template_port) {

  gate_template->add_template_port(template_port);
  update_ports(gate_template);
}

void LogicModel::remove_template_port_from_gate_template(GateTemplate_shptr gate_template, 
							 GateTemplatePort_shptr template_port) {
  
  gate_template->remove_template_port(template_port);
  update_ports(gate_template);
}

void LogicModel::update_ports(Gate_shptr gate) throw(InvalidPointerException) {

  if(gate == NULL)
    throw InvalidPointerException("Invalid parameter for update_ports()");

  GateTemplate_shptr gate_template = gate->get_gate_template();

  debug(TM, "upate ports on gate %d", gate->get_object_id());

  if(gate->has_template()) {
    // iterate over template ports
    for(GateTemplate::port_iterator tmpl_port_iter = gate_template->ports_begin();
	tmpl_port_iter != gate_template->ports_end(); ++tmpl_port_iter) {
      GateTemplatePort_shptr tmpl_port = *tmpl_port_iter;
      
      if(!gate->has_template_port(tmpl_port) && gate->has_orientation()) {
	debug(TM, "adding a port to gate");
	GatePort_shptr new_gate_port(new GatePort(gate, tmpl_port));
	new_gate_port->set_object_id(get_new_object_id());
	gate->add_port(new_gate_port); // will set coordinates, too
	
	assert(gate->get_layer() != NULL);
	add_object(gate->get_layer()->get_layer_pos(), new_gate_port);
      }
    }
  }
  
  std::list<GatePort_shptr> ports_to_remove;

  
  // iterate over gate ports
  for(Gate::port_iterator port_iter = gate->ports_begin();
      port_iter != gate->ports_end(); ++port_iter) {
    
    debug(TM, "iterating over ports");
    GatePort_shptr gate_port = *port_iter;
    assert(gate_port != NULL);

    if(gate->has_template()) {

      GateTemplate_shptr tmpl = gate->get_gate_template();
      assert(tmpl != NULL);

      GateTemplatePort_shptr tmpl_port = gate_port->get_template_port();
      assert(tmpl_port != NULL);

      bool has_template_port = tmpl->has_template_port(tmpl_port->get_object_id());

      if(has_template_port) {
	GateTemplatePort_shptr tmpl_port = gate_port->get_template_port();
	// reset port coordinates
	if(gate->has_orientation()) {
	  unsigned int x, y;
	  x = gate->get_relative_x_position_within_gate(tmpl_port->get_x());
	  y = gate->get_relative_y_position_within_gate(tmpl_port->get_y());
	  gate_port->set_x(x + gate->get_min_x());
	  gate_port->set_y(y + gate->get_min_y());
	  gate_port->set_name(tmpl_port->get_name());
	}
      }
      // unset port coordinates
      else {
	debug(TM, "should remove port from gate");
	ports_to_remove.push_back(gate_port);
      }
    }
    else {
      debug(TM, "should remove port from gate, because gate has no template");
      ports_to_remove.push_back(gate_port);
    }
  }

  for(std::list<GatePort_shptr>::iterator iter = ports_to_remove.begin();
      iter != ports_to_remove.end(); ++iter) {
    debug(TM, "remove real port:");
    (*iter)->print();
    gate->remove_port(*iter);
    remove_object(*iter);
  }

}

void LogicModel::update_ports(GateTemplate_shptr gate_template) 
  throw(InvalidPointerException) {

  if(gate_template == NULL)
    throw InvalidPointerException("Invalid parameter for update_ports()");

  // iterate over all gates ...     
  for(gate_collection::iterator g_iter = gates.begin(); 
      g_iter != gates.end(); ++g_iter) {
    Gate_shptr gate = (*g_iter).second;
    if(gate->get_gate_template() == gate_template) {
      debug(TM, "update ports on gate with id %d", gate->get_object_id());
      update_ports(gate);
    }
  }
}


void LogicModel::add_layer(layer_position_t pos, Layer_shptr new_layer) 
  throw(DegateLogicException) {

  if(layers.size() <= pos) layers.resize(pos + 1);
      
  if(layers[pos] != NULL) 
    throw DegateLogicException("There is already a layer for this layer number.");
  else {
    if(!new_layer->is_empty()) throw DegateLogicException("You must add an empty layer.");
    layers[pos] = new_layer;
    new_layer->set_layer_pos(pos);
  }

  if(current_layer == NULL) current_layer = get_layer(0);
  if(current_layer == NULL) current_layer = new_layer;
}


void LogicModel::add_layer(layer_position_t pos) throw(DegateLogicException) {
  Layer_shptr new_layer(new Layer(bounding_box));
  add_layer(pos, new_layer);
}

Layer_shptr LogicModel::get_layer(layer_position_t pos) {
  return layers.at(pos);
}


void LogicModel::remove_layer(layer_position_t pos) {
  remove_layer(layers.at(pos));
}

void LogicModel::remove_layer(Layer_shptr layer) {
  /* 
     XXX: gates / wires / vias / connections
  */
  layers.erase(remove(layers.begin(), layers.end(), layer), layers.end());
}

void LogicModel::set_layer(layer_position_t pos) {
  current_layer = layers[pos];
}

Layer_shptr LogicModel::get_current_layer() {
  return current_layer;
}

GateLibrary_shptr LogicModel::get_gate_library() { 
  return gate_library;  
}

void LogicModel::set_gate_library(GateLibrary_shptr new_gate_lib) {
  if(gate_library != NULL) {
    // XXX 
  }
  gate_library = new_gate_lib;
}

void LogicModel::add_net(Net_shptr net) {
  if(!net->has_valid_object_id()) net->set_object_id(get_new_object_id());
  nets[net->get_object_id()] = net;
}


Net_shptr LogicModel::get_net(object_id_t net_id) {
  return nets[net_id];
}

void LogicModel::remove_net(Net_shptr net) 
  throw(InvalidObjectIDException, CollectionLookupException) {
  if(!net->has_valid_object_id()) 
    throw InvalidObjectIDException("The net object has no object ID.");
  else if(nets.find(net->get_object_id()) == nets.end())
    throw CollectionLookupException("Unknown net.");
  else {
    while(net->size() > 0) {
      
      // get an object ID from the net
      object_id_t oid = *(net->begin());
      
      // logic check: this object should be known
      if(objects.find(oid) == objects.end()) throw CollectionLookupException();
      
      // the logic model object should be connectable
      if(ConnectedLogicModelObject_shptr o = 
	 std::tr1::dynamic_pointer_cast<ConnectedLogicModelObject>(objects[oid])) {
	
	// unconnect object from net and net from object
	o->remove_net();
      }
      else 
	throw DegateLogicException("Can't dynamic cast to a shared ptr of "
				   "ConnectedLogicModelObject, but the object "
				   "must be of that type, because it is "
				   "referenced from a net.");
    }
    
    // remove the net
    nets[net->get_object_id()].reset();
  }
}

LogicModel::object_collection::iterator LogicModel::objects_begin() {
  return objects.begin();
}

LogicModel::object_collection::iterator LogicModel::objects_end() {
  return objects.end();
}

LogicModel::gate_collection::iterator LogicModel::gates_begin() {
  return gates.begin();
}

LogicModel::gate_collection::iterator LogicModel::gates_end() {
  return gates.end();
}

LogicModel::layer_collection::iterator LogicModel::layers_begin() {
  return layers.begin();
}

LogicModel::layer_collection::iterator LogicModel::layers_end() {
  return layers.end();
}

LogicModel::net_collection::iterator LogicModel::nets_begin() {
  return nets.begin();
}

LogicModel::net_collection::iterator LogicModel::nets_end() {
  return nets.end();
}


unsigned int LogicModel::get_num_layers() const {
  return layers.size();
}
