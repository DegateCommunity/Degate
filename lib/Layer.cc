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
#include <Layer.h>
#include <boost/format.hpp>

using namespace degate;

void Layer::add_object(std::tr1::shared_ptr<PlacedLogicModelObject> o) 
  throw(DegateRuntimeException, DegateLogicException) { 

  if(o->get_bounding_box() == BoundingBox(0, 0, 0, 0)) {
    boost::format fmter("Error in add_object(): Object %1% with ID %2% has an "
			"undefined bounding box. Can't insert it into the quadtree");
    fmter % o->get_object_type_name() % o->get_object_id();
    throw DegateLogicException(fmter.str());
  }

  if(RET_IS_NOT_OK(quadtree.insert(o))) {
    debug(TM, "Failed to insert object into quadtree.");
    throw DegateRuntimeException("Failed to insert object into quadtree.");
  }
  objects[o->get_object_id()] = o;
}

void Layer::remove_object(std::tr1::shared_ptr<PlacedLogicModelObject> o) 
  throw(DegateRuntimeException) {
  if(RET_IS_NOT_OK(quadtree.remove(o))) {
    debug(TM, "Failed to remove object from quadtree.");
    throw std::runtime_error("Failed to remove object from quadtree.");
  }

  objects.erase(o->get_object_id());
}

Layer::Layer(BoundingBox const & bbox, Layer::LAYER_TYPE _layer_type) : 
  quadtree(bbox, 100), 
  layer_type(_layer_type), 
  layer_pos(0),
  enabled(true)  {
}

Layer::Layer(BoundingBox const & bbox, Layer::LAYER_TYPE _layer_type, 
	     BackgroundImage_shptr img) : 
  quadtree(bbox, 100), 
  layer_type(_layer_type), 
  layer_pos(0),
  enabled(true) {

  set_image(img);
}


Layer::~Layer() {
}

unsigned int Layer::get_width() const {
  return quadtree.get_width();
}

unsigned int Layer::get_height() const {
  return quadtree.get_height();
}

const std::string Layer::get_layer_type_as_string() const {
  return get_layer_type_as_string(layer_type);
}

const std::string Layer::get_layer_type_as_string(LAYER_TYPE _layer_type) {
  switch(_layer_type) {
  case METAL:
    return std::string("metal");
  case LOGIC:
    return std::string("logic");
  case TRANSISTOR:
    return std::string("transistor");
  case UNDEFINED:
  default:
    return std::string("undefined");
  }
}

Layer::LAYER_TYPE Layer::get_layer_type_from_string(std::string const& layer_type_str) 
  throw(DegateRuntimeException) {

  if(layer_type_str == "metal") return Layer::METAL;
  else if(layer_type_str == "logic") return Layer::LOGIC;
  else if(layer_type_str == "transistor") return Layer::TRANSISTOR;
  else if(layer_type_str == "undefined") return Layer::UNDEFINED;
  else throw DegateRuntimeException("Can't parse layer type.");
}


Layer::LAYER_TYPE Layer::get_layer_type() const { 
  return layer_type; 
}

void Layer::set_layer_type(LAYER_TYPE _layer_type) { 
  layer_type = _layer_type; 
}


bool Layer::is_empty() const { 
  return quadtree.is_empty(); 
}

layer_position_t Layer::get_layer_pos() const {
  return layer_pos;
}

Layer::object_iterator Layer::objects_begin() {
  return quadtree.region_iter_begin();
}

Layer::object_iterator Layer::objects_end() {
  return quadtree.region_iter_end();
}

Layer::qt_region_iterator Layer::region_begin(int min_x, int max_x, int min_y, int max_y) {
  return quadtree.region_iter_begin(min_x, max_x, min_y, max_y);
}

Layer::qt_region_iterator Layer::region_end() {
  return quadtree.region_iter_end();
}

void Layer::set_image(BackgroundImage_shptr img) {

  scaling_manager = 
    std::tr1::shared_ptr<ScalingManager<BackgroundImage> >
    (new ScalingManager<BackgroundImage>(img, img->get_directory()));

  scaling_manager->create_scalings();
}

BackgroundImage_shptr Layer::get_image() throw(DegateLogicException) {
  if(scaling_manager != NULL) {
    ScalingManager<BackgroundImage>::image_map_element p = scaling_manager->get_image(1);
    return p.second;
  }
  else throw DegateLogicException("You have to set the background image first.");
}

std::string Layer::get_image_filename() const throw(DegateLogicException) { 
  
  if(scaling_manager == NULL) 
    throw DegateLogicException("There is no scaling manager.");
  else {
    const ScalingManager<BackgroundImage>::image_map_element p = 
      scaling_manager->get_image(1);

    if(p.second != NULL)
      return p.second->get_directory();
    else
      throw DegateLogicException("The scaling manager failed to return an image pointer.");
  }
}

bool Layer::has_background_image() const {
  return scaling_manager != NULL;
}

void Layer::unset_image() throw(DegateLogicException) {
  if(scaling_manager == NULL) throw DegateLogicException("There is no scaling manager.");
  std::string img_dir = get_image_filename();
  scaling_manager.reset();
  debug(TM, "remove directory: %s", img_dir.c_str());
  remove_directory(img_dir);
}

ScalingManager_shptr Layer::get_scaling_manager() {
  return scaling_manager;
}

void Layer::print(std::ostream & os) {

  os 
    << "Layer position       : " << get_layer_pos() << std::endl
    << "Width                : " << get_width() << std::endl
    << "Height               : " << get_height() << std::endl
    << "Layer type           : " << get_layer_type_as_string() << std::endl
    << "Has background image : " << (has_background_image() ? "true" : "false") << std::endl
    << "Background image     : " << (has_background_image() ? get_image_filename() : "none" ) << std::endl
    << std::endl
    ;

  quadtree.print(os);
}

void Layer::notify_shape_change(object_id_t object_id) 
  throw(CollectionLookupException, InvalidObjectIDException){

  if(!object_id) 
    throw InvalidObjectIDException("Invalid object ID in Layer::notify_shape_change()");

  object_collection::iterator iter = objects.find(object_id);
  if(iter == objects.end())
    throw CollectionLookupException("Error in Layer::notify_shape_change(): "
				    "The object is not in the layer.");

  quadtree.notify_shape_change((*iter).second);
}


PlacedLogicModelObject_shptr Layer::get_object_at_position(int x, int y) {

  debug(TM, "get_object_at_position %d, %d", x, y);
  PlacedLogicModelObject_shptr plo;

  for(qt_region_iterator iter = quadtree.region_iter_begin(x, x, y, y);
      iter != quadtree.region_iter_end(); ++iter) {

    if(plo == NULL) plo = *iter;
    if(std::tr1::dynamic_pointer_cast<GatePort>(*iter) != NULL) {
      return *iter;
    }
  }
  return plo;
}


bool Layer::exists_gate_in_region(unsigned int min_x, unsigned int max_x,
				  unsigned int min_y, unsigned int max_y) {
  
  for(Layer::qt_region_iterator iter = quadtree.region_iter_begin(min_x, max_x, min_y, max_y);
      iter != quadtree.region_iter_end(); ++iter) {
    
    if(std::tr1::dynamic_pointer_cast<Gate>(*iter) != NULL) {
      return true;
    }
  }
  return false;
}


void Layer::set_enabled(bool state) {
  enabled = state;
}

bool Layer::is_enabled() const {
  return enabled;
}


std::string Layer::get_description() const {
  return description;
}


void Layer::set_description(std::string const& description) {
  this->description = description;
}
