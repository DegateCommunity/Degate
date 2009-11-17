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

#include <degate.h>
#include <LogicModelHelper.h>
#include <boost/format.hpp>

using namespace degate;

Layer_shptr degate::get_first_layer(LogicModel_shptr lmodel, Layer::LAYER_TYPE layer_type) 
  throw(InvalidPointerException, CollectionLookupException, DegateLogicException) {

  if(layer_type == Layer::UNDEFINED) 
    throw DegateLogicException("Invalid layer type.");

  if(lmodel == NULL) 
    throw InvalidPointerException("Error: you passed an invalid pointer to get_first_layer()");

  for(LogicModel::layer_collection::iterator iter = lmodel->layers_begin();
      iter != lmodel->layers_end(); ++iter) {
    Layer_shptr layer = *iter;

    if(layer->is_enabled() && layer->get_layer_type() == layer_type)
      return layer;
  }
  
  boost::format fmter("Can't lookup layer of type %1% in logic model.");
  fmter % Layer::get_layer_type_as_string(layer_type);
  throw CollectionLookupException(fmter.str());
}

Layer_shptr degate::get_first_logic_layer(LogicModel_shptr lmodel) 
  throw(InvalidPointerException, CollectionLookupException) {

  if(lmodel == NULL) 
    throw InvalidPointerException("Error: you passed an invalid pointer to get_first_logic_layer()");
  try {
    return get_first_layer(lmodel, Layer::LOGIC);
  }
  catch(CollectionLookupException const& ex) {
    throw;
  }
}


Gate_shptr degate::get_gate_by_name(LogicModel_shptr lmodel, 
				    std::string const& gate_name) {
  for(LogicModel::gate_collection::iterator iter = lmodel->gates_begin();
      iter != lmodel->gates_end(); ++iter) {
    Gate_shptr gate = (*iter).second;
    if(gate->get_name() == gate_name) return gate;
  }

  return Gate_shptr();
}

void degate::apply_colors_to_gate_ports(LogicModel_shptr lmodel,
					PortColorManager_shptr pcm) {
  
  // iterate over gates
  for(LogicModel::gate_collection::iterator iter = lmodel->gates_begin();
      iter != lmodel->gates_end(); ++iter) {
    Gate_shptr gate = (*iter).second;

    // iterate over gate ports
    for(Gate::port_iterator iter = gate->ports_begin(); 
	iter != gate->ports_end(); ++iter) {

      GatePort_shptr gate_port = *iter;

      if(gate_port->has_template_port()) {
	GateTemplatePort_shptr tmpl_port = gate_port->get_template_port();
	std::string port_name = tmpl_port->get_name();
	if(pcm->has_color_definition(port_name)) {
	  color_t fill_c = pcm->get_fill_color(port_name);
	  color_t frame_c = pcm->get_frame_color(port_name);
	
	  gate_port->set_fill_color(fill_c);
	  gate_port->set_frame_color(frame_c);
	}
      }
    }
  }
}


void degate::grab_template_images(LogicModel_shptr lmodel,
				  GateTemplate_shptr gate_template,
				  BoundingBox const& bounding_box,
				  Gate::ORIENTATION orientation) {

  std::list<Layer_shptr> layers;

  Layer_shptr l;

  try {
    l = get_first_layer(lmodel, Layer::TRANSISTOR);
    if(l != NULL) layers.push_back(l);
  }
  catch(CollectionLookupException const& ex) {
    debug(TM, "Got an exception. A layer is not available. I will ignore it: %s", ex.what());
  }
    
  try {
    l = get_first_logic_layer(lmodel);
    if(l != NULL) layers.push_back(l);
  }
  catch(CollectionLookupException const& ex) {
    debug(TM, "Got an exception. A layer is not available. I will ignore it: %s", ex.what());
  }
    
  try {
    l = get_first_layer(lmodel, Layer::METAL);
    if(l != NULL) layers.push_back(l);
  }
  catch(CollectionLookupException const& ex) {
    debug(TM, "Got an exception. A layer is not available. I will ignore it: %s", ex.what());
  }

  for(std::list<Layer_shptr>::iterator iter = layers.begin();
      iter != layers.end(); ++iter) {
    
    Layer_shptr layer = *iter;
    assert(layer->get_layer_type() != Layer::UNDEFINED);
    debug(TM, "grab image from %s layer", 
	  layer->get_layer_type_as_string().c_str());

    // extract image
    
    GateTemplateImage_shptr tmpl_img = 
      grab_image<GateTemplateImage>(lmodel, layer, bounding_box);
    assert(tmpl_img != NULL);
    
    // flip
    switch(orientation) {
    case Gate::ORIENTATION_FLIPPED_UP_DOWN:
      flip_up_down<GateTemplateImage>(tmpl_img);
      break;
    case Gate::ORIENTATION_FLIPPED_LEFT_RIGHT:
      flip_left_right<GateTemplateImage>(tmpl_img);
      break;
    case Gate::ORIENTATION_FLIPPED_BOTH:
      flip_up_down<GateTemplateImage>(tmpl_img);
      flip_left_right<GateTemplateImage>(tmpl_img);
      break;
    default:
      // do nothing
      break;
    }
    
    // set as master image
    gate_template->set_image(layer->get_layer_type(), tmpl_img);

  }
}


