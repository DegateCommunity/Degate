/* -*-c++-*-

  This file is part of the IC reverse engineering tool degate.

  Copyright 2008, 2009, 2010 by Martin Schobert
  Copyright 2012 Robert Nitsch

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

#ifndef __LOGICMODEL_H__
#define __LOGICMODEL_H__

#include <globals.h>
#include <LogicModelObjectBase.h>
#include <PlacedLogicModelObject.h>
#include <Net.h>
#include <Layer.h>

#include <Rectangle.h>

#include <Via.h>
#include <Wire.h>
#include <DeepCopyable.h>
#include <EMarker.h>
#include <Gate.h>
#include <GatePort.h>
#include <GateTemplate.h>
#include <GateTemplatePort.h>
#include <GateLibrary.h>
#include <Annotation.h>
#include <Module.h>

#include <memory>
#include <set>
#include <map>
#include <sstream>
#include <iostream>

namespace degate {

  /**
   * This class represents the logic model.
   *
   * Note: This class is central in degate. So it tends to be somehow bloated.
   * Convenience methods should go into LogicModelHelper.h
   *
   * @todo implement a move_object
   */
  class LogicModel : public DeepCopyable {

  public:

    typedef std::map<object_id_t, PlacedLogicModelObject_shptr> object_collection;
    typedef std::map<object_id_t, Net_shptr> net_collection;
    typedef std::map<object_id_t, Annotation_shptr> annotation_collection;
    typedef std::map<object_id_t, Via_shptr > via_collection;

    typedef std::vector<Layer_shptr> layer_collection;
    typedef std::map<object_id_t, Gate_shptr > gate_collection;
    typedef std::map<object_id_t, Wire_shptr > wire_collection;
    typedef std::map<object_id_t, EMarker_shptr> emarker_collection;
    
  private:

    BoundingBox bounding_box;

    layer_collection layers; // x
    Layer_shptr current_layer;

    std::shared_ptr<GateLibrary> gate_library; // x

    gate_collection gates;
    wire_collection wires;
    via_collection vias;
    emarker_collection emarkers;
    annotation_collection annotations;
    net_collection nets;
    Module_shptr main_module;

    /**
     * Contains any placeable object.
     */
    object_collection objects;


    /**
     * Counter to generate new object IDs.
     */
    object_id_t object_id_counter;


    /**
     * List of remote objects, that were deleted from the logic model.
     */
    std::list<object_id_t> removed_remote_oids;

    typedef std::map<object_id_t, object_id_t> roid_mapping_t;

    /**
     * Mapping from remote OIDs to local OIDs.
     */
    roid_mapping_t roid_mapping;

    diameter_t port_diameter; 

  private:

    /**
     * Get a layer. Create the layer if it doesn't exists.
     * @see get_layer
     */
    Layer_shptr get_create_layer(layer_position_t pos);

    /**
     * Add a wire into the logic model. If the layer doesn't exists, the layer is created implicitly.
     * If the wire has no object ID, a new object ID for the wire is generated.
     * @param layer_pos The layer position (starting at 0).
     * @param o A shared pointer to the object.
     */

    void add_wire(int layer_pos, Wire_shptr o);

    /**
     * Add a via into the logic model. If the layer doesn't exists, the layer is created implicitly.
     * If the via has no object ID, a new object ID for the via is generated.
     * @param layer_pos The layer position (starting at 0).
     * @param o A shared pointer to the object.
     */

    void add_via(int layer_pos, Via_shptr o);

    /**
     * Add an emarker into the logic model. If the layer doesn't exists, the layer is created implicitly.
     * If the emarker has no object ID, a new object ID for the emarker is generated.
     * @param layer_pos The layer position (starting at 0).
     * @param o A shared pointer to the object.
     */

    void add_emarker(int layer_pos, EMarker_shptr o);

    /**
     * Add an annotation into the logic model. If the layer doesn't exists, the layer is created implicitly.
     * If the annotation has no object ID, a new object ID for the via is generated.
     * @param layer_pos The layer position (starting at 0).
     * @param o A shared pointer to the object.
     */

    void add_annotation(int layer_pos, Annotation_shptr o);


    /**
     * Add a gate into the logic model. If the layer doesn't exists, the layer is
     * created implicitly.
     * If the gate has no object ID, a new object ID for the gate is generated.
     * Port are implicitly added, too.
     * @param layer_pos The layer position (starting at 0).
     * @param o A shared pointer to the object.
     */

    void add_gate(int layer_pos, Gate_shptr o);


    /**
     * Remove all ports from the logic model for a given gate.
     * @param o A shared pointer to the object.
     */

    void remove_gate_ports(Gate_shptr o);

    /**
     * Remove a gate from the logic model.
     * This effects the module hierarchy, too.
     * @param o A shared pointer to the object.
     */

    void remove_gate(Gate_shptr o);

    /**
     * Remove a wire from the logic model.
     * @param o A shared pointer to the object.
     */

    void remove_wire(Wire_shptr o);

    /**
     * Remove a via from the logic model.
     * @param o A shared pointer to the object.
     */

    void remove_via(Via_shptr o);

    /**
     * Remove an emarker from the logic model.
     * @param o A shared pointer to the object.
     */

    void remove_emarker(EMarker_shptr o);

    /**
     * Remove an annotation from the logic model.
     * @param o A shared pointer to the object.
     */

    void remove_annotation(Annotation_shptr o);


    /**
     * Remove an onject from the logic model and control if the operation
     * should be remembered in delete log.
     */
    void remove_object(PlacedLogicModelObject_shptr o,  bool add_to_remove_list);


    /**
     * Create a new layer ID.
     */
    layer_id_t get_new_layer_id();

    bool exists_layer_id(layer_collection const& layers, layer_id_t lid) const;

  public:

    /**
     * The constructor for the logic model.
     */

    LogicModel(unsigned int width, unsigned int height, unsigned int layers = 0);


    /**
     * The destructor.
     */

    virtual ~LogicModel();

    //@{
    DeepCopyable_shptr cloneShallow() const;
    void cloneDeepInto(DeepCopyable_shptr destination, oldnew_t *oldnew) const;
    //@}
    
    /**
     * Get the width of logic model.
     */

    unsigned int get_width() const;

    /**
     * Get the height of logic model.
     */

    unsigned int get_height() const;


    /**
     * Get a new unique logic model object ID.
     */

    object_id_t get_new_object_id();


    /**
     * Lookup an object from the logic model for a given object ID.
     * @exception CollectionLookupException Is thrown if there is
     *   no logic model object with that object ID.
     * @return Returns a shared pointer to the logic model.
     */

    PlacedLogicModelObject_shptr get_object(object_id_t object_id);


    /**
     * Add a generic logic model object into the logic model. If the layer doesn't
     * exists, the layer is created implicitly.
     * If the object has no object ID, a new object ID is generated.
     *
     * This method will set a reference to the layer, where the object is placed,
     * into the object, such that each placed object knows, where on which
     * layer it is placed.
     *
     * @param layer_pos The layer position (starting at 0).
     * @param o A shared pointer to the object.
     * @exception DegateLogicException This exception is thrown, if an object with the
     *            same object ID is already in the logic model.
     */

    void add_object(int layer_pos, PlacedLogicModelObject_shptr o);

    void add_object(Layer_shptr layer, PlacedLogicModelObject_shptr o) {
      add_object(layer->get_layer_pos(), o);
    }


    /**
     * Remove a generic logic model object from the logic model.
     * @param o A shared pointer to the object.
     * @todo fix it
     */

    void remove_object(PlacedLogicModelObject_shptr o);

    /**
     * Remove a remote object.
     * @exception InvalidObjectIDException This exception is thrown, if remote_id is invalid.
     */

    void remove_remote_object(object_id_t remote_id);


    /**
     * Add a gate template to the gate library and update the logic model.
     * All placed gates that already reference this gate template via an ID become updated.
     * If the template has no object ID defined, a new object ID for the template is set.
     * @exception DegateLogicException Is thrown if there is no gate library.
     * @see GateLibrary::add_gate_template
     *
     * @todo Implement update.
     * @todo should call add_object() for the port
     */

    void add_gate_template(GateTemplate_shptr tmpl);

    /**
     * Remove a gate template from the gate library and all placed gates that refer
     * to the template. This will destroy gate ports as well.
     *
     * @see add_gate_template()
     * @see GateLibrary::remove_gate_template()
     * @see remove_object()
     * @exception DegateLogicException Is thrown, if there is no gate library.
     * @todo should call remove_object() for the port
     * @todo The method name does not express, that the gates are removed as well.
     *     The method should be named remove_template_and_gates() and remove_gate_template()
     *     should only unreference and destroy the template.
     */

    void remove_gate_template(GateTemplate_shptr tmpl);

    /**
     * Remove the reference to a certain gate template from all gates.
     * This will destroy gate ports as well.
     * @exception DegateLogicException Is thrown, if there is no gate library.
     * @see Gate::remove_template()
     * @todo should call remove_object() for the port
     */

    void remove_template_references(GateTemplate_shptr tmpl);


    /**
     * Remove all gates, that reference a template. The template is preserved.
     * It will remove the gate ports as well.
     */

    void remove_gates_by_template_type(GateTemplate_shptr tmpl);

    /**
     * Add a template port to a gate template and make relevant updates in the logic model.
     * If you add or remove ports from a gate template, the "real" gates of that type must be updated.
     * The corresponding ports must be added to or removed from the "real" gates.
     * @todo should call add_object() for the port
     */

    void add_template_port_to_gate_template(GateTemplate_shptr gate_template,
					    GateTemplatePort_shptr template_port);


    /**
     * Remove a template port from a gate template and make relevant
     * updates in the logic model. If you add or remove ports from a
     * gate template, the "real" gates of that type must be updated.
     * The corresponding ports must be added to or removed from the
     * "real" gates. This method does this.
     * @todo should call add_object() for the port.
     */

    void remove_template_port_from_gate_template(GateTemplate_shptr gate_template,
						 GateTemplatePort_shptr template_port);


    /**
     * Add an empty layer.
     * @exception DegateLogicException Throws an exception, if you add a layer
     *            on a position where another layer object is already stored.
     *            If the layer that should be added already contains logic model objects, this
     *            exception is thrown, too.
     */

    void add_layer(layer_position_t pos, Layer_shptr new_layer);

    /**
     * Add an empty layer.
     * @see add_layer()
     */

    void add_layer(layer_position_t pos);

    /**
     * Get a layer by its posiion index.
     */

    Layer_shptr get_layer(layer_position_t pos);

    /**
     * Get a layer by its ID.
     * @exception CollectionLookupException This exception is thrown, if there is no matching layer.
     */
    Layer_shptr get_layer_by_id(layer_id_t lid);


    /**
     * Remove a layer.
     * @see remove_layer()
     */

    void remove_layer(layer_position_t pos);

    /**
     * Set layers.
     */

    void set_layers(layer_collection layers);

    /**
     * Remove a layer from the logic model.
     * A layer contains logical objects. These object are referred in other parts
     * of the logic model. References in other parts must be cleaned as well.
     * @todo Implement this.
     */

    void remove_layer(Layer_shptr layer);

    /**
     * Set the current layer you are working on.
     */

    void set_current_layer(layer_position_t pos);

    /**
     * Get the current layer.
     */

    Layer_shptr get_current_layer();


    /**
     * Get the current gate library for this logic model.
     */

    GateLibrary_shptr get_gate_library();

    /**
     * Set the gate library.
     * @todo: Implement updates on the logic model.
     */

    void set_gate_library(GateLibrary_shptr new_gate_lib);


    /**
     * Add a net to the logic model.
     * If the net object has no object ID, a new object ID is set to the net.
     * @exception InvalidPointerException Is thrown, if an invalid pointer was
     *   passed as argument.
     */

    void add_net(Net_shptr net);


    /**
     * Get a net from the logic model.
     * @exception CollectionLookupException Is thrown if there is
     *   no logic model object with that object ID.     
     */

    Net_shptr get_net(object_id_t net_id);

    /**
     * Remove a net from the logic model.
     * @exception InvalidPointerException Is thrown, if an invalid pointer was
     *   passed as argument.
     * @exception CollectionLookupException Is thrown if there is
     *   no logic model object with that object ID.
     */

    void remove_net(Net_shptr net);


    /**
     * Get a iterator to iterate over all placeable objects.
     */

    object_collection::iterator objects_begin();


    /**
     * Get an end iterator for the iteration over all placeable objects.
     */

    object_collection::iterator objects_end();


    /**
     * Get a iterator to iterate over all gates.
     */

    gate_collection::iterator gates_begin();


    /**
     * Get an end iterator for the iteration over all gates.
     */

    gate_collection::iterator gates_end();

    /**
     * Get a iterator to iterate over all vias.
     */

    via_collection::iterator vias_begin();

    /**
     * Get an end iterator for the iteration over all vias.
     */

    via_collection::iterator vias_end();

    /**
     * Get a iterator to iterate over all placeable objects.
     */

    layer_collection::iterator layers_begin();

    /**
     * Get an end iterator for the iteration over all placeable objects.
     */

    layer_collection::iterator layers_end();


    /**
     * Get a iterator to iterate over all nets.
     */

    net_collection::iterator nets_begin();


    /**
     * Get an end iterator for the iteration over all nets.
     */

    net_collection::iterator nets_end();


    /**
     * Get a iterator to iterate over all annotations.
     */

    annotation_collection::iterator annotations_begin();


    /**
     * Get an end iterator for the iteration over all annotations.
     */

    annotation_collection::iterator annotations_end();


    /**
     * Print the content of the logic model into an ostream.
     */
    void print(std::ostream & os = std::cout);


    /**
     * Get number of layers.
     */
    unsigned int get_num_layers() const;


    /**
     * Compare ports of a gate with template ports of its associated
     * template and update them.
     */

    void update_ports(Gate_shptr gate);

    /**
     * Compare ports of all gates that reference a given template
     * and update them.
     */

    void update_ports(GateTemplate_shptr gate_template);


    /**
     * Get the main module.
     */

    Module_shptr get_main_module() const;

    /**
     * Set main module.
     */
    void set_main_module(Module_shptr main_module);

    /**
     *
     */
    void reset_removed_remote_objetcs_list();

    std::list<object_id_t> const & get_removed_remote_objetcs_list();

    void update_roid_mapping(object_id_t remote_oid, object_id_t local_oid);

    object_id_t get_local_oid_for_roid(object_id_t remote_oid);


    /**
     * Set default gate port diameter.
     */
    void set_default_gate_port_diameter(diameter_t port_diameter);

  };



}

#endif
