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
#include <LogicModelImporter.h>
#include <SubProjectAnnotation.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include <tr1/memory>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <list>

#include <boost/format.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace degate;

void LogicModelImporter::import_into(LogicModel_shptr lmodel,
				     std::string const& filename) {
  if(RET_IS_NOT_OK(check_file(filename))) {
    debug(TM, "Problem: file %s not found.", filename.c_str());
    throw InvalidPathException("Can't load logic model from file.");
  }

  try {
    //debug(TM, "try to parse file %s", filename.c_str());

    xmlpp::DomParser parser;
    parser.set_substitute_entities(); // We just want the text to be resolved/unescaped automatically.

    parser.parse_file(filename);
    assert(parser == true);

    const xmlpp::Document * doc = parser.get_document();
    assert(doc != NULL);

    const xmlpp::Element * root_elem = doc->get_root_node(); // deleted by DomParser
    assert(root_elem != NULL);

    lmodel->set_gate_library(gate_library);

    parse_logic_model_element(root_elem, lmodel);

  }
  catch(const std::exception& ex) {
    std::cout << "Exception caught: " << ex.what() << std::endl;
    throw;
  }

}

LogicModel_shptr LogicModelImporter::import(std::string const& filename) {

  LogicModel_shptr lmodel(new LogicModel(width, height));
  assert(lmodel != NULL);

  import_into(lmodel, filename);

  return lmodel;
}

void LogicModelImporter::parse_logic_model_element(const xmlpp::Element * const lm_elem,
						   LogicModel_shptr lmodel) {


  const xmlpp::Element * gates_elem = get_dom_twig(lm_elem, "gates");
  if(gates_elem != NULL) parse_gates_element(gates_elem, lmodel);

  const xmlpp::Element * vias_elem = get_dom_twig(lm_elem, "vias");
  if(vias_elem != NULL) parse_vias_element(vias_elem, lmodel);

  const xmlpp::Element * emarkers_elem = get_dom_twig(lm_elem, "emarkers");
  if(emarkers_elem != NULL) parse_emarkers_element(emarkers_elem, lmodel);

  const xmlpp::Element * wires_elem = get_dom_twig(lm_elem, "wires");
  if(wires_elem != NULL) parse_wires_element(wires_elem, lmodel);

  const xmlpp::Element * nets_elem = get_dom_twig(lm_elem, "nets");
  if(nets_elem != NULL) parse_nets_element(nets_elem, lmodel);

  const xmlpp::Element * annotations_elem = get_dom_twig(lm_elem, "annotations");
  if(annotations_elem != NULL) parse_annotations_element(annotations_elem, lmodel);

  const xmlpp::Element * modules_elem = get_dom_twig(lm_elem, "modules");
  if(modules_elem != NULL) {
    std::list<Module_shptr> mods = parse_modules_element(modules_elem, lmodel);
    assert(mods.size() == 1);    
    lmodel->set_main_module(mods.front());
  }

}

void LogicModelImporter::parse_nets_element(const xmlpp::Element * const nets_element,
					    LogicModel_shptr lmodel) {

  if(nets_element == NULL || lmodel == NULL)
    throw InvalidPointerException("Got a NULL pointer in  LogicModelImporter::parse_nets_element()");

  const xmlpp::Node::NodeList net_list = nets_element->get_children("net");
  for(xmlpp::Node::NodeList::const_iterator iter = net_list.begin();
      iter != net_list.end();
      ++iter) {

    int object_counter = 0;
    if(const xmlpp::Element* net_elem = dynamic_cast<const xmlpp::Element*>(*iter)) {

      object_id_t net_id = parse_number<object_id_t>(net_elem, "id");

      Net_shptr net(new Net());
      net->set_object_id(net_id);

      const xmlpp::Node::NodeList connection_list = net_elem->get_children("connection");
      for(xmlpp::Node::NodeList::const_iterator iter2 = connection_list.begin();
	  iter2 != connection_list.end();
	  ++iter2) {

	if(const xmlpp::Element* conn_elem = dynamic_cast<const xmlpp::Element*>(*iter2)) {

	  object_id_t object_id = parse_number<object_id_t>(conn_elem, "object-id");

	  // add connection
	  try {
	    PlacedLogicModelObject_shptr placed_object = lmodel->get_object(object_id);
	    if(placed_object == NULL) {
	      debug(TM,
		    "Failed to lookup logic model object %d. Can't connect it to net %d.",
		    object_id, net_id);
	    }
	    else {
	      ConnectedLogicModelObject_shptr o =
		std::tr1::dynamic_pointer_cast<ConnectedLogicModelObject>(placed_object);
	      if(o != NULL) {
		o->set_net(net);
	      }
	      else {
		debug(TM, "Failed to dynamic_cast<> a logic model object with ID %d", object_id);
	      }
	    }

	  }
	  catch(CollectionLookupException const & ex) {
	    debug(TM,
		  "Failed to insert a connection for net %d into the logic layer. "
		  "Can't lookup logic model object %d that should be connected to that net.",
		  net_id, object_id);
	    throw; // rethrow
	  }
	} // end of if

	object_counter++;
      } // end of for

      if(object_counter < 2) {
	boost::format f("Net with ID %1% has only a single object. This should not occur.");
	f % net_id;
	std::cout << "WARNING: " << f.str() << std::endl;
	//throw DegateLogicException(f.str());
      }
      lmodel->add_net(net);
    }
  }
}

