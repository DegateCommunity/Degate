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

#include <globals.h>
#include <degate.h>
#include <LogicModelHelper.h>
#include <TangencyCheck.h>

#include <boost/format.hpp>
#include <boost/foreach.hpp>

using namespace degate;

Layer_shptr degate::get_first_layer(LogicModel_shptr lmodel, Layer::LAYER_TYPE layer_type) {

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

Layer_shptr degate::get_first_logic_layer(LogicModel_shptr lmodel) {

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



std::list<Layer_shptr> degate::get_available_standard_layers(LogicModel_shptr lmodel) {

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

  return layers;
}


void degate::grab_template_images(LogicModel_shptr lmodel,
				  GateTemplate_shptr gate_template,
				  BoundingBox const& bounding_box,
				  Gate::ORIENTATION orientation) {

  std::list<Layer_shptr> layers = get_available_standard_layers(lmodel);

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
				   std::string const& image_file) {
  
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


void degate::clear_logic_model(LogicModel_shptr lmodel, Layer_shptr layer) {
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

Layer_shptr degate::get_first_enabled_layer(LogicModel_shptr lmodel) {
  if(lmodel == NULL) 
    throw InvalidPointerException("Error: you passed an invalid pointer to get_first_enabled_layer()");

  for(LogicModel::layer_collection::iterator iter = lmodel->layers_begin();
      iter != lmodel->layers_end(); ++iter) {
    Layer_shptr layer = *iter;

    if(layer->is_enabled()) return layer;
  }
  throw InvalidPointerException("Error: all layers are disabled.");
}

Layer_shptr degate::get_next_enabled_layer(LogicModel_shptr lmodel) {
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


Layer_shptr degate::get_next_enabled_layer(LogicModel_shptr lmodel, Layer_shptr layer) {

  if(lmodel == NULL || layer == NULL) 
    throw InvalidPointerException("Error: you passed an invalid pointer to get_next_enabled_layer()");

  for(unsigned int l_pos = layer->get_layer_pos() + 1; 
      l_pos < lmodel->get_num_layers(); l_pos++) {
    Layer_shptr l = lmodel->get_layer(l_pos);
    if(l->is_enabled()) return l;
  }
  
  return Layer_shptr();
}

Layer_shptr degate::get_prev_enabled_layer(LogicModel_shptr lmodel) {

  if(lmodel == NULL) 
    throw InvalidPointerException("Error: you passed an invalid pointer to get_prev_enabled_layer()");

  Layer_shptr curr_layer = lmodel->get_current_layer();
  if(curr_layer == NULL)
    throw DegateRuntimeException("Error: there is no current layer.");

  if(lmodel->get_num_layers() == 1) return curr_layer;

  for(unsigned int l_pos = curr_layer->get_layer_pos() + lmodel->get_num_layers() - 1; 
      l_pos > 0; l_pos--) {
    Layer_shptr layer = lmodel->get_layer(l_pos % lmodel->get_num_layers());
    if(layer->is_enabled()) return layer;
  }
  throw InvalidPointerException("Error: all layers are disabled.");
  return Layer_shptr(); // to avoid compiler warning

}

Layer_shptr degate::get_prev_enabled_layer(LogicModel_shptr lmodel, Layer_shptr layer) {

  if(lmodel == NULL || layer == NULL) 
    throw InvalidPointerException("Error: you passed an invalid pointer to get_prev_enabled_layer()");

  for(unsigned int l_pos = layer->get_layer_pos(); l_pos > 0; l_pos--) {
    
    Layer_shptr l = lmodel->get_layer(l_pos - 1);
    if(l->is_enabled()) return l;
  }
  return Layer_shptr();
}

Layer_shptr degate::get_current_layer(Project_shptr project) {
  if(project == NULL) 
    throw InvalidPointerException("Invalid parameter for get_curent_layer()");

  LogicModel_shptr lmodel = project->get_logic_model();
  assert(lmodel != NULL);
  return lmodel->get_current_layer();
}

bool degate::is_logic_class(Gate_shptr gate, std::string const& logic_class) {

  if(gate == NULL) 
    throw InvalidPointerException("Invalid parameter for is_logic_class()");

  if(gate->has_template()) {
    GateTemplate_shptr gate_tmpl = gate->get_gate_template();
    std::string const& lclass = gate_tmpl->get_logic_class();
    if(logic_class == lclass) return true;
    if(logic_class.size() < lclass.size()) {
      if(lclass.substr(0, logic_class.size()) == logic_class) return true;
    }
  }

  return false;
}


GateTemplatePort::PORT_TYPE degate::get_port_type(GatePort_shptr gate_port) {

  if(gate_port == NULL) 
    throw InvalidPointerException("Invalid parameter for get_port_type()");

  if(gate_port->has_template_port()) {
    GateTemplatePort_shptr tmpl_port = gate_port->get_template_port();
    return tmpl_port->get_port_type();
  }

  return GateTemplatePort::PORT_TYPE_UNDEFINED;
}


std::string degate::get_template_port_name(GatePort_shptr gate_port) {

  if(gate_port == NULL) 
    throw InvalidPointerException("Invalid parameter for get_template_port_name()");

  if(gate_port->has_template_port()) {
    GateTemplatePort_shptr tmpl_port = gate_port->get_template_port();
    return tmpl_port->get_name();
  }

  return "";
}

void degate::apply_port_color_settings(LogicModel_shptr lmodel, PortColorManager_shptr pcm) {
  if(lmodel == NULL || pcm == NULL)
    throw InvalidPointerException("Invalid parameter for apply_port_color_settings()");

  // iterate over gates

  for(LogicModel::gate_collection::iterator gate_iter = lmodel->gates_begin();
      gate_iter != lmodel->gates_end(); ++gate_iter) {
    Gate_shptr gate = gate_iter->second;

    // iterate over ports

    for(Gate::port_iterator iter = gate->ports_begin(); iter != gate->ports_end(); ++iter) {
      GatePort_shptr port = *iter;

      if(port->has_template_port()) {
	GateTemplatePort_shptr tmpl_port = port->get_template_port();

	std::string port_name = tmpl_port->get_name();
	if(pcm->has_color_definition(port_name)) {
	  tmpl_port->set_frame_color(pcm->get_frame_color(port_name));
	  tmpl_port->set_fill_color(pcm->get_fill_color(port_name));
	}
	
      }
    }

  }

}


void degate::merge_gate_images(LogicModel_shptr lmodel,
			       Layer_shptr layer,
			       GateTemplate_shptr tmpl, 
			       std::list<Gate_shptr> const& gates) {

  if(gates.empty()) return;
  
  std::list<GateTemplateImage_shptr> images;
  
  BOOST_FOREACH(const Gate_shptr g, gates) {
    
    GateTemplateImage_shptr tmpl_img = 
      grab_image<GateTemplateImage>(lmodel, layer, g->get_bounding_box());
    assert(tmpl_img != NULL);
    
    // flip
    switch(g->get_orientation()) {
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
    
    images.push_back(tmpl_img);
  }
  
  GateTemplateImage_shptr merged_img = merge_images<GateTemplateImage>(images);
  
  tmpl->set_image(layer->get_layer_type(), merged_img);
}

void degate::merge_gate_images(LogicModel_shptr lmodel,
			       ObjectSet gates) {

  /*
   * Classify gates by their standard cell object ID.
   */
  typedef std::map<object_id_t, std::list<Gate_shptr> > gate_sets_type;
  gate_sets_type gate_sets;
  
  BOOST_FOREACH(PlacedLogicModelObject_shptr plo, gates) {
    if(Gate_shptr gate = std::tr1::dynamic_pointer_cast<Gate>(plo)) {
      GateTemplate_shptr tmpl = gate->get_gate_template();
      if(tmpl) // ignore gates, that have no standard cell
	gate_sets[tmpl->get_object_id()].push_back(gate);
    }
  }
  
  /*
   * Iterate over layers.
   */
  
  BOOST_FOREACH(Layer_shptr layer, get_available_standard_layers(lmodel)) {
    
    /*
     * Iterate over standard cell classes.
     */
    for(gate_sets_type::iterator iter = gate_sets.begin(); iter != gate_sets.end(); ++iter) {
      
      Gate_shptr g = iter->second.front();
      assert(g != NULL);
      
      merge_gate_images(lmodel, layer, g->get_gate_template(), iter->second);
    }
  }
  
}


void degate::connect_objects(LogicModel_shptr lmodel, 
			     ConnectedLogicModelObject_shptr o1,
			     ConnectedLogicModelObject_shptr o2) {

  std::list<ConnectedLogicModelObject_shptr> v;
  v.push_back(o1);
  v.push_back(o2);
  connect_objects(lmodel, v.begin(), v.end());
}


void degate::autoconnect_objects(LogicModel_shptr lmodel, Layer_shptr layer,
				 BoundingBox const& search_bbox) {

  if(lmodel == NULL || layer == NULL)
    throw InvalidPointerException("You passed an invalid shared pointer.");

  // iterate over connectable objects
  for(Layer::qt_region_iterator iter = layer->region_begin(search_bbox);
      iter != layer->region_end(); ++iter) {
   
    ConnectedLogicModelObject_shptr clmo1;

    if((clmo1 = std::tr1::dynamic_pointer_cast<ConnectedLogicModelObject>(*iter)) != NULL) {
      
      BoundingBox const& bb = clmo1->get_bounding_box();
      
      /* Iterate over connectable objects in the region identified
	 by bounding box bb.
      */
      for(Layer::qt_region_iterator siter = layer->region_begin(bb);
	  siter != layer->region_end(); ++siter) {

	ConnectedLogicModelObject_shptr clmo2;
	if((clmo2 = 
	    std::tr1::dynamic_pointer_cast<ConnectedLogicModelObject>(*siter)) != NULL) {

	  if((clmo1->get_net() == NULL || 
	      clmo2->get_net() == NULL ||
	      clmo1->get_net() != clmo2->get_net()) && // excludes identical objects, too
	     check_object_tangency(std::tr1::dynamic_pointer_cast<PlacedLogicModelObject>(clmo1), 
				   std::tr1::dynamic_pointer_cast<PlacedLogicModelObject>(clmo2)))
	    
	    connect_objects(lmodel, clmo1, clmo2); 
	  	    
	  	      	  
	}
      }      
    }
  }
}

void autoconnect_interlayer_objects_via_via(LogicModel_shptr lmodel, 
					    Layer_shptr adjacent_layer,
					    BoundingBox const& search_bbox,
					    Via_shptr v1,
					    Via::DIRECTION v1_dir_criteria,
					    Via::DIRECTION v2_dir_criteria) {

  Via_shptr v2;

  for(Layer::qt_region_iterator siter = adjacent_layer->region_begin(search_bbox);
      siter != adjacent_layer->region_end(); ++siter) {
    
    if((v2 = std::tr1::dynamic_pointer_cast<Via>(*siter)) != NULL) {
      
      if((v1->get_net() == NULL || v2->get_net() == NULL ||
	  v1->get_net() != v2->get_net()) &&
	 v1->get_direction() == v1_dir_criteria &&
	 v2->get_direction() == v2_dir_criteria &&
	 check_object_tangency(std::tr1::dynamic_pointer_cast<Circle>(v1), 
			       std::tr1::dynamic_pointer_cast<Circle>(v2)))
	connect_objects(lmodel, 
			std::tr1::dynamic_pointer_cast<ConnectedLogicModelObject>(v1), 
			std::tr1::dynamic_pointer_cast<ConnectedLogicModelObject>(v2));
    }
  }      

}

void autoconnect_interlayer_objects_via_gport(LogicModel_shptr lmodel, 
					      Layer_shptr adjacent_layer,
					      BoundingBox const& search_bbox,
					      Via_shptr v1,
					      Via::DIRECTION v1_dir_criteria) {

  GatePort_shptr v2;

  for(Layer::qt_region_iterator siter = adjacent_layer->region_begin(search_bbox);
      siter != adjacent_layer->region_end(); ++siter) {
    
    if((v2 = std::tr1::dynamic_pointer_cast<GatePort>(*siter)) != NULL) {
      
      if((v1->get_net() == NULL || v2->get_net() == NULL ||
	  v1->get_net() != v2->get_net()) &&
	 v1->get_direction() == v1_dir_criteria &&
	 check_object_tangency(std::tr1::dynamic_pointer_cast<Circle>(v1), 
			       std::tr1::dynamic_pointer_cast<Circle>(v2)))
	connect_objects(lmodel,
			std::tr1::dynamic_pointer_cast<ConnectedLogicModelObject>(v1), 
			std::tr1::dynamic_pointer_cast<ConnectedLogicModelObject>(v2));
    }
  }      

}

void degate::autoconnect_interlayer_objects(LogicModel_shptr lmodel, 
					    Layer_shptr layer,
					    BoundingBox const& search_bbox) {
  if(lmodel == NULL || layer == NULL)
    throw InvalidPointerException("You passed an invalid shared pointer.");

  Layer_shptr 
    layer_above = get_next_enabled_layer(lmodel, layer),
    layer_below = get_prev_enabled_layer(lmodel, layer);

  Via_shptr v1;

  // iterate over objects
  for(Layer::qt_region_iterator iter = layer->region_begin(search_bbox);
      iter != layer->region_end(); ++iter) {
  
    if((v1 = std::tr1::dynamic_pointer_cast<Via>(*iter)) != NULL) {
      
      BoundingBox const& bb = v1->get_bounding_box();
      
      /* Iterate over vias one layer above and one layer below
	 in the region identified by bounding box bb. */

      if(layer_above != NULL)
	autoconnect_interlayer_objects_via_via(lmodel, layer_above, bb, v1, 
					       Via::DIRECTION_UP, Via::DIRECTION_DOWN);

      if(layer_below != NULL) {
	autoconnect_interlayer_objects_via_via(lmodel, layer_below, bb, v1, 
					       Via::DIRECTION_DOWN, Via::DIRECTION_UP);
	autoconnect_interlayer_objects_via_gport(lmodel, layer_below, bb, v1, 
						 Via::DIRECTION_DOWN);
      }

    }
  }

}
