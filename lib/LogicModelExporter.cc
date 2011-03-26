/*

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
#include "LogicModelExporter.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <list>
#include <tr1/memory>

using namespace std;
using namespace degate;

void LogicModelExporter::export_data(std::string const& filename, LogicModel_shptr lmodel) {

  if(lmodel == NULL) throw InvalidPointerException("Logic model pointer is NULL.");

  try {

    xmlpp::Document doc;

    xmlpp::Element * root_elem = doc.create_root_node("logic-model");
    assert(root_elem != NULL);

    xmlpp::Element* gates_elem = root_elem->add_child("gates");
    if(gates_elem == NULL) throw(std::runtime_error("Failed to create node."));

    xmlpp::Element* vias_elem = root_elem->add_child("vias");
    if(vias_elem == NULL) throw(std::runtime_error("Failed to create node."));

    xmlpp::Element* emarkers_elem = root_elem->add_child("emarkers");
    if(emarkers_elem == NULL) throw(std::runtime_error("Failed to create node."));

    xmlpp::Element* wires_elem = root_elem->add_child("wires");
    if(wires_elem == NULL) throw(std::runtime_error("Failed to create node."));

    xmlpp::Element* nets_elem = root_elem->add_child("nets");
    if(nets_elem == NULL) throw(std::runtime_error("Failed to create node."));

    xmlpp::Element* annotations_elem = root_elem->add_child("annotations");
    if(annotations_elem == NULL) throw(std::runtime_error("Failed to create node."));

    for(LogicModel::layer_collection::iterator layer_iter = lmodel->layers_begin();
	layer_iter != lmodel->layers_end(); ++layer_iter) {

      Layer_shptr layer = *layer_iter;

      for(Layer::object_iterator iter = layer->objects_begin();
	  iter != layer->objects_end(); ++iter) {

	layer_position_t layer_pos = layer->get_layer_pos();

	PlacedLogicModelObject_shptr o = (*iter);

	if(Gate_shptr gate = std::tr1::dynamic_pointer_cast<Gate>(o))
	  add_gate(gates_elem, gate, layer_pos);

	else if(Via_shptr via = std::tr1::dynamic_pointer_cast<Via>(o))
	  add_via(vias_elem, via, layer_pos);

	else if(EMarker_shptr emarker = std::tr1::dynamic_pointer_cast<EMarker>(o))
	  add_emarker(emarkers_elem, emarker, layer_pos);

	else if(Wire_shptr wire = std::tr1::dynamic_pointer_cast<Wire>(o))
	  add_wire(wires_elem, wire, layer_pos);

	else if(Annotation_shptr annotation = std::tr1::dynamic_pointer_cast<Annotation>(o))
	  add_annotation(annotations_elem, annotation, layer_pos);

      }
    }

    add_nets(nets_elem, lmodel);

    // actually we have only one main module
    xmlpp::Element* modules_elem = root_elem->add_child("modules");
    if(modules_elem == NULL) throw(std::runtime_error("Failed to create node."));
    else add_module(modules_elem, lmodel, lmodel->get_main_module());

    doc.write_to_file_formatted(filename, "ISO-8859-1");

  }
  catch(const std::exception& ex) {
    std::cout << "Exception caught: " << ex.what() << std::endl;
    throw;
  }

}

void LogicModelExporter::add_nets(xmlpp::Element* nets_elem, LogicModel_shptr lmodel) {

  for(LogicModel::net_collection::iterator net_iter = lmodel->nets_begin();
      net_iter != lmodel->nets_end(); ++net_iter) {

    xmlpp::Element* net_elem = nets_elem->add_child("net");

    Net_shptr net = net_iter->second;
    assert(net != NULL);

    object_id_t old_net_id = net->get_object_id();
    assert(old_net_id != 0);
    object_id_t new_net_id = oid_rewriter->get_new_object_id(old_net_id);

    net_elem->set_attribute("id", number_to_string<object_id_t>(new_net_id));

    for(Net::connection_iterator conn_iter = net->begin();
	conn_iter != net->end(); ++conn_iter) {
      object_id_t oid = *conn_iter;

      const ConnectedLogicModelObject_shptr conn_obj =
	std::tr1::dynamic_pointer_cast<ConnectedLogicModelObject>(lmodel->get_object(oid));

      xmlpp::Element* conn_elem = net_elem->add_child("connection");
      conn_elem->set_attribute("object-id",
			       number_to_string<object_id_t>(oid_rewriter->get_new_object_id(oid)));

    }

  }

}

void LogicModelExporter::add_gate(xmlpp::Element* gates_elem, Gate_shptr gate, layer_position_t layer_pos) {

  xmlpp::Element* gate_elem = gates_elem->add_child("gate");
  if(gate_elem == NULL) throw(std::runtime_error("Failed to create node."));

  object_id_t new_oid = oid_rewriter->get_new_object_id(gate->get_object_id());
  gate_elem->set_attribute("id", number_to_string<object_id_t>(new_oid));
  gate_elem->set_attribute("name", gate->get_name());
  gate_elem->set_attribute("description", gate->get_description());
  gate_elem->set_attribute("layer", number_to_string<layer_position_t>(layer_pos));
  gate_elem->set_attribute("orientation", gate->get_orienation_type_as_string());

  gate_elem->set_attribute("min-x", number_to_string<int>(gate->get_min_x()));
  gate_elem->set_attribute("min-y", number_to_string<int>(gate->get_min_y()));
  gate_elem->set_attribute("max-x", number_to_string<int>(gate->get_max_x()));
  gate_elem->set_attribute("max-y", number_to_string<int>(gate->get_max_y()));

  gate_elem->set_attribute("type-id",
			   number_to_string<object_id_t>(oid_rewriter->get_new_object_id(gate->get_template_type_id())));


  for(Gate::port_iterator iter = gate->ports_begin();
      iter != gate->ports_end(); ++iter) {

    GatePort_shptr port = *iter;

    xmlpp::Element* port_elem = gate_elem->add_child("port");
    if(port_elem == NULL) throw(std::runtime_error("Failed to create node."));

    object_id_t new_port_id = oid_rewriter->get_new_object_id(port->get_object_id());
    port_elem->set_attribute("id", number_to_string<object_id_t>(new_port_id));

    if(port->get_name().size() > 0) port_elem->set_attribute("name", port->get_name());
    if(port->get_description().size() > 0) port_elem->set_attribute("description", port->get_description());

    object_id_t new_type_id = oid_rewriter->get_new_object_id(port->get_template_port_type_id());
    port_elem->set_attribute("type-id", number_to_string<object_id_t>(new_type_id));

    port_elem->set_attribute("diameter", number_to_string<diameter_t>(port->get_diameter()));

  }

}

void LogicModelExporter::add_wire(xmlpp::Element* wires_elem, Wire_shptr wire, layer_position_t layer_pos) {

  xmlpp::Element* wire_elem = wires_elem->add_child("wire");
  if(wire_elem == NULL) throw(std::runtime_error("Failed to create node."));

  object_id_t new_oid = oid_rewriter->get_new_object_id(wire->get_object_id());
  wire_elem->set_attribute("id", number_to_string<object_id_t>(new_oid));
  wire_elem->set_attribute("name", wire->get_name());
  wire_elem->set_attribute("description", wire->get_description());
  wire_elem->set_attribute("layer", number_to_string<layer_position_t>(layer_pos));
  wire_elem->set_attribute("diameter", number_to_string<unsigned int>(wire->get_diameter()));

  wire_elem->set_attribute("from-x", number_to_string<int>(wire->get_from_x()));
  wire_elem->set_attribute("from-y", number_to_string<int>(wire->get_from_y()));
  wire_elem->set_attribute("to-x", number_to_string<int>(wire->get_to_x()));
  wire_elem->set_attribute("to-y", number_to_string<int>(wire->get_to_y()));

  wire_elem->set_attribute("fill-color", to_color_string(wire->get_fill_color()));
  wire_elem->set_attribute("frame-color", to_color_string(wire->get_frame_color()));

  wire_elem->set_attribute("remote-id",
			   number_to_string<object_id_t>(wire->get_remote_object_id()));

}

void LogicModelExporter::add_via(xmlpp::Element* vias_elem, Via_shptr via, layer_position_t layer_pos) {

  xmlpp::Element* via_elem = vias_elem->add_child("via");
  if(via_elem == NULL) throw(std::runtime_error("Failed to create node."));

  object_id_t new_oid = oid_rewriter->get_new_object_id(via->get_object_id());
  via_elem->set_attribute("id", number_to_string<object_id_t>(new_oid));
  via_elem->set_attribute("name", via->get_name());
  via_elem->set_attribute("description", via->get_description());
  via_elem->set_attribute("layer", number_to_string<layer_position_t>(layer_pos));
  via_elem->set_attribute("diameter", number_to_string<unsigned int>(via->get_diameter()));

  via_elem->set_attribute("x", number_to_string<int>(via->get_x()));
  via_elem->set_attribute("y", number_to_string<int>(via->get_y()));

  via_elem->set_attribute("fill-color", to_color_string(via->get_fill_color()));
  via_elem->set_attribute("frame-color", to_color_string(via->get_frame_color()));

  via_elem->set_attribute("direction", via->get_direction_as_string());
  via_elem->set_attribute("remote-id",
			  number_to_string<object_id_t>(via->get_remote_object_id()));
}

void LogicModelExporter::add_emarker(xmlpp::Element* emarkers_elem, EMarker_shptr emarker, 
				     layer_position_t layer_pos) {

  xmlpp::Element* emarker_elem = emarkers_elem->add_child("emarker");
  if(emarker_elem == NULL) throw(std::runtime_error("Failed to create node."));

  object_id_t new_oid = oid_rewriter->get_new_object_id(emarker->get_object_id());
  emarker_elem->set_attribute("id", number_to_string<object_id_t>(new_oid));
  emarker_elem->set_attribute("name", emarker->get_name());
  emarker_elem->set_attribute("description", emarker->get_description());
  emarker_elem->set_attribute("layer", number_to_string<layer_position_t>(layer_pos));
  emarker_elem->set_attribute("diameter", number_to_string<unsigned int>(emarker->get_diameter()));

  emarker_elem->set_attribute("x", number_to_string<int>(emarker->get_x()));
  emarker_elem->set_attribute("y", number_to_string<int>(emarker->get_y()));

  emarker_elem->set_attribute("fill-color", to_color_string(emarker->get_fill_color()));
  emarker_elem->set_attribute("frame-color", to_color_string(emarker->get_frame_color()));

  emarker_elem->set_attribute("remote-id",
			      number_to_string<object_id_t>(emarker->get_remote_object_id()));
}


void LogicModelExporter::add_annotation(xmlpp::Element* annotations_elem, Annotation_shptr annotation, layer_position_t layer_pos) {

  xmlpp::Element* annotation_elem = annotations_elem->add_child("annotation");
  if(annotation_elem == NULL) throw(std::runtime_error("Failed to create node."));

  object_id_t new_oid = oid_rewriter->get_new_object_id(annotation->get_object_id());
  annotation_elem->set_attribute("id", number_to_string<object_id_t>(new_oid));
  annotation_elem->set_attribute("name", annotation->get_name());
  annotation_elem->set_attribute("description", annotation->get_description());
  annotation_elem->set_attribute("layer", number_to_string<layer_position_t>(layer_pos));
  annotation_elem->set_attribute("class-id", number_to_string<layer_position_t>(annotation->get_class_id()));

  annotation_elem->set_attribute("min-x", number_to_string<int>(annotation->get_min_x()));
  annotation_elem->set_attribute("min-y", number_to_string<int>(annotation->get_min_y()));
  annotation_elem->set_attribute("max-x", number_to_string<int>(annotation->get_max_x()));
  annotation_elem->set_attribute("max-y", number_to_string<int>(annotation->get_max_y()));

  annotation_elem->set_attribute("fill-color", to_color_string(annotation->get_fill_color()));
  annotation_elem->set_attribute("frame-color", to_color_string(annotation->get_frame_color()));

  for(Annotation::parameter_set_type::const_iterator iter = annotation->parameters_begin();
      iter != annotation->parameters_end(); ++iter) {
    annotation_elem->set_attribute(iter->first, iter->second);
  }
}


void LogicModelExporter::add_module(xmlpp::Element* modules_elem, LogicModel_shptr lmodel, Module_shptr module) {

  xmlpp::Element* this_elem = modules_elem->add_child("module");
  if(this_elem == NULL) throw(std::runtime_error("Failed to create node."));

  xmlpp::Element* module_ports_elem = this_elem->add_child("module-ports");
  xmlpp::Element* cells_elem = this_elem->add_child("cells");
  xmlpp::Element* sub_modules_elem = this_elem->add_child("modules");
  if(module_ports_elem == NULL ||
     cells_elem == NULL ||
     sub_modules_elem == NULL) throw(std::runtime_error("Failed to create node."));

  /*
    <module id="42" name="ff23" entity-type="flip-flop">

      <module-ports>
        <module-port name="d" object-id="666"/> -- connected with object 666
        <module-port name="q" object-id="667"/>
      </module-ports>

      <cells>
        <cell id="9999"/>
      </cells>

      <modules>
        ...
      </modules>

    </module>

  */
  
  // module itself

  object_id_t new_mod_id = oid_rewriter->get_new_object_id(module->get_object_id());
  this_elem->set_attribute("id", number_to_string<object_id_t>(new_mod_id));
  this_elem->set_attribute("name", module->get_name());
  this_elem->set_attribute("entity", module->get_entity_name());
  
  // write module ports
  for(Module::port_collection::const_iterator p_iter = module->ports_begin();
      p_iter != module->ports_end(); ++p_iter) {

    xmlpp::Element* mport_elem = module_ports_elem->add_child("module-port");
    if(mport_elem == NULL) throw(std::runtime_error("Failed to create node."));
    
    GatePort_shptr gport = p_iter->second;

    mport_elem->set_attribute("name", p_iter->first);
    mport_elem->set_attribute("object-id", number_to_string<object_id_t>(gport->get_object_id()));
  }

  // write standard cells
  for(Module::gate_collection::const_iterator g_iter = module->gates_begin();
      g_iter != module->gates_end(); ++g_iter) {

    xmlpp::Element* cell_elem = cells_elem->add_child("cell");
    if(cell_elem == NULL) throw(std::runtime_error("Failed to create node."));

    cell_elem->set_attribute("object-id", number_to_string<object_id_t>((*g_iter)->get_object_id()));
  }

  // write sub-modules
  for(Module::module_collection::const_iterator m_iter = module->modules_begin();
      m_iter != module->modules_end(); ++m_iter) {
    add_module(sub_modules_elem, lmodel, *m_iter);
  }

}
