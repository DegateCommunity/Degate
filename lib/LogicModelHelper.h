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

#ifndef __LOGICMODELHELPER_H__
#define __LOGICMODELHELPER_H__

#include <degate.h>
#include <ImageHelper.h>
#include <ConnectedLogicModelObject.h>
#include <Project.h>
#include <ObjectSet.h>

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

  Layer_shptr get_first_layer(LogicModel_shptr lmodel, Layer::LAYER_TYPE layer_type);

  /**
   * Get the first layer that is a logic layer. The search order is
   * from bottom to top (lower layer position numbers to higher).
   * Method ignores disabled layers.
   * @exception InvalidPointerException Is thrown if you passed
   *   an invalid pointer for \p lmodel .
   * @exception CollectionLookupException Is thrown if there is no
   *   logc layer.
   */

  Layer_shptr get_first_logic_layer(LogicModel_shptr lmodel);



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
   * Get layer pointers for the first transistor layer and M1 and M2, if they are available.
   */
  std::list<Layer_shptr> get_available_standard_layers(LogicModel_shptr lmodel);

  /**
   * Extract a partial image from the background image for a layer.
   * @exception DegateLogicException Is thrown if the layer has no
   *   background image set.
   */
  template<typename ImageType>
  std::shared_ptr<ImageType> grab_image(LogicModel_shptr lmodel,
					     Layer_shptr layer,
					     BoundingBox const& bounding_box) {

    // create empty image with the size of the bounding box
    std::shared_ptr<ImageType> new_img(new ImageType(bounding_box.get_width(),
							  bounding_box.get_height()));

    BackgroundImage_shptr bg_image = layer->get_image();
    if(bg_image == NULL) throw DegateLogicException("The layer has no background image");

    extract_partial_image<ImageType, BackgroundImage>(new_img, bg_image, bounding_box);

    //save_image<ImageType>("/tmp/zzz.tif", new_img);

    return new_img;
  }

  /**
   * Merge images.
   */
  void merge_gate_images(LogicModel_shptr lmodel,
			 Layer_shptr layer,
			 GateTemplate_shptr tmpl, std::list<Gate_shptr> const& gates);

  /**
   * Merge images.
   * @param lmodel
   * @param gates A set of objects. It can contain non-gate types too.
   */
  void merge_gate_images(LogicModel_shptr lmodel,
			 ObjectSet gates);

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
  std::set<Net_shptr> collect_nets(InputIterator first, InputIterator last) {

    std::set<Net_shptr> nets;

    for(InputIterator it = first; it != last; ++it) {
      ConnectedLogicModelObject_shptr clo =
	std::dynamic_pointer_cast<ConnectedLogicModelObject>(*it);

      if(clo == NULL) {
	throw DegateRuntimeException("Error in collect_nets(). One of the objects "
				     "cannot be connected with anything.");
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
  void isolate_objects(LogicModel_shptr lmodel, InputIterator first, InputIterator last) {

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
	std::dynamic_pointer_cast<ConnectedLogicModelObject>(*it);

      if(clo == NULL) {
	throw DegateRuntimeException("Error in isolate_objecs(). One of the object "
				     "cannot be connected with anything");
      }
      else clo->remove_net();
    }

    // check nets: remove them from the logic model if they are not in use
    for(std::set<Net_shptr>::iterator iter = nets.begin(); iter != nets.end(); ++iter)
      if((*iter)->size() == 0) lmodel->remove_net(*iter);

  }


  /**
   * Remove net from the logic model and remove it from all objects, which share a net.
   */
  void remove_entire_net(LogicModel_shptr lmodel, Net_shptr net);


  /**
   * Connect objects.
   */
  void connect_objects(LogicModel_shptr lmodel,
		       ConnectedLogicModelObject_shptr o1,
		       ConnectedLogicModelObject_shptr o2);


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
   * @see connect_objects()
   * @see autoconnect_objects()
   */

  template<class InputIterator>
  void connect_objects(LogicModel_shptr lmodel, InputIterator first, InputIterator last) {

    if(lmodel == NULL)
      throw InvalidPointerException("You passed an invalid shared pointer for lmodel");


    std::set<Net_shptr> nets;
    try {
      nets = collect_nets<InputIterator>(first, last);
    }
    catch(DegateRuntimeException const& ex) {
      throw;
    }


    // collect objects we want to join
    std::set<ConnectedLogicModelObject_shptr> objects;

    for(InputIterator it = first; it != last; ++it) {
      objects.insert(std::dynamic_pointer_cast<ConnectedLogicModelObject>(*it));
    }

    for(std::set<Net_shptr>::iterator iter = nets.begin(); iter != nets.end(); ++iter) {

      Net_shptr net = *iter;

      for(Net::connection_iterator ci = net->begin(); ci != net->end(); ++ci) {
	PlacedLogicModelObject_shptr plo = lmodel->get_object(*ci);

	ConnectedLogicModelObject_shptr clo =
	  std::dynamic_pointer_cast<ConnectedLogicModelObject>(plo);

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
   * Autoconnect objects that tangent each other from a layer within the bounding box.
   *
   * @exception InvalidPointerException If you pass an invalid shared pointer for the
   *   logic model, then this exception is raised.
   * @see connnect_objects()
   */

  void autoconnect_objects(LogicModel_shptr lmodel, Layer_shptr layer,
			   BoundingBox const& search_bbox);


  /**
   * Autoconnect vias on adjacent enabled layers.
   * @exception InvalidPointerException If you pass an invalid shared pointer for the
   *   logic model, then this exception is raised.
   */
  void autoconnect_interlayer_objects(LogicModel_shptr lmodel,
				      Layer_shptr layer,
				      BoundingBox const& search_bbox);


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
			     std::string const& image_file);

  /**
   * Clear the logic model for a layer.
   * @exception InvalidPointerException If you pass an invalid shared pointer for
   *   \p lmodel or \p layer, then this exception is raised.
   */

  void clear_logic_model(LogicModel_shptr lmodel, Layer_shptr layer);


  /**
   * Get first enabled layer
   * @exception InvalidPointerException If you pass an invalid shared pointer for
   *   \p layer, then this exception is raised.
   * @exception CollectionLookupException Is thrown, if all layers are disabled.
   */

  Layer_shptr get_first_enabled_layer(LogicModel_shptr lmodel);

  /**
   * Get next enabled layer.
   * @return Returns the "next" visible layer relative to the current layer. If
   *   the current layer is the top layer, the bottom layer is returned.
   * @exception InvalidPointerException If you pass an invalid shared pointer for
   *   \p layer, then this exception is raised.
   * @exception CollectionLookupException Is thrown, if all layers are disabled.
   * @exception DegateRuntimeException Is thrown, if there is no current layer.
   */

  Layer_shptr get_next_enabled_layer(LogicModel_shptr lmodel);


  /**
   * Get the next enabled layer, that is above \p layer. The method will not
   * turn around in the layer stack.
   * @return Returns the next layer. If there is no next layer, a NULL pointer
   *   is returned.
   * @exception InvalidPointerException If you pass an invalid shared pointer,
   *   then this exception is raised.
   */
  Layer_shptr get_next_enabled_layer(LogicModel_shptr lmodel, Layer_shptr layer);

  /**
   * Get previous enabled layer.
   * @return Returns the "previous" visible layer relative to the current layer. If
   *   the current layer is the bottom layer, the top layer is returned.
   * @exception InvalidPointerException If you pass an invalid shared pointer for
   *   \p layer, then this exception is raised.
   * @exception CollectionLookupException Is thrown, if all layers are disabled.
   * @exception DegateRuntimeException Is thrown, if there is no current layer.
   */

  Layer_shptr get_prev_enabled_layer(LogicModel_shptr lmodel);


  /**
   * Get the previous enabled layer, that is below \p layer. The method will not
   * turn around in the layer stack.
   * @return Returns the previous layer. If there is no previous layer, a NULL pointer
   *   is returned.
   * @exception InvalidPointerException If you pass an invalid shared pointer,
   *   then this exception is raised.
   */

  Layer_shptr get_prev_enabled_layer(LogicModel_shptr lmodel, Layer_shptr layer);

  /**
   * Get the current layer.
   */

  Layer_shptr get_current_layer(Project_shptr project);


  /**
   * Check if a gate is of a specific logic class.
   */

  bool is_logic_class(Gate_shptr gate, std::string const& logic_class) ;


  /**
   * Get the port type of a gate port.
   * @return Returns the port type or GateTemplatePort::PORT_TYPE_UNDEFINED if the port type
   *   cannot be determined.
   */

  GateTemplatePort::PORT_TYPE get_port_type(GatePort_shptr gate_port);

  /**
   * Get the name of a corresponding template port
   * @return Returns the name of the corresponding template name. If there is no name
   *   an empty string is returned.
   */

  std::string get_template_port_name(GatePort_shptr gate_port);


  /**
   * Apply port color definitions to all gate template ports.
   */
  void apply_port_color_settings(LogicModel_shptr lmodel, PortColorManager_shptr pcm);


  /**
   * Resize all gate ports from the logic model to the new size.
   */
  void update_port_diameters(LogicModel_shptr lmodel, diameter_t new_size);

}

#endif
