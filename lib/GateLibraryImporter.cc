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
#include <GateLibraryImporter.h>
#include <ImageHelper.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <list>
#include <tr1/memory>

using namespace std;
using namespace degate;

GateLibrary_shptr GateLibraryImporter::import(std::string const& filename) {

  if(RET_IS_NOT_OK(check_file(filename))) {
    debug(TM, "Problem: file %s not found.", filename.c_str());
    throw InvalidPathException("Can't load gate library from file.");
  }

  std::string directory = get_basedir(filename);

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

    return parse_gate_library_element(root_elem, directory);

  }
  catch(const std::exception& ex) {
    std::cout << "Exception caught: " << ex.what() << std::endl;
    throw;
  }



}

GateLibrary_shptr GateLibraryImporter::parse_gate_library_element(const xmlpp::Element * const gt_elem,
								  std::string const& directory) {

  // parse width and height
  GateLibrary_shptr gate_lib(new GateLibrary());
  assert(gate_lib != NULL);

  const xmlpp::Element * e = get_dom_twig(gt_elem, "gate-templates");
  if(e != NULL) parse_gate_templates_element(e, gate_lib, directory);

  return gate_lib;
}

void GateLibraryImporter::parse_gate_templates_element(const xmlpp::Element * const gate_template_element,
						       GateLibrary_shptr gate_lib,
						       std::string const& directory) {

  if(gate_template_element == NULL || gate_lib == NULL) throw(InvalidPointerException("Invalid pointer"));

  const xmlpp::Node::NodeList gate_list = gate_template_element->get_children("gate");
  for(xmlpp::Node::NodeList::const_iterator iter = gate_list.begin();
      iter != gate_list.end();
      ++iter) {

    if(const xmlpp::Element* gate_elem = dynamic_cast<const xmlpp::Element*>(*iter)) {

      object_id_t object_id = parse_number<object_id_t>(gate_elem, "type-id");
      int min_x = parse_number<int>(gate_elem, "min-x", 0);
      int min_y = parse_number<int>(gate_elem, "min-y", 0);
      int max_x = parse_number<int>(gate_elem, "max-x", 0);
      int max_y = parse_number<int>(gate_elem, "max-y", 0);

      int width = parse_number<int>(gate_elem, "width", 0);
      int height = parse_number<int>(gate_elem, "height", 0);

      const Glib::ustring name(gate_elem->get_attribute_value("name"));
      const Glib::ustring description(gate_elem->get_attribute_value("description"));
      const Glib::ustring logic_class(gate_elem->get_attribute_value("logic-class"));
      const Glib::ustring frame_color_str(gate_elem->get_attribute_value("frame-color"));
      const Glib::ustring fill_color_str(gate_elem->get_attribute_value("fill-color"));

      GateTemplate_shptr gate_template;

      if(min_x == 0 && min_y == 0 && max_x == 0 && max_y == 0) {
	gate_template = GateTemplate_shptr(new GateTemplate(width, height));
      }
      else {
	gate_template = GateTemplate_shptr(new GateTemplate(min_x, max_x, min_y, max_y));
      }

      gate_template->set_name(string(name.c_str()));
      gate_template->set_description(string(description.c_str()));
      gate_template->set_logic_class(string(logic_class.c_str()));
      gate_template->set_object_id(object_id);
      gate_template->set_fill_color(parse_color_string(fill_color_str));
      gate_template->set_frame_color(parse_color_string(frame_color_str));


      const xmlpp::Element * images = get_dom_twig(gate_elem, "images");
      if(images != NULL) parse_template_images_element(images, gate_template, directory);


      const xmlpp::Element * ports = get_dom_twig(gate_elem, "ports");
      if(ports != NULL) parse_template_ports_element(ports, gate_template, gate_lib);

      const xmlpp::Element * implementations = get_dom_twig(gate_elem, "implementations");
      if(implementations != NULL)
	parse_template_implementations_element(implementations, gate_template, directory);

      gate_lib->add_template(gate_template);
    }
  }

}

void GateLibraryImporter::parse_template_images_element(const xmlpp::Element * const template_images_element,
							GateTemplate_shptr gate_tmpl,
							std::string const& directory) {

  if(template_images_element == NULL ||
     gate_tmpl == NULL) throw InvalidPointerException("Invalid pointer");

  const xmlpp::Node::NodeList image_list = template_images_element->get_children("image");
  for(xmlpp::Node::NodeList::const_iterator iter = image_list.begin();
      iter != image_list.end(); ++iter) {

    if(const xmlpp::Element* image_elem = dynamic_cast<const xmlpp::Element*>(*iter)) {
      const std::string layer_type_str(image_elem->get_attribute_value("layer-type"));
      const std::string image_file(image_elem->get_attribute_value("image"));

      Layer::LAYER_TYPE layer_type = Layer::get_layer_type_from_string(layer_type_str);
      GateTemplateImage_shptr img = load_image<GateTemplateImage>(join_pathes(directory, image_file));

      assert(img != NULL);
      gate_tmpl->set_image(layer_type, img);
    }
  }

}