void degate::load_background_image(Layer_shptr layer, 
				   std::string const& project_dir,
				   std::string const& image_file) throw(InvalidPointerException) {
  
  if(layer == NULL) 
    throw InvalidPointerException("Error: you passed an invalid pointer to load_background_image()");
  
  boost::format fmter("layer_%1%.dimg");
  fmter % layer->get_layer_pos();

  std::string dir(join_pathes(project_dir, fmter.str()));

  if(layer->has_background_image())
    layer->unset_image();

  debug(TM, "Create background image in %s", dir.c_str());
  BackgroundImage_shptr bg_image(new BackgroundImage(layer->get_width(),
						     layer->get_height(),
						     dir));
  
  debug(TM, "Load image %s", image_file.c_str());
  load_image<BackgroundImage>(image_file, bg_image);

  debug(TM, "Set image to layer.");
  layer->set_image(bg_image);
  debug(TM, "Done.");
}


void degate::clear_logic_model(LogicModel_shptr lmodel, Layer_shptr layer) throw(InvalidPointerException) {
  if(lmodel == NULL || layer == NULL) 
    throw InvalidPointerException("Error: you passed an invalid pointer to clear_logc_model()");

  // iterate over all objects that are placed on a specific layer and remove them

  for(LogicModel::object_collection::iterator iter = lmodel->objects_begin();
      iter != lmodel->objects_end(); ++iter) {
    PlacedLogicModelObject_shptr lmo = (*iter).second;
    if(lmo->get_layer() == layer) {
      lmodel->remove_object(lmo);
    }
  }

}

Layer_shptr degate::get_first_enabled_layer(LogicModel_shptr lmodel) 
  throw(InvalidPointerException, CollectionLookupException) {
  if(lmodel == NULL) 
    throw InvalidPointerException("Error: you passed an invalid pointer to get_first_enabled_layer()");

  for(LogicModel::layer_collection::iterator iter = lmodel->layers_begin();
      iter != lmodel->layers_end(); ++iter) {
    Layer_shptr layer = *iter;

    if(layer->is_enabled()) return layer;
  }
  throw InvalidPointerException("Error: all layers are disabled.");
}

Layer_shptr degate::get_next_enabled_layer(LogicModel_shptr lmodel) 
  throw(InvalidPointerException, CollectionLookupException, DegateRuntimeException) {
  if(lmodel == NULL) 
    throw InvalidPointerException("Error: you passed an invalid pointer to get_next_enabled_layer()");

  Layer_shptr curr_layer = lmodel->get_current_layer();
  if(curr_layer == NULL)
    throw DegateRuntimeException("Error: there is no current layer.");

  for(unsigned int l_pos = curr_layer->get_layer_pos() + 1; 
      l_pos <= curr_layer->get_layer_pos() + lmodel->get_num_layers(); l_pos++) {
    Layer_shptr layer = lmodel->get_layer(l_pos % lmodel->get_num_layers());
    if(layer->is_enabled()) return layer;
  }
  throw InvalidPointerException("Error: all layers are disabled.");
  return Layer_shptr(); // to avoid compiler warning
}

Layer_shptr degate::get_prev_enabled_layer(LogicModel_shptr lmodel) 
  throw(InvalidPointerException, CollectionLookupException, DegateRuntimeException) {
  if(lmodel == NULL) 
    throw InvalidPointerException("Error: you passed an invalid pointer to get_prev_enabled_layer()");

  Layer_shptr curr_layer = lmodel->get_current_layer();
  if(curr_layer == NULL)
    throw DegateRuntimeException("Error: there is no current layer.");

  for(unsigned int l_pos = curr_layer->get_layer_pos() + lmodel->get_num_layers() - 1; 
      l_pos > 0; l_pos--) {
    Layer_shptr layer = lmodel->get_layer(l_pos % lmodel->get_num_layers());
    if(layer->is_enabled()) return layer;
  }
  throw InvalidPointerException("Error: all layers are disabled.");
  return Layer_shptr(); // to avoid compiler warning

}