void LogicModelImporter::parse_wires_element(const xmlpp::Element * const wires_element,
					    LogicModel_shptr lmodel) {

  if(wires_element == NULL || lmodel == NULL)
    throw InvalidPointerException("Null pointer in LogicModelImporter::parse_wires_element()");

  const xmlpp::Node::NodeList wire_list = wires_element->get_children("wire");
  for(xmlpp::Node::NodeList::const_iterator iter = wire_list.begin();
      iter != wire_list.end();
      ++iter) {

    if(const xmlpp::Element* wire_elem = dynamic_cast<const xmlpp::Element*>(*iter)) {

      // XXX PORT ID REPLACER ...

      object_id_t object_id = parse_number<object_id_t>(wire_elem, "id");
      int from_x = parse_number<int>(wire_elem, "from-x");
      int from_y = parse_number<int>(wire_elem, "from-y");
      int to_x = parse_number<int>(wire_elem, "to-x");
      int to_y = parse_number<int>(wire_elem, "to-y");
      int diameter = parse_number<int>(wire_elem, "diameter");
      int layer = parse_number<int>(wire_elem, "layer");
      int remote_id = parse_number<object_id_t>(wire_elem, "remote-id", 0);

      const Glib::ustring name(wire_elem->get_attribute_value("name"));
      const Glib::ustring description(wire_elem->get_attribute_value("description"));
      const Glib::ustring fill_color_str(wire_elem->get_attribute_value("fill-color"));
      const Glib::ustring frame_color_str(wire_elem->get_attribute_value("frame-color"));


      Wire_shptr wire(new Wire(from_x, from_y, to_x, to_y, diameter));
      wire->set_name(name.c_str());
      wire->set_description(description.c_str());
      wire->set_object_id(object_id);
      wire->set_fill_color(parse_color_string(fill_color_str));
      wire->set_frame_color(parse_color_string(frame_color_str));

      wire->set_remote_object_id(remote_id);
      lmodel->add_object(layer, wire);
    }
  }
}

