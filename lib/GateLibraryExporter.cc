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
#include <GateLibraryExporter.h>
#include <FileSystem.h>
#include <ImageHelper.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <list>
#include <tr1/memory>


using namespace std;
using namespace degate;

void GateLibraryExporter::export_data(std::string const& filename, GateLibrary_shptr gate_lib)
  throw( InvalidPathException, InvalidPointerException, std::runtime_error ) {

  if(gate_lib == NULL) throw InvalidPointerException("Gate library pointer is NULL.");

  std::string directory = get_basedir(filename);

  try {

    xmlpp::Document doc;

    xmlpp::Element * root_elem = doc.create_root_node("gate-library");
    assert(root_elem != NULL);

    xmlpp::Element* templates_elem = root_elem->add_child("gate-templates");
    if(templates_elem == NULL) throw(std::runtime_error("Failed to create node."));

    add_gates(templates_elem, gate_lib, directory);

    doc.write_to_file_formatted(filename, "ISO-8859-1");

  }
  catch(const std::exception& ex) {
    std::cout << "Exception caught: " << ex.what() << std::endl;
    throw;
  }

}

void GateLibraryExporter::add_gates(xmlpp::Element* templates_elem,
				    GateLibrary_shptr gate_lib,
				    std::string const& directory)
  throw(std::runtime_error ) {

  for(GateLibrary::template_iterator iter = gate_lib->begin();
      iter != gate_lib->end(); ++iter) {

    GateTemplate_shptr gate_tmpl((*iter).second);

    xmlpp::Element* gate_elem = templates_elem->add_child("gate");
    if(gate_elem == NULL) throw(std::runtime_error("Failed to create node."));

    object_id_t new_oid = oid_rewriter->get_new_object_id(gate_tmpl->get_object_id());
    gate_elem->set_attribute("type-id", number_to_string<object_id_t>(new_oid));
    gate_elem->set_attribute("name", gate_tmpl->get_name());
    gate_elem->set_attribute("description", gate_tmpl->get_description());
    gate_elem->set_attribute("logic-class", gate_tmpl->get_logic_class());

    gate_elem->set_attribute("fill-color", to_color_string(gate_tmpl->get_fill_color()));
    gate_elem->set_attribute("frame-color", to_color_string(gate_tmpl->get_frame_color()));

    gate_elem->set_attribute("width", number_to_string<unsigned int>(gate_tmpl->get_width()));
    gate_elem->set_attribute("height", number_to_string<unsigned int>(gate_tmpl->get_height()));

    add_images(gate_elem, gate_tmpl, directory);
    add_ports(gate_elem, gate_tmpl);
    add_implementations(gate_elem, gate_tmpl, directory);
  }
}


void GateLibraryExporter::add_images(xmlpp::Element* gate_elem,
				     GateTemplate_shptr gate_tmpl,
				     std::string const& directory)
  throw(std::runtime_error ) {

  // export images

  xmlpp::Element* images_elem = gate_elem->add_child("images");
  if(images_elem == NULL) throw(std::runtime_error("Failed to create node."));

  for(GateTemplate::image_iterator img_iter = gate_tmpl->images_begin();
      img_iter != gate_tmpl->images_end(); ++img_iter) {

    Layer::LAYER_TYPE layer_type = (*img_iter).first;
    GateTemplateImage_shptr img = (*img_iter).second;
    assert(img != NULL);

    xmlpp::Element* img_elem = images_elem->add_child("image");
    if(img_elem == NULL) throw(std::runtime_error("Failed to create node."));

    img_elem->set_attribute("layer-type", Layer::get_layer_type_as_string(layer_type));

    // export the image
    object_id_t new_oid = oid_rewriter->get_new_object_id(gate_tmpl->get_object_id());
    boost::format fmter("%1%_%2%.tif");
    fmter % new_oid % Layer::get_layer_type_as_string(layer_type);
    std::string filename(fmter.str());

    img_elem->set_attribute("image", filename);

    save_image<GateTemplateImage>(join_pathes(directory, filename), img);
  }

}

void GateLibraryExporter::add_ports(xmlpp::Element* gate_elem,
				    GateTemplate_shptr gate_tmpl) {

  xmlpp::Element* ports_elem = gate_elem->add_child("ports");
  if(ports_elem == NULL) throw(std::runtime_error("Failed to create node."));

  for(GateTemplate::port_iterator piter = gate_tmpl->ports_begin();
      piter != gate_tmpl->ports_end(); ++piter) {

    xmlpp::Element* port_elem = ports_elem->add_child("port");
    if(port_elem == NULL) throw(std::runtime_error("Failed to create node."));

    GateTemplatePort_shptr tmpl_port((*piter));

    object_id_t new_port_id = oid_rewriter->get_new_object_id(tmpl_port->get_object_id());
    port_elem->set_attribute("id", number_to_string<object_id_t>(new_port_id));
    port_elem->set_attribute("name", tmpl_port->get_name());
    port_elem->set_attribute("description", tmpl_port->get_description());

    port_elem->set_attribute("type", tmpl_port->get_port_type_as_string());

    if(tmpl_port->is_position_defined()) {
      Point const & point = tmpl_port->get_point();
      port_elem->set_attribute("x", number_to_string<int>(point.get_x()));
      port_elem->set_attribute("y", number_to_string<int>(point.get_y()));
    }

  }
}

void GateLibraryExporter::add_implementations(xmlpp::Element* gate_elem,
					      GateTemplate_shptr gate_tmpl,
					      std::string const& directory) {

  xmlpp::Element* implementations_elem = gate_elem->add_child("implementations");
  if(implementations_elem == NULL) throw(std::runtime_error("Failed to create node."));

  for(GateTemplate::implementation_iter iter = gate_tmpl->implementations_begin();
      iter != gate_tmpl->implementations_end(); ++iter) {


    GateTemplate::IMPLEMENTATION_TYPE t = iter->first;
    std::string const& code = iter->second;

    std::cout << "Code: " << code;
    if(t != GateTemplate::UNDEFINED && !code.empty()) {

      xmlpp::Element* impl_elem = implementations_elem->add_child("implementation");
      if(impl_elem == NULL) throw(std::runtime_error("Failed to create node."));

      object_id_t new_oid = oid_rewriter->get_new_object_id(gate_tmpl->get_object_id());
      boost::format fmter("%1%%2%.%3%");
      switch(t) {
      case GateTemplate::TEXT: fmter % "" % new_oid % "txt"; break;
      case GateTemplate::VHDL: fmter % "" % new_oid % "vhdl"; break;
      case GateTemplate::VHDL_TESTBENCH: fmter % "test_" % new_oid % "vhdl"; break;
      case GateTemplate::VERILOG: fmter % "" % new_oid % "v"; break;
      case GateTemplate::VERILOG_TESTBENCH: fmter % "test_" % new_oid % "v"; break;
      default: assert(1==0); // already handled. just to get rid of a compiler warning.
      }
      std::string filename(fmter.str());


      std::ofstream myfile;
      myfile.open(join_pathes(directory, filename).c_str());
      myfile << code;
      myfile.close();

      impl_elem->set_attribute("type", GateTemplate::get_impl_type_as_string(t));
      impl_elem->set_attribute("file", filename);
    }
  }
}
