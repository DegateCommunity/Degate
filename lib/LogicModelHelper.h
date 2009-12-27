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

#ifndef __LOGICMODELHELPER_H__
#define __LOGICMODELHELPER_H__

#include <degate.h>
#include <ImageHelper.h>
#include <ConnectedLogicModelObject.h>
#include <Project.h>

namespace degate {

  /**
   * Get the first layer of a layer type. The search order is
   * from bottom to top (lower layer position numbers to higher).
   * Method ignores disabled layers.
   * @exception InvalidPointerException Is thrown if you passed
   *   an invalid pointer for \p lmodel .
   * @exception DegateLogicException This exception is thrown
   *   if you passed an invalid \p layer_type.
   * @exception CollectionLookupException Is thrown if there is no
   *   layer of the requested type.
   */

  Layer_shptr get_first_layer(LogicModel_shptr lmodel, Layer::LAYER_TYPE layer_type) 
    throw(InvalidPointerException, CollectionLookupException, DegateLogicException);

  /**
   * Get the first layer that is a logic layer. The search order is
   * from bottom to top (lower layer position numbers to higher).
   * Method ignores disabled layers.
   * @exception InvalidPointerException Is thrown if you passed
   *   an invalid pointer for \p lmodel .
   * @exception CollectionLookupException Is thrown if there is no
   *   logc layer.
   */

  Layer_shptr get_first_logic_layer(LogicModel_shptr lmodel) 
    throw(InvalidPointerException, CollectionLookupException);



  /**
   * Lookup a gate by it's name.
   * The name should be unique. If there is more then one gate with that name,
   * the first one is returned.
   * @return Returns a shared pointer to the gate. This pointer can represent
   *  a NULL pointer, if no gate was found.
   */
  Gate_shptr get_gate_by_name(LogicModel_shptr lmodel, std::string const& gate_name);


  /**
   * Get the color definition for port names from the PortColorManager and
   * apply it to gate ports.
   */
  void apply_colors_to_gate_ports(LogicModel_shptr lmodel,
				  PortColorManager_shptr pcm);


  /**
   * Extract a partial image from the background image for a layer.
   * @exception DegateLogicException Is thrown if the layer has no
   *   background image set.
   */
  template<typename ImageType>
  std::tr1::shared_ptr<ImageType> grab_image(LogicModel_shptr lmodel,
					     Layer_shptr layer,
					     BoundingBox const& bounding_box) 
    throw(DegateLogicException) {

    // create empty image with the size of the bounding box
    std::tr1::shared_ptr<ImageType> new_img(new ImageType(bounding_box.get_width(),
							  bounding_box.get_height()));

    BackgroundImage_shptr bg_image = layer->get_image();
    if(bg_image == NULL) throw DegateLogicException("The layer has no background image");

    extract_partial_image<ImageType, BackgroundImage>(new_img, bg_image, bounding_box);    

    //save_image<ImageType>("/tmp/zzz.tif", new_img);

    return new_img;
  }

  /**
   * Extract a partial image from the background images for several layers
   * and set the extracted images as master images for a gate template.
   * This operation happens for the first transistor layer, for the first
   * logic layer and for the first metal layer, but only if a layer of a type
   * exists.
   * With the paramter \p orientation you can specify that a flipping
   * of the image is necessary. 
   */
  void grab_template_images(LogicModel_shptr lmodel,
			    GateTemplate_shptr gate_template,
			    BoundingBox const& bounding_box,
			    Gate::ORIENTATION orientation = Gate::ORIENTATION_NORMAL);


  /**
   * Collect nets that are used by the objects from \p first to \p last.
   * @exception DegateRuntimeException This exception is thrown if one of the objects
   *   is not of type ConnectedLogicModelObject. This means that the object cannot be
   *   connected with anything.
   */

  template<class InputIterator>
  std::set<Net_shptr> collect_nets(InputIterator first, InputIterator last) 
    throw(DegateRuntimeException) {

    std::set<Net_shptr> nets;

    for(InputIterator it = first; it != last; ++it) {
      ConnectedLogicModelObject_shptr clo = 
	std::tr1::dynamic_pointer_cast<ConnectedLogicModelObject>(*it);

      if(clo == NULL) {
	throw DegateRuntimeException("Error in connect_objecs(). One of the object "
				     "cannot be connected with anything");
      }
      else {
	Net_shptr net = clo->get_net();
	if(net != NULL) nets.insert(net);
      }

    }
    return nets;
  }


  /**
   * Isolate objects.
   *
   * Unused nets are removed from the logic model.
   *
   * @exception DegateRuntimeException This exception is thrown if one of the objects
   *   is not of type ConnectedLogicModelObject. This means that the object cannot be
   *   connected with anything.
   * @exception InvalidPointerException If you pass an invalid shared pointer for the
   *   logic model, then this exception is raised.
   */

