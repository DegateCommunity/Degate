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

#include <globals.h>
#include <Layer.h>
#include <ProjectExporter.h>
#include <FileSystem.h>
#include <ObjectIDRewriter.h>
#include <LogicModelExporter.h>
#include <GateLibraryExporter.h>
#include <RCVBlacklistExporter.h>

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
#include <boost/foreach.hpp>

using namespace std;
using namespace degate;

void ProjectExporter::export_all(std::string const& project_directory, Project_shptr prj,
				 bool enable_oid_rewrite,
				 std::string const& project_file,
				 std::string const& lmodel_file,
				 std::string const& gatelib_file,
				 std::string const& rcbl_file) {

  if(!is_directory(project_directory)) {
    throw InvalidPathException("The path where the project should be exported to is not a directory.");
  }
  else {
    ObjectIDRewriter_shptr oid_rewriter(new ObjectIDRewriter(enable_oid_rewrite));

    export_data(join_pathes(project_directory, project_file), prj);

    LogicModel_shptr lmodel = prj->get_logic_model();

    if(lmodel != NULL) {
      LogicModelExporter lm_exporter(oid_rewriter);
      string lm_filename(join_pathes(project_directory, lmodel_file));
      lm_exporter.export_data(lm_filename, lmodel);


      RCVBlacklistExporter rcv_exporter(oid_rewriter);
      rcv_exporter.export_data(join_pathes(project_directory, rcbl_file), prj->get_rcv_blacklist());

      GateLibrary_shptr glib = lmodel->get_gate_library();
      if(glib != NULL) {

	GateLibraryExporter gl_exporter(oid_rewriter);
	gl_exporter.export_data(join_pathes(project_directory, gatelib_file), glib);
      }
    }
  }
}

void ProjectExporter::export_data(std::string const& filename, Project_shptr prj) {

  if(prj == NULL) throw InvalidPointerException("Project pointer is NULL.");

  try {

    xmlpp::Document doc;

    xmlpp::Element * root_elem = doc.create_root_node("project");
    assert(root_elem != NULL);
    set_project_node_attributes(root_elem, prj);

    add_layers(root_elem, prj->get_logic_model(), prj->get_project_directory());
    add_grids(root_elem, prj);
    add_colors(root_elem, prj);
    add_port_colors(root_elem, prj->get_port_color_manager());

    doc.write_to_file_formatted(filename, "ISO-8859-1");

  }
  catch(const std::exception& ex) {
    std::cout << "Exception caught: " << ex.what() << std::endl;
    throw;
  }

}

void ProjectExporter::add_grids(xmlpp::Element* prj_elem, Project_shptr prj) {

  xmlpp::Element* grids_elem = prj_elem->add_child("grids");
  if(grids_elem == NULL) throw(std::runtime_error("Failed to create node."));


  add_regular_grid(grids_elem, prj->get_regular_horizontal_grid(), "horizontal");
  add_regular_grid(grids_elem, prj->get_regular_vertical_grid(), "vertical");

  add_irregular_grid(grids_elem, prj->get_irregular_horizontal_grid(), "horizontal");
  add_irregular_grid(grids_elem, prj->get_irregular_vertical_grid(), "vertical");
}

void ProjectExporter::add_regular_grid(xmlpp::Element* grids_elem,
				       const RegularGrid_shptr grid,
				       std::string const & grid_orientation) {

  xmlpp::Element* grid_elem = grids_elem->add_child("regular-grid");
  if(grid_elem == NULL) throw(std::runtime_error("Failed to create node."));

  grid_elem->set_attribute("enabled", grid->is_enabled() ? "true" : "false");
  grid_elem->set_attribute("distance", number_to_string<double>(grid->get_distance()));
  grid_elem->set_attribute("offset", number_to_string<int>(grid->get_min()));
  grid_elem->set_attribute("orientation", grid_orientation);


}

void ProjectExporter::add_irregular_grid(xmlpp::Element* grids_elem,
					 const IrregularGrid_shptr grid,
					 std::string const & grid_orientation) {

  xmlpp::Element* grid_elem = grids_elem->add_child("irregular-grid");
  if(grid_elem == NULL) throw(std::runtime_error("Failed to create node."));

  grid_elem->set_attribute("enabled", grid->is_enabled() ? "true" : "false");
  grid_elem->set_attribute("orientation", grid_orientation);

  xmlpp::Element* offsets_elem = grid_elem->add_child("offsets");
  if(offsets_elem == NULL) throw(std::runtime_error("Failed to create node."));

  for(IrregularGrid::grid_iter iter = grid->begin();
      iter != grid->end(); ++iter) {

    xmlpp::Element* offset_elem = offsets_elem->add_child("offset-entry");
    if(offset_elem == NULL) throw(std::runtime_error("Failed to create node."));

    offset_elem->set_attribute("offset", number_to_string<int>(*iter));
  }

}