void GateLibraryImporter::parse_template_implementations_element(const xmlpp::Element * const implementations_element,
								 GateTemplate_shptr gate_tmpl,
								 std::string const& directory) {

  if(implementations_element == NULL ||
     gate_tmpl == NULL) throw InvalidPointerException("Invalid pointer");

  const xmlpp::Node::NodeList impl_list = implementations_element->get_children("implementation");
  for(xmlpp::Node::NodeList::const_iterator iter = impl_list.begin();
      iter != impl_list.end(); ++iter) {

    if(const xmlpp::Element* impl_elem = dynamic_cast<const xmlpp::Element*>(*iter)) {
      const std::string impl_type_str(impl_elem->get_attribute_value("type"));
      const std::string impl_file_attr(impl_elem->get_attribute_value("file"));
      const std::string impl_file(join_pathes(directory, impl_file_attr));

      if(!impl_file_attr.empty()) {
	GateTemplate::IMPLEMENTATION_TYPE impl_type;
	try {
	  impl_type = GateTemplate::get_impl_type_from_string(impl_type_str);
	}
	catch(DegateRuntimeException const &ex) {
	  boost::format f("Can't interprete attribute %1% while reading gate library (%2%)");
	  f % impl_type_str % ex.what();
	  throw XMLAttributeParseException(f.str());
	}

	debug(TM, "Parsing file '%s'", impl_file.c_str());
	std::ifstream myfile(impl_file.c_str());
	std::string line, code;
	if(myfile.is_open()) {
	  while (!myfile.eof()) {
	    getline(myfile, line);
	    code.append(line);
	    code.append("\n");
	  }
	  myfile.close();
	  gate_tmpl->set_implementation(impl_type, code);
	  gate_tmpl->get_implementation(impl_type);
	}
	else {
	  boost::format f("Can't open file %1%");
	  f % impl_file;
	  throw FileSystemException(f.str());
	}

      }
    }
  }

}


void GateLibraryImporter::parse_template_ports_element(const xmlpp::Element * const template_ports_element,
						       GateTemplate_shptr gate_tmpl,
						       GateLibrary_shptr gate_lib) {

  if(template_ports_element == NULL ||
     gate_tmpl == NULL) throw InvalidPointerException("Invalid pointer");

  const xmlpp::Node::NodeList port_list = template_ports_element->get_children("port");
  for(xmlpp::Node::NodeList::const_iterator iter = port_list.begin();
      iter != port_list.end();
      ++iter) {

    if(const xmlpp::Element* port_elem = dynamic_cast<const xmlpp::Element*>(*iter)) {

      object_id_t object_id = parse_number<object_id_t>(port_elem, "id");

      // we check if the object id is somehow present in the logic model
      if(gate_lib->exists_template_port(object_id) || gate_lib->exists_template(object_id)) {
	boost::format f("Error: The object ID %1% is used twice in the gate library.");	
	f % object_id;
	std::cout << f.str() << "\n";
	throw DegateInconsistencyException(f.str());
      }


      const std::string name(port_elem->get_attribute_value("name"));
      const std::string description(port_elem->get_attribute_value("description"));
      const std::string type_str(port_elem->get_attribute_value("type").lowercase());

      GateTemplatePort::PORT_TYPE port_type = GateTemplatePort::get_port_type_from_string(type_str);

      GateTemplatePort_shptr tmpl_port;

      int pos_x, pos_y;
      try {
	pos_x = parse_number<int>(port_elem, "x");
	pos_y = parse_number<int>(port_elem, "y");

	tmpl_port = GateTemplatePort_shptr(new GateTemplatePort(pos_x, pos_y, port_type));
      }
      catch(XMLAttributeMissingException const & ex) {
	tmpl_port = GateTemplatePort_shptr(new GateTemplatePort(port_type));
      }
      assert(tmpl_port != NULL);


      tmpl_port->set_name(string(name.c_str()));
      tmpl_port->set_description(string(description.c_str()));
      tmpl_port->set_object_id(object_id);

      // add port to the gate template
      gate_tmpl->add_template_port(tmpl_port);
    }


  }
}