  template<class InputIterator>
  void isolate_objects(LogicModel_shptr lmodel, InputIterator first, InputIterator last) 
    throw (DegateRuntimeException, InvalidPointerException) {

    if(lmodel == NULL) 
      throw InvalidPointerException("You passed an invalid shared pointer for lmodel");

    // collect nets
    std::set<Net_shptr> nets;
    try {
      nets = collect_nets<InputIterator>(first, last);
    }
    catch(DegateRuntimeException const& ex) {
      throw;
    }

    // unconnect objects
    for(InputIterator it = first; it != last; ++it) {
      ConnectedLogicModelObject_shptr clo = 
	std::tr1::dynamic_pointer_cast<ConnectedLogicModelObject>(*it);
      
      if(clo == NULL) {
	throw DegateRuntimeException("Error in connect_objecs(). One of the object "
				     "cannot be connected with anything");
      }
      else clo->remove_net();
    }

    // check nets: remove them from the logic model if they are not in use
    for(std::set<Net_shptr>::iterator iter = nets.begin(); iter != nets.end(); ++iter)
      if((*iter)->size() == 0) lmodel->remove_net(*iter);

  }




  /**
   * Connect objects.
   *
   * Unused nets are removed from the logic model.
   *
   * @exception DegateRuntimeException This exception is thrown if one of the objects
   *   is not of type ConnectedLogicModelObject. This means that the object cannot be
   *   connected with anything.
   * @exception InvalidPointerException If you pass an invalid shared pointer for the
   *   logic model, then this exception is raised.
   */

  template<class InputIterator>
  void connect_objects(LogicModel_shptr lmodel, InputIterator first, InputIterator last) 
    throw (DegateRuntimeException, InvalidPointerException) {

    if(lmodel == NULL) 
      throw InvalidPointerException("You passed an invalid shared pointer for lmodel");

    
    std::set<Net_shptr> nets;
    try {
      nets = collect_nets<InputIterator>(first, last);
    }
    catch(DegateRuntimeException const& ex) {
      throw;
    }


    // collect object we want to join
    std::set<ConnectedLogicModelObject_shptr> objects;

    for(InputIterator it = first; it != last; ++it) {
      objects.insert(std::tr1::dynamic_pointer_cast<ConnectedLogicModelObject>(*it));
    }

    for(std::set<Net_shptr>::iterator iter = nets.begin(); iter != nets.end(); ++iter) {

      Net_shptr net = *iter;

      for(Net::connection_iterator ci = net->begin(); ci != net->end(); ++ci) {
	PlacedLogicModelObject_shptr plo = lmodel->get_object(*ci);

	ConnectedLogicModelObject_shptr clo = 
	  std::tr1::dynamic_pointer_cast<ConnectedLogicModelObject>(plo);

	assert(clo != NULL);
	objects.insert(clo);
      }
    }


    Net_shptr new_net(new Net());
    
    // set new net
    for(std::set<ConnectedLogicModelObject_shptr>::iterator iter = objects.begin();
	iter != objects.end(); ++iter) {
      ConnectedLogicModelObject_shptr clo = *iter;
      clo->set_net(new_net);
    }
    

    // remove nets from the logic model
    for(std::set<Net_shptr>::iterator iter = nets.begin(); iter != nets.end(); ++iter) {
      assert((*iter)->size() == 0);
      lmodel->remove_net(*iter);
    }

    lmodel->add_net(new_net);
  }

  
  /**
   * Load an image in a common image format as background image for a layer.
   * If there is already a background image, it will be unset and removed from
   * the project directory.
   * @exception InvalidPointerException If you pass an invalid shared pointer for
   *   \p layer, then this exception is raised.
   * 
   */
  void load_background_image(Layer_shptr layer, 
			     std::string const& project_dir,
			     std::string const& image_file) throw(InvalidPointerException);

  /**
   * Clear the logic model for a layer.
   * @exception InvalidPointerException If you pass an invalid shared pointer for
   *   \p lmodel or \p layer, then this exception is raised.
   */

  void clear_logic_model(LogicModel_shptr lmodel, Layer_shptr layer) throw(InvalidPointerException);


  /**
   * Get first enabled layer
   * @exception InvalidPointerException If you pass an invalid shared pointer for
   *   \p layer, then this exception is raised.
   * @exception CollectionLookupException Is thrown, if all layers are disabled.
   */

  Layer_shptr get_first_enabled_layer(LogicModel_shptr lmodel) 
    throw(InvalidPointerException, CollectionLookupException);
  
  /**
   * Get next enabled layer.
   * @exception InvalidPointerException If you pass an invalid shared pointer for
   *   \p layer, then this exception is raised.
   * @exception CollectionLookupException Is thrown, if all layers are disabled.
   * @exception DegateRuntimeException Is thrown, if there is no current layer.
   */

  Layer_shptr get_next_enabled_layer(LogicModel_shptr lmodel) 
    throw(InvalidPointerException, CollectionLookupException, DegateRuntimeException);

  /**
   * Get previous enabled layer.
   * @exception InvalidPointerException If you pass an invalid shared pointer for
   *   \p layer, then this exception is raised.
   * @exception CollectionLookupException Is thrown, if all layers are disabled.
   * @exception DegateRuntimeException Is thrown, if there is no current layer.
   */

  Layer_shptr get_prev_enabled_layer(LogicModel_shptr lmodel) 
    throw(InvalidPointerException, CollectionLookupException, DegateRuntimeException);


  /**
   * Get the current layer.
   */
  Layer_shptr get_current_layer(Project_shptr project) throw(InvalidPointerException);

}

#endif