void ProjectExporter::set_project_node_attributes(xmlpp::Element* prj_elem,
						  Project_shptr prj) {

  prj_elem->set_attribute("degate-version", prj->get_degate_version());
  prj_elem->set_attribute("name", prj->get_name());
  prj_elem->set_attribute("description", prj->get_description());
  prj_elem->set_attribute("width", number_to_string<int>(prj->get_width()));
  prj_elem->set_attribute("height", number_to_string<int>(prj->get_height()));

  prj_elem->set_attribute("lambda", number_to_string<length_t>(prj->get_lambda()));
  prj_elem->set_attribute("pin-diameter", number_to_string<length_t>(prj->get_default_pin_diameter()));
  prj_elem->set_attribute("wire-diameter", number_to_string<length_t>(prj->get_default_wire_diameter()));
  prj_elem->set_attribute("port-diameter", number_to_string<length_t>(prj->get_default_port_diameter()));

  prj_elem->set_attribute("pixel-per-um", number_to_string<double>(prj->get_pixel_per_um()));
  prj_elem->set_attribute("template-dimension", number_to_string<int>(prj->get_template_dimension()));
  prj_elem->set_attribute("font-size", number_to_string<unsigned int>(prj->get_font_size()));

  prj_elem->set_attribute("server-url", prj->get_server_url());
  prj_elem->set_attribute("last-pulled-transaction-id",
			  number_to_string<transaction_id_t>(prj->get_last_pulled_tid()));
}


void ProjectExporter::add_layers(xmlpp::Element* prj_elem,
				 LogicModel_shptr lmodel,
				 std::string const& project_dir) {

  if(lmodel == NULL) throw InvalidPointerException();

  xmlpp::Element* layers_elem = prj_elem->add_child("layers");
  if(layers_elem == NULL) throw(std::runtime_error("Failed to create node."));

  for(LogicModel::layer_collection::iterator layer_iter = lmodel->layers_begin();
      layer_iter != lmodel->layers_end(); ++layer_iter) {

    xmlpp::Element* layer_elem = layers_elem->add_child("layer");
    if(layer_elem == NULL) throw(std::runtime_error("Failed to create node."));

    Layer_shptr layer = *layer_iter;
    assert(layer->has_valid_layer_id());

    layer_elem->set_attribute("position", number_to_string<layer_position_t>(layer->get_layer_pos()));
    layer_elem->set_attribute("id", number_to_string<layer_id_t>(layer->get_layer_id()));
    layer_elem->set_attribute("type", layer->get_layer_type_as_string());
    layer_elem->set_attribute("description", layer->get_description());
    layer_elem->set_attribute("enabled", layer->is_enabled() ? "true" : "false");

    if(layer->has_background_image())
      layer_elem->set_attribute("image-filename",
				get_relative_path(layer->get_image_filename(), project_dir));

  }

}


void ProjectExporter::add_port_colors(xmlpp::Element* prj_elem,
				      PortColorManager_shptr port_color_manager) {

  if(port_color_manager == NULL) throw InvalidPointerException();

  xmlpp::Element* port_colors_elem = prj_elem->add_child("port-colors");
  if(port_colors_elem == NULL) throw(std::runtime_error("Failed to create node."));

  for(PortColorManager::port_color_collection::iterator iter = port_color_manager->begin();
      iter != port_color_manager->end(); ++iter) {

    const std::string port_name = (*iter).first;
    const color_t frame_color = port_color_manager->get_frame_color(port_name);
    const color_t fill_color = port_color_manager->get_fill_color(port_name);

    xmlpp::Element* color_elem = port_colors_elem->add_child("port-color");
    if(color_elem == NULL) throw(std::runtime_error("Failed to create node."));

    color_elem->set_attribute("fill-color", to_color_string(fill_color));
    color_elem->set_attribute("frame-color", to_color_string(frame_color));
  }

}

void ProjectExporter::add_colors(xmlpp::Element* prj_elem, Project_shptr prj) {

  if(prj == NULL) throw InvalidPointerException();

  xmlpp::Element* colors_elem = prj_elem->add_child("default-colors");
  if(colors_elem == NULL) throw(std::runtime_error("Failed to create node."));

  default_colors_t default_colors = prj->get_default_colors();
  BOOST_FOREACH(default_colors_t::value_type const& p, default_colors) {
    cout << "ITERATING\n";
    xmlpp::Element* color_elem = colors_elem->add_child("color");
    if(color_elem == NULL) throw(std::runtime_error("Failed to create node."));

    std::string o;
    switch(p.first) {
    case DEFAULT_COLOR_WIRE: o = "wire"; break;
    case DEFAULT_COLOR_VIA_UP: o = "via-up"; break;
    case DEFAULT_COLOR_VIA_DOWN: o = "via-down"; break;
    case DEFAULT_COLOR_EMARKER: o = "emarker"; break;
    case DEFAULT_COLOR_GRID: o = "grid"; break;
    case DEFAULT_COLOR_ANNOTATION: o = "annotation"; break;
    case DEFAULT_COLOR_ANNOTATION_FRAME: o = "annotation-frame"; break;
    case DEFAULT_COLOR_GATE: o = "gate"; break;
    case DEFAULT_COLOR_GATE_FRAME: o = "gate-frame"; break;
    case DEFAULT_COLOR_GATE_PORT: o = "gate-port"; break;
    case DEFAULT_COLOR_TEXT: o = "text"; break;
    default:
      throw std::runtime_error("Invalid object type.");
    }

    color_elem->set_attribute("object", o);
    color_elem->set_attribute("color", to_color_string(p.second));
  }

}
