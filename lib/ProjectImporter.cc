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

#include "ProjectImporter.h"
#include "Layer.h"
#include "FileSystem.h"
#include "degate_exceptions.h"
#include "GateLibraryImporter.h"
#include "LogicModelImporter.h"
#include "PortColorManager.h"
#include <Image.h>
#include <LogicModelHelper.h>
#include <ImageHelper.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <list>

#include <boost/format.hpp>

using namespace std;
using namespace degate;

std::string ProjectImporter::get_project_filename(std::string const& dir) const {

  if(is_directory(dir))
    return join_pathes(dir, "project.xml");
  else
    return dir;
}

Project_shptr ProjectImporter::import_all(std::string const& directory) {
  Project_shptr prj = import(directory);

  if(prj != NULL) {

    try {
      GateLibraryImporter gl_importer;
      
      GateLibrary_shptr gate_lib;

      std::string gate_lib_file(get_basedir(directory) + "/gate_library.xml");

      if(file_exists(gate_lib_file))
	gate_lib = gl_importer.import(gate_lib_file);
      else gate_lib = GateLibrary_shptr(new GateLibrary());

      LogicModelImporter lm_importer(prj->get_width(), prj->get_height(), gate_lib);

      lm_importer.import_into(prj->get_logic_model(),
			      get_basedir(directory) + "/lmodel.xml");

      LogicModel_shptr lmodel = prj->get_logic_model();
	    

      /*
	For degate projects that were exported with degate 0.0.6 the gate templates
	were expressed in terms of an image region. This is bad. Here is a part of the fix:
	We have loaded the project with the background images and we have the gate
	library. We iterate over the gate library, extract the template image from the
	background image and put it into the gate library. We do it for the first
	transistor, the first logic and the first metal layer.
      */

      debug(TM, "Check if we have template images.");
      for(GateLibrary::template_iterator iter = gate_lib->begin();
	  iter != gate_lib->end(); ++iter) {
	
	GateTemplate_shptr tmpl = (*iter).second;
	assert(tmpl != NULL);

	BoundingBox const& bbox = tmpl->get_bounding_box();
	if(bbox.get_min_x() != 0 && bbox.get_min_y() != 0 &&
	   bbox.get_max_x() != 0 && bbox.get_max_y() != 0) { // a heuristic
	  debug(TM, "Grab template images from the background for template %s.", tmpl->get_name().c_str());
	  grab_template_images(lmodel, tmpl, bbox);
	}

      }
      debug(TM, "Project loaded.");
      //prj->print_all(cout);

    }
    catch(const std::exception & ex) {
      throw;
    }
  }

  return prj;
}

Project_shptr ProjectImporter::import(std::string const& directory) throw( InvalidPathException, InvalidXMLException ) {

  string filename = get_project_filename(directory);
  if(RET_IS_NOT_OK(check_file(filename))) {
    debug(TM, "Problem: file %s not found.", filename.c_str());
    throw InvalidPathException("The ProjectImporter cannot load the project file. File does not exists.");
  }
	
  try {

    xmlpp::DomParser parser;
    parser.set_substitute_entities(); // We just want the text to be resolved/unescaped automatically.
		
    parser.parse_file(filename);
    assert(parser == true);
		
    const xmlpp::Document * doc = parser.get_document();
    assert(doc != NULL);
		
    const xmlpp::Element * root_elem = doc->get_root_node(); // deleted by DomParser
    assert(root_elem != NULL);
    
    // parse width and height
    int w = parse_number<length_t>(root_elem, "width");
    int h = parse_number<length_t>(root_elem, "height");

    Project_shptr prj(new Project(w, h, get_basedir(filename)));
    assert(prj->get_project_directory().length() != 0);

    parse_project_element(prj, root_elem);

    return prj;
  }
  catch(const std::exception& ex) {
    std::cout << "Exception caught: " << ex.what() << std::endl;
    throw InvalidXMLException(ex.what());
  }
}


void ProjectImporter::parse_layers_element(const xmlpp::Element * const layers_elem, Project_shptr prj) throw(std::runtime_error) {
  debug(TM, "parsing layers");

  const xmlpp::Node::NodeList layer_list = layers_elem->get_children("layer");
  for(xmlpp::Node::NodeList::const_iterator iter = layer_list.begin();
      iter != layer_list.end(); 
      ++iter) {

    if(const xmlpp::Element* layer_elem = dynamic_cast<const xmlpp::Element*>(*iter)) {

      const std::string image_filename(layer_elem->get_attribute_value("image-filename"));
      const std::string layer_type_str(layer_elem->get_attribute_value("type"));
      unsigned int position = parse_number<unsigned int>(layer_elem, "position");
      
      Layer::LAYER_TYPE layer_type = Layer::get_layer_type_from_string(layer_type_str);

      Layer_shptr new_layer(new Layer(prj->get_bounding_box(), layer_type));
      LogicModel_shptr lmodel = prj->get_logic_model();

      debug(TM, "Parsed a layer entry for type %s. This is a %s layer. Background image is %s", 
	    layer_type_str.c_str(),
	    Layer::get_layer_type_as_string(layer_type).c_str(),
	    image_filename.c_str());

      lmodel->add_layer(position, new_layer);

      load_background_image(new_layer, image_filename, prj);

    }
  }
}