void LogicModelImporter::parse_vias_element(const xmlpp::Element * const vias_element,
					    LogicModel_shptr lmodel) {

  if(vias_element == NULL || lmodel == NULL) throw InvalidPointerException();

  const xmlpp::Node::NodeList via_list = vias_element->get_children("via");
  for(xmlpp::Node::NodeList::const_iterator iter = via_list.begin();
      iter != via_list.end();
      ++iter) {

    if(const xmlpp::Element* via_elem = dynamic_cast<const xmlpp::Element*>(*iter)) {

      // XXX PORT ID REPLACER ...

      object_id_t object_id = parse_number<object_id_t>(via_elem, "id");
      int x = parse_number<int>(via_elem, "x");
      int y = parse_number<int>(via_elem, "y");
      int diameter = parse_number<int>(via_elem, "diameter");
      int layer = parse_number<int>(via_elem, "layer");
      int remote_id = parse_number<object_id_t>(via_elem, "remote-id", 0);

      const Glib::ustring name(via_elem->get_attribute_value("name"));
      const Glib::ustring description(via_elem->get_attribute_value("description"));
      const Glib::ustring fill_color_str(via_elem->get_attribute_value("fill-color"));
      const Glib::ustring frame_color_str(via_elem->get_attribute_value("frame-color"));
      const Glib::ustring direction_str(via_elem->get_attribute_value("direction").lowercase());

      Via::DIRECTION direction;
      if(direction_str == "undefined") direction = Via::DIRECTION_UNDEFINED;
      else if(direction_str == "up") direction = Via::DIRECTION_UP;
      else if(direction_str == "down") direction = Via::DIRECTION_DOWN;
      else {
	boost::format f("Can't parse via direction type: %1%");
	f % direction_str;
	throw XMLAttributeParseException(f.str());
      }

      Via_shptr via(new Via(x, y, diameter, direction));
      via->set_name(name.c_str());
      via->set_description(description.c_str());
      via->set_object_id(object_id);
      via->set_fill_color(parse_color_string(fill_color_str));
      via->set_frame_color(parse_color_string(frame_color_str));

      via->set_remote_object_id(remote_id);

      lmodel->add_object(layer, via);
    }
  }
}

void LogicModelImporter::parse_emarkers_element(const xmlpp::Element * const emarkers_element,
						LogicModel_shptr lmodel) {

  if(emarkers_element == NULL || lmodel == NULL) throw InvalidPointerException();

  const xmlpp::Node::NodeList emarker_list = emarkers_element->get_children("emarker");
  for(xmlpp::Node::NodeList::const_iterator iter = emarker_list.begin();
      iter != emarker_list.end();
      ++iter) {

    if(const xmlpp::Element* emarker_elem = dynamic_cast<const xmlpp::Element*>(*iter)) {

      // XXX PORT ID REPLACER ...

      object_id_t object_id = parse_number<object_id_t>(emarker_elem, "id");
      int x = parse_number<int>(emarker_elem, "x");
      int y = parse_number<int>(emarker_elem, "y");
      int diameter = parse_number<diameter_t>(emarker_elem, "diameter");
      int layer = parse_number<int>(emarker_elem, "layer");
      int remote_id = parse_number<object_id_t>(emarker_elem, "remote-id", 0);

      const Glib::ustring name(emarker_elem->get_attribute_value("name"));
      const Glib::ustring description(emarker_elem->get_attribute_value("description"));
      const Glib::ustring fill_color_str(emarker_elem->get_attribute_value("fill-color"));
      const Glib::ustring frame_color_str(emarker_elem->get_attribute_value("frame-color"));
      const Glib::ustring direction_str(emarker_elem->get_attribute_value("direction").lowercase());

      EMarker_shptr emarker(new EMarker(x, y, diameter));
      emarker->set_name(name.c_str());
      emarker->set_description(description.c_str());
      emarker->set_object_id(object_id);
      emarker->set_fill_color(parse_color_string(fill_color_str));
      emarker->set_frame_color(parse_color_string(frame_color_str));

      emarker->set_remote_object_id(remote_id);

      lmodel->add_object(layer, emarker);
    }
  }
}

