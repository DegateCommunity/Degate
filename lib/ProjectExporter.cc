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

#include "globals.h"
#include "Layer.h"
#include "ProjectExporter.h"
#include "FileSystem.h"
#include "ObjectIDRewriter.h"
#include "LogicModelExporter.h"
#include "GateLibraryExporter.h"

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

void ProjectExporter::export_all(std::string const& project_directory, Project_shptr prj, 
				 bool enable_oid_rewrite) 
  throw( InvalidPathException, InvalidPointerException, std::runtime_error ) {

  if(!is_directory(project_directory)) {
    throw InvalidPathException("The path where the project should be exportet to is not a directory.");
  }
  else {
    ObjectIDRewriter_shptr oid_rewriter(new ObjectIDRewriter(enable_oid_rewrite));

    export_data(project_directory + string("/project.xml"), prj);
    
    LogicModel_shptr lmodel = prj->get_logic_model();

    if(lmodel != NULL) {
      LogicModelExporter lm_exporter(oid_rewriter);
      string lm_filename(project_directory + string("/lmodel.xml"));
      lm_exporter.export_data(lm_filename, lmodel);


      GateLibrary_shptr glib = lmodel->get_gate_library();
      if(glib != NULL) {
      
	GateLibraryExporter gl_exporter(oid_rewriter);
	gl_exporter.export_data(project_directory + string("/gate_library.xml"), glib);
      }
    }
  }
}

void ProjectExporter::export_data(std::string const& filename, Project_shptr prj) 
  throw( InvalidPathException, InvalidPointerException, std::runtime_error ) {

  if(prj == NULL) throw InvalidPointerException("Project pointer is NULL.");

  try {

    xmlpp::Document doc;

    xmlpp::Element * root_elem = doc.create_root_node("project");
    assert(root_elem != NULL);
    set_project_node_attributes(root_elem, prj);

    add_layers(root_elem, prj->get_logic_model(), prj->get_project_directory());
    add_grids(root_elem, prj);
    add_port_colors(root_elem, prj->get_port_color_manager());

    xmlpp::Element* subprojects_elem = root_elem->add_child("subprojects");
    if(subprojects_elem == NULL) throw(std::runtime_error("Failed to create node."));
    
    doc.write_to_file_formatted(filename, "ISO-8859-1");

  }
  catch(const std::exception& ex) {
    std::cout << "Exception caught: " << ex.what() << std::endl;
    throw;
  }

}

void ProjectExporter::add_grids(xmlpp::Element* prj_elem, 
				Project_shptr prj) throw(std::runtime_error ) {

  xmlpp::Element* grids_elem = prj_elem->add_child("grids");
  if(grids_elem == NULL) throw(std::runtime_error("Failed to create node."));

  
  add_regular_grid(grids_elem, prj->get_regular_horizontal_grid(), "horizontal");
  add_regular_grid(grids_elem, prj->get_regular_vertical_grid(), "vertical");

  add_irregular_grid(grids_elem, prj->get_irregular_horizontal_grid(), "horizontal");
  add_irregular_grid(grids_elem, prj->get_irregular_vertical_grid(), "vertical");
}

void ProjectExporter::add_regular_grid(xmlpp::Element* grids_elem, 
				       RegularGrid const & grid, 
				       std::string const & grid_orientation) 
  throw(std::runtime_error ) {

  xmlpp::Element* grid_elem = grids_elem->add_child("regular-grid");
  if(grid_elem == NULL) throw(std::runtime_error("Failed to create node."));

  grid_elem->set_attribute("enabled", grid.is_enabled() ? "true" : "false");
  grid_elem->set_attribute("distance", number_to_string<double>(grid.get_distance()));
  grid_elem->set_attribute("offset", number_to_string<int>(grid.get_min()));
  grid_elem->set_attribute("orientation", grid_orientation);

  
}

void ProjectExporter::add_irregular_grid(xmlpp::Element* grids_elem, 
					 IrregularGrid const & grid, 
					 std::string const & grid_orientation) 
  throw(std::runtime_error ) {

  xmlpp::Element* grid_elem = grids_elem->add_child("irregular-grid");
  if(grid_elem == NULL) throw(std::runtime_error("Failed to create node."));

  grid_elem->set_attribute("enabled", grid.is_enabled() ? "true" : "false");
  grid_elem->set_attribute("orientation", grid_orientation);  

  xmlpp::Element* offsets_elem = grid_elem->add_child("offsets");
  if(offsets_elem == NULL) throw(std::runtime_error("Failed to create node."));

  for(IrregularGrid::grid_iter iter = grid.begin();
      iter != grid.end(); ++iter) {

    xmlpp::Element* offset_elem = offsets_elem->add_child("offset-entry");
    if(offset_elem == NULL) throw(std::runtime_error("Failed to create node."));

    offset_elem->set_attribute("offset", number_to_string<int>(*iter));
  }

}


void ProjectExporter::set_project_node_attributes(xmlpp::Element* prj_elem, 
						  Project_shptr prj) throw(std::runtime_error ) {

  prj_elem->set_attribute("degate-version", prj->get_degate_version());
  prj_elem->set_attribute("name", prj->get_name());
  prj_elem->set_attribute("description", prj->get_description());
  prj_elem->set_attribute("width", number_to_string<int>(prj->get_width()));
  prj_elem->set_attribute("height", number_to_string<int>(prj->get_height()));

  prj_elem->set_attribute("lambda", number_to_string<length_t>(prj->get_lambda()));
  prj_elem->set_attribute("pin-diameter", number_to_string<length_t>(prj->get_default_pin_diameter()));
  prj_elem->set_attribute("wire-diameter", number_to_string<length_t>(prj->get_default_wire_diameter()));
  
}


void ProjectExporter::add_layers(xmlpp::Element* prj_elem, 
				 LogicModel_shptr lmodel,
				 std::string const& project_dir) 
  throw(InvalidPointerException, std::runtime_error ) {

  if(lmodel == NULL) throw InvalidPointerException();

  xmlpp::Element* layers_elem = prj_elem->add_child("layers");
  if(layers_elem == NULL) throw(std::runtime_error("Failed to create node."));

  for(LogicModel::layer_collection::iterator layer_iter = lmodel->layers_begin();
      layer_iter != lmodel->layers_end(); ++layer_iter) {

    xmlpp::Element* layer_elem = layers_elem->add_child("layer");
    if(layer_elem == NULL) throw(std::runtime_error("Failed to create node."));

    Layer_shptr layer = *layer_iter;

    layer_elem->set_attribute("position", number_to_string<layer_position_t>(layer->get_layer_pos()));
    layer_elem->set_attribute("type", layer->get_layer_type_as_string());
    
    if(layer->has_background_image()) 
      layer_elem->set_attribute("image-filename", 
				get_relative_path(layer->get_image_filename(), project_dir));
    
  }

}


void ProjectExporter::add_port_colors(xmlpp::Element* prj_elem, 
				      PortColorManager_shptr port_color_manager) 
  throw(InvalidPointerException, std::runtime_error ) {

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