void ProjectImporter::load_background_image(Layer_shptr layer, 
					    std::string const& image_filename, 
					    Project_shptr prj) 
  throw(DegateRuntimeException) {

  debug(TM, "try to load image [%s]", image_filename.c_str());
  if(!image_filename.empty()) {

    assert(prj->get_project_directory().length() != 0);

    std::string image_path_to_load = join_pathes(prj->get_project_directory(), image_filename);

    debug(TM, "try to load image [%s]", image_path_to_load.c_str());

    if(is_directory(image_path_to_load)) { // new background image format

      debug(TM, "project importer loads an tile based image from [%s]", image_path_to_load.c_str());

      BackgroundImage_shptr bg_image = 	
	load_degate_image<BackgroundImage>(prj->get_width(),
					   prj->get_height(),
					   image_path_to_load);
      
      if(bg_image == NULL) 
	throw DegateRuntimeException("Failed to load the background image");

      debug(TM, "Loading done.");
      layer->set_image(bg_image);
    }
    else if(is_file(image_path_to_load)) { // old image background format

      // determine where we can store the new backgound image
      boost::format fmter("layer_%1%.dimg");
      fmter % layer->get_layer_pos();
      std::string new_dir(join_pathes(prj->get_project_directory(), fmter.str()));

      debug(TM, "project importer loads an old single file based image from [%s]", 
	    image_path_to_load.c_str());

      if(!is_directory(new_dir)) { // we have to check this, before we call the constructor

	// create new background image
	BackgroundImage_shptr new_bg_image(new BackgroundImage(prj->get_width(),
							     prj->get_height(),
							     new_dir));

	// load old single file image
	PersistentImage_RGBA_shptr old_bg_image =
	  load_degate_image<PersistentImage_RGBA>(prj->get_width(),
						  prj->get_height(),
						  image_path_to_load);
	
	if(new_bg_image == NULL) 
	  throw DegateRuntimeException("Failed to load the background image");
	
	// convert image into new format
	
	debug(TM, "Copy the image into a new format. The data is stored in directory %s",
	      new_dir.c_str());

	copy_image<BackgroundImage, PersistentImage_RGBA>(new_bg_image, old_bg_image);
	
	layer->set_image(new_bg_image);

	//save_image<PersistentImage_RGBA>("/tmp/xxx.tif", old_bg_image);
	//save_image<BackgroundImage>("/tmp/yyy.tif", new_bg_image);
      }
      else {
	debug(TM, 
	      "There is already a directory named %s. It should be loaded as an image now.", 
	      new_dir.c_str());

	BackgroundImage_shptr new_bg_image(new BackgroundImage(prj->get_width(),
							       prj->get_height(),
							       new_dir));

	layer->set_image(new_bg_image);
      }

      
    }
    
  }
  else {
    debug(TM, "project in %s has no layer image defined for a layer.", 
	  prj->get_project_directory().c_str());
  }
}


void ProjectImporter::parse_port_colors_element(const xmlpp::Element * const port_colors_elem, Project_shptr prj) 
  throw(std::runtime_error) {

  const xmlpp::Node::NodeList color_list = port_colors_elem->get_children("port-color");

  PortColorManager_shptr port_color_manager = prj->get_port_color_manager();

  for(xmlpp::Node::NodeList::const_iterator iter = color_list.begin();
      iter != color_list.end(); 
      ++iter) {

    if(const xmlpp::Element* color_elem = dynamic_cast<const xmlpp::Element*>(*iter)) {

      const std::string port_name(color_elem->get_attribute_value("port-name"));
      const std::string fill_color_str(color_elem->get_attribute_value("fill-color"));
      const std::string frame_color_str(color_elem->get_attribute_value("frame-color"));

      port_color_manager->set_color(port_name,
				    parse_color_string(frame_color_str),
				    parse_color_string(fill_color_str) );
      

    }
  }
  
}