void LogicModelImporter::parse_gates_element(const xmlpp::Element * const gates_element,
					     LogicModel_shptr lmodel) {

  if(gates_element == NULL || lmodel == NULL) throw InvalidPointerException();

  const xmlpp::Node::NodeList gate_list = gates_element->get_children("gate");
  for(xmlpp::Node::NodeList::const_iterator iter = gate_list.begin();
      iter != gate_list.end();
      ++iter) {

    if(const xmlpp::Element* gate_elem = dynamic_cast<const xmlpp::Element*>(*iter)) {


      object_id_t object_id = parse_number<object_id_t>(gate_elem, "id");
      int min_x = parse_number<int>(gate_elem, "min-x");
      int min_y = parse_number<int>(gate_elem, "min-y");
      int max_x = parse_number<int>(gate_elem, "max-x");
      int max_y = parse_number<int>(gate_elem, "max-y");

      int layer = parse_number<int>(gate_elem, "layer");

      int gate_type_id = parse_number<int>(gate_elem, "type-id");
      const Glib::ustring name(gate_elem->get_attribute_value("name"));
      const Glib::ustring description(gate_elem->get_attribute_value("description"));
      const Glib::ustring orientation_str(gate_elem->get_attribute_value("orientation").lowercase());
      const Glib::ustring frame_color_str(gate_elem->get_attribute_value("frame-color"));
      const Glib::ustring fill_color_str(gate_elem->get_attribute_value("fill-color"));

      Gate::ORIENTATION orientation;
      if(orientation_str == "undefined") orientation = Gate::ORIENTATION_UNDEFINED;
      else if(orientation_str == "normal") orientation = Gate::ORIENTATION_NORMAL;
      else if(orientation_str == "flipped-left-right") orientation = Gate::ORIENTATION_FLIPPED_LEFT_RIGHT;
      else if(orientation_str == "flipped-up-down") orientation = Gate::ORIENTATION_FLIPPED_UP_DOWN;
      else if(orientation_str == "flipped-both") orientation = Gate::ORIENTATION_FLIPPED_BOTH;
      else throw XMLAttributeParseException("Can't parse orientation type.");

      // create a new gate and add it into the logic model

      Gate_shptr gate(new Gate(min_x, max_x, min_y, max_y, orientation));
      gate->set_name(name.c_str());
      gate->set_description(description.c_str());
      gate->set_object_id(object_id);
      gate->set_template_type_id(gate_type_id);
      gate->set_fill_color(parse_color_string(fill_color_str));
      gate->set_frame_color(parse_color_string(frame_color_str));

      if(gate_library != NULL) {
	GateTemplate_shptr tmpl = gate_library->get_template(gate_type_id);
	assert(tmpl != NULL);
	gate->set_gate_template(tmpl);
      }

      // parse port instances
      const xmlpp::Node::NodeList port_list = gate_elem->get_children("port");
      for(xmlpp::Node::NodeList::const_iterator iter2 = port_list.begin();
	  iter2 != port_list.end();
	  ++iter2) {

	if(const xmlpp::Element* port_elem = dynamic_cast<const xmlpp::Element*>(*iter2)) {

	  object_id_t template_port_id = parse_number<object_id_t>(port_elem, "type-id");

	  // create a new port
	  GatePort_shptr gate_port(new GatePort(gate));
	  gate_port->set_object_id(parse_number<object_id_t>(port_elem, "id"));
	  gate_port->set_template_port_type_id(template_port_id);
	  gate_port->set_diameter(parse_number<diameter_t>(port_elem, "diameter", 5));

	  if(gate_library != NULL) {
	    GateTemplatePort_shptr tmpl_port = gate_library->get_template_port(template_port_id);
	    gate_port->set_template_port(tmpl_port);
	  }

	  gate->add_port(gate_port);
	}
      }

      lmodel->add_object(layer, gate);
      lmodel->update_ports(gate);
      gate->print();
    }
  }

}


void LogicModelImporter::parse_annotations_element(const xmlpp::Element * const annotations_element,
						   LogicModel_shptr lmodel) {

  if(annotations_element == NULL || lmodel == NULL) throw InvalidPointerException();

  const xmlpp::Node::NodeList annotation_list = annotations_element->get_children("annotation");
  for(xmlpp::Node::NodeList::const_iterator iter = annotation_list.begin();
      iter != annotation_list.end();
      ++iter) {

    if(const xmlpp::Element* annotation_elem = dynamic_cast<const xmlpp::Element*>(*iter)) {

      object_id_t object_id = parse_number<object_id_t>(annotation_elem, "id");

      int min_x = parse_number<int>(annotation_elem, "min-x");
      int min_y = parse_number<int>(annotation_elem, "min-y");
      int max_x = parse_number<int>(annotation_elem, "max-x");
      int max_y = parse_number<int>(annotation_elem, "max-y");

      int layer = parse_number<int>(annotation_elem, "layer");
      Annotation::class_id_t class_id = parse_number<Annotation::class_id_t>(annotation_elem, "class-id");

      const Glib::ustring name(annotation_elem->get_attribute_value("name"));
      const Glib::ustring description(annotation_elem->get_attribute_value("description"));
      const Glib::ustring fill_color_str(annotation_elem->get_attribute_value("fill-color"));
      const Glib::ustring frame_color_str(annotation_elem->get_attribute_value("frame-color"));


      Annotation_shptr annotation;

      if(class_id == Annotation::SUBPROJECT) {
	const std::string path = annotation_elem->get_attribute_value("subproject-directory");
	annotation = Annotation_shptr(new SubProjectAnnotation(min_x, max_x, min_y, max_y, path));
      }
      else
	annotation = Annotation_shptr(new Annotation(min_x, max_x, min_y, max_y, class_id));

      annotation->set_name(name.c_str());
      annotation->set_description(description.c_str());
      annotation->set_object_id(object_id);
      annotation->set_fill_color(parse_color_string(fill_color_str));
      annotation->set_frame_color(parse_color_string(frame_color_str));

      lmodel->add_object(layer, annotation);
    }
  }
}

std::list<Module_shptr> LogicModelImporter::parse_modules_element(const xmlpp::Element * const modules_element,
						       LogicModel_shptr lmodel) {

  if(modules_element == NULL || lmodel == NULL)
    throw InvalidPointerException("Got a NULL pointer in  LogicModelImporter::parse_modules_element()");

  std::list<Module_shptr> modules;

  const xmlpp::Node::NodeList module_list = modules_element->get_children("module");

  for(xmlpp::Node::NodeList::const_iterator m_iter = module_list.begin();  m_iter != module_list.end(); ++m_iter) {


    if(const xmlpp::Element* module_elem = dynamic_cast<const xmlpp::Element*>(*m_iter)) {

      // parse module attributes
      object_id_t id = parse_number<object_id_t>(module_elem, "id");
      const Glib::ustring name(module_elem->get_attribute_value("name"));
      const Glib::ustring entity(module_elem->get_attribute_value("entity"));

      Module_shptr module(new Module(name, entity));
      module->set_object_id(id);

      // parse standard cell list
      const xmlpp::Element * cells_elem = get_dom_twig(module_elem, "cells");
      if(cells_elem != NULL) {

	const xmlpp::Node::NodeList cell_list = cells_elem->get_children("cell");
	for(xmlpp::Node::NodeList::const_iterator c_iter = cell_list.begin();
	    c_iter != cell_list.end(); ++c_iter) {

	  if(const xmlpp::Element* cell_elem = dynamic_cast<const xmlpp::Element*>(*c_iter)) {	  
	    object_id_t cell_id = parse_number<object_id_t>(cell_elem, "object-id");

	    // Lookup will throw an exception, if cell is not in the logic model. This is intented behaviour.
	    if(Gate_shptr gate = std::tr1::dynamic_pointer_cast<Gate>(lmodel->get_object(cell_id)))
	      module->add_gate(gate, /* autodetect module ports = */ false);
	  }
	}
      }

      // parse module ports
      const xmlpp::Element * mports_elem = get_dom_twig(module_elem, "module-ports");
      if(mports_elem != NULL) {

	const xmlpp::Node::NodeList mport_list = mports_elem->get_children("module-port");
	for(xmlpp::Node::NodeList::const_iterator mp_iter = mport_list.begin();
	    mp_iter != mport_list.end(); ++mp_iter) {

	  if(const xmlpp::Element* mport_elem = dynamic_cast<const xmlpp::Element*>(*mp_iter)) {

	    const Glib::ustring port_name(mport_elem->get_attribute_value("name"));
	    object_id_t ref_id = parse_number<object_id_t>(mport_elem, "object-id");
	    
	    // Lookup will throw an exception, if cell is not in the logic model. This is intented behaviour.
	    if(GatePort_shptr gport = std::tr1::dynamic_pointer_cast<GatePort>(lmodel->get_object(ref_id)))
	      module->add_module_port(port_name, gport);
	  }
	}
      }


      // parse sub-modules
      const xmlpp::Element * sub_modules_elem = get_dom_twig(module_elem, "modules");
      if(sub_modules_elem != NULL) {
	std::list<Module_shptr> sub_modules = parse_modules_element(sub_modules_elem, lmodel);
	BOOST_FOREACH(Module_shptr submod, sub_modules) module->add_module(submod);
      }


      modules.push_back(module);

    }
  }

  return modules;
}