void ProjectImporter::parse_grids_element(const xmlpp::Element * const grids_elem, Project_shptr prj) {

  xmlpp::Node::NodeList::const_iterator iter;

  const xmlpp::Node::NodeList regular_grid_list = grids_elem->get_children("regular-grid");
  const xmlpp::Node::NodeList irregular_grid_list = grids_elem->get_children("irregular-grid");

  
  for(iter = regular_grid_list.begin(); iter != regular_grid_list.end(); ++iter) {
    if(const xmlpp::Element* regular_grid_elem = dynamic_cast<const xmlpp::Element*>(*iter)) {

      const Glib::ustring orientation(regular_grid_elem->get_attribute_value("orientation"));

      RegularGrid_shptr reg_grid = (orientation == "horizontal") ?
	prj->get_regular_horizontal_grid() : prj->get_regular_vertical_grid();

      reg_grid->set_distance(parse_number<unsigned int>(regular_grid_elem, "distance", 0));
      reg_grid->set_enabled(parse_bool(regular_grid_elem->get_attribute_value("enabled")));
    }
  }

  for(iter = irregular_grid_list.begin(); iter != irregular_grid_list.end(); ++iter) {
    if(const xmlpp::Element* irregular_grid_elem = dynamic_cast<const xmlpp::Element*>(*iter)) {

      const Glib::ustring orientation(irregular_grid_elem->get_attribute_value("orientation"));

      IrregularGrid_shptr irreg_grid = (orientation == "horizontal") ? 
	prj->get_irregular_horizontal_grid() : prj->get_irregular_vertical_grid();

      irreg_grid->set_enabled(parse_bool(irregular_grid_elem->get_attribute_value("enabled")));

      const xmlpp::Node::NodeList offsets_entry_list = irregular_grid_elem->get_children("offsets");
      const xmlpp::Node::NodeList::const_iterator offsets_iter = offsets_entry_list.begin();
      if(offsets_iter != offsets_entry_list.end()) {
	const xmlpp::Element* offsets_elem = dynamic_cast<const xmlpp::Element*>(*offsets_iter);
	if(offsets_elem != NULL) {

	  const xmlpp::Node::NodeList offset_entry_list = offsets_elem->get_children("offset-entry");
	  for(xmlpp::Node::NodeList::const_iterator offs_iter = offset_entry_list.begin(); 
	      offs_iter != offset_entry_list.end(); 
	      ++offs_iter) {
	    if(const xmlpp::Element* offset_entry_elem = dynamic_cast<const xmlpp::Element*>(*offs_iter)) {
	      irreg_grid->add_offset(parse_number<int>(offset_entry_elem, "offset"));
	    }
	  }
	}
      }
    }
  }
}



void ProjectImporter::parse_project_element(Project_shptr parent_prj,
					    const xmlpp::Element * const project_elem) {

  int w = parent_prj->get_width();
  int h = parent_prj->get_height();
					    
  // Use geometry information to set up regular grid ranges.
  // The RegularGrid implementation might be changed in order to avoid this setup.
  RegularGrid_shptr reg_vert_grid = parent_prj->get_regular_vertical_grid();
  RegularGrid_shptr reg_hor_grid = parent_prj->get_regular_horizontal_grid();
  reg_vert_grid->set_range(0, w);
  reg_hor_grid->set_range(0, h);

  parent_prj->set_degate_version(project_elem->get_attribute_value("degate-version"));
  parent_prj->set_name(project_elem->get_attribute_value("name"));
  parent_prj->set_description(project_elem->get_attribute_value("description"));

  parent_prj->set_lambda(parse_number<length_t>(project_elem, "lambda"));
  parent_prj->set_default_pin_diameter(parse_number<diameter_t>(project_elem, "pin-diameter"));
  parent_prj->set_default_wire_diameter(parse_number<diameter_t>(project_elem, "wire-diameter"));


  const xmlpp::Element * e = get_dom_twig(project_elem, "grids");
  if(e != NULL) parse_grids_element(e, parent_prj);

  e = get_dom_twig(project_elem, "layers");
  if(e != NULL) parse_layers_element(e, parent_prj);

  e = get_dom_twig(project_elem, "port-colors");
  if(e != NULL) parse_port_colors_element(e, parent_prj);



  /*
  xmlpp::Node::NodeList sproject_node_list = project_node->get_children("subprojects");

  // there are subprojects
  if(!sproject_node_list.empty()) {

    xmlpp::Node::Node * sproject_node = sproject_node_list.front();
    assert(sproject_node != NULL);


    xmlpp::Node::NodeList project_node_list = sproject_node->get_children("subproject");

    for(xmlpp::Node::NodeList::iterator iter = project_node_list.begin(); iter != project_node_list.end(); ++iter) {

    if(const xmlpp::Element* child_project_node = dynamic_cast<const xmlpp::Element*>(*iter)) {
	debug(TM, "parse subnode:");
	Project_shptr sub_prj = parse_project_node(child_project_node);
	assert(sub_prj != NULL);
	parent_prj->add_sub_project(sub_prj);
      }
    }
  }
  */
  
}
