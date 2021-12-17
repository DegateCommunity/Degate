/**
 * This file is part of the IC reverse engineering tool Degate.
 *
 * Copyright 2008, 2009, 2010 by Martin Schobert
 * Copyright 2012 Robert Nitsch
 * Copyright 2019-2020 Dorian Bachelot
 *
 * Degate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * Degate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with degate. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __LAYER_H__
#define __LAYER_H__

#include "Globals.h"

#include "Core/Primitive/Rectangle.h"
#include "Core/Primitive/QuadTree.h"
#include "Core/LogicModel/PlacedLogicModelObject.h"

#include "Core/Image/Image.h"
#include "Core/Image/Manipulation/ScalingManager.h"

#include <set>
#include <stdexcept>

namespace degate
{
    /**
     * Representation of a chip layer.
     */
    class Layer : public DeepCopyable
    {
        friend class LogicModel;

    public:

        /**
         * Enums to declare the type of a layer.
         */
        enum LAYER_TYPE
        {
            UNDEFINED = 0,
            METAL = 1,
            LOGIC = 2,
            TRANSISTOR = 3
        };

        typedef std::shared_ptr<PlacedLogicModelObject> quadtree_element_type;

        typedef RegionIterator<quadtree_element_type> qt_region_iterator;
        typedef qt_region_iterator                    object_iterator;

    private:

        QuadTree<quadtree_element_type> quadtree;

        LAYER_TYPE layer_type;

        layer_position_t layer_pos;

        std::shared_ptr<ScalingManager<BackgroundImage>> scaling_manager;

        // store shared pointers to objects, that belong to the layer
        typedef std::map<object_id_t, PlacedLogicModelObject_shptr> object_collection;
        object_collection objects;

        bool enabled;
        std::string description;

        layer_id_t layer_id;

        ProjectType project_type;

    protected:

        /**
         * Add an logic model object into this layer.
         * @throw DegateRuntimeException Is thrown if the object
         *   cannot be inserted into the quadtree.
         * @throw DegateLogicException
         */
        void add_object(std::shared_ptr<PlacedLogicModelObject> o);


        /**
         * Remove object from layer.
         * @throw DegateRuntimeException Is thrown if the object
         *   cannot be removed from the quadtree.
         */
        void remove_object(std::shared_ptr<PlacedLogicModelObject> o);

        /**
         * Get the object corresponding to the object_id.
         *
         * @exception CollectionLookupException This exception is thrown if
         *    there is no object in the layer, that has this object ID.
         * @exception InvalidObjectIDException Is raised, if \p object_id
         *    has an invalid ID.
         *
         * @param object_id : the object id.
         */
        std::shared_ptr<PlacedLogicModelObject> get_object(object_id_t object_id);

    public:


        /**
         * Create a new logic model layer.
         */
        Layer(BoundingBox const& bbox, ProjectType project_type, LAYER_TYPE layer_type = Layer::UNDEFINED);

        /**
         * Create a new logic model layer.
         */
        Layer(BoundingBox const& bbox, ProjectType project_type, LAYER_TYPE layer_type, BackgroundImage_shptr img);

        /**
         * Destruct a layer.
         */
        virtual ~Layer();

        //@{
        DeepCopyable_shptr clone_shallow() const override;
        void clone_deep_into(DeepCopyable_shptr destination, oldnew_t* oldnew) const override;
        //@}

        /**
         * Get the width of a layer.
         */
        unsigned int get_width() const;

        /**
         * Get the height of a layer.
         */
        unsigned int get_height() const;

        /**
         * Get the bounding box for a layer.
         */
        BoundingBox const& get_bounding_box() const;

        /**
         * Get layer type of this layer as human readable string, e.g. the string
         * "metal" for a layer of type Layer::METAL .
         */
        const std::string get_layer_type_as_string() const;

        /**
         * Get a layer type type as human readable string.
         */
        static const std::string get_layer_type_as_string(LAYER_TYPE layer_type);

        /**
         * Parse a layer type indicating string.
         * @exception DegateRuntimeException This exception is thrown if the string
         *   cannot be parsed.
         */
        static LAYER_TYPE get_layer_type_from_string(std::string const& layer_type_str);


        /**
         * Get layer type.
         */
        LAYER_TYPE get_layer_type() const;

        /**
         * Set layer type.
         */
        void set_layer_type(LAYER_TYPE layer_type);


        /**
         * Check if a layer has logic model objects or not.
         */
        bool is_empty() const;


        /**
         * Get the position of the layer within the layer stack.
         */
        layer_position_t get_layer_pos() const;


        /**
         * Get an iterator to iterate over all placed objects.
         */
        object_iterator objects_begin();

        /**
         * Get an end iterator.
         */
        object_iterator objects_end();

        /**
         * Get an iterator to iterate over a region.
         */
        qt_region_iterator region_begin(int min_x, int max_x, int min_y, int max_y);

        /**
         * Get an iterator to iterate over a region.
         */
        qt_region_iterator region_begin(BoundingBox const& bbox);

        /**
         * Get an end marker for region iteration.
         */
        qt_region_iterator region_end();


        /**
         * Set the background image for a layer.
         * Calling this method will also initialize the ScalingManager, that
         * handles multiple prescaled versions of the background image.
         * The ScalingManager will write the prescaled images into subdirectories
         * of the the background image.
         */
        void set_image(BackgroundImage_shptr img);


        /**
         * Get the background image.
         * @return Returns a shared pointer to the background image.
         * @exception DegateLogicException If you did not set the background image, then this
         *   exception is thrown.
         * @see set_image()
         */
        BackgroundImage_shptr get_image();

        /**
         * Get the path name for the image, that represents the
         * background image of the layer.
         * @exception DegateLogicException If you did not set the background image, then this
         *   exception is thrown.
         */
        std::string get_image_filename() const;

        /**
         * Check if the layer has a background image.
         */
        bool has_background_image() const;


        /**
         * Unset the background image.
         * This will destroy the image and it's scaling manager object and it will remove
         * the data from the project dir.
         * @exception DegateLogicException This excpetion is thrown if there is no background image.
         */
        void unset_image();

        /**
         * Get the scaling manager.
         * If you want to access the background image of a layer, that is the
         * chip surface depicting image, you have to ask for the ScalingMananger.
         * From the scaling mananger you will get the image.
         * @return Returns a shared pointer to the  scaling manager object.
         *   The pointer can be a nullptr pointer. This is the case if you did not
         *   initialized it via set_image()
         * @see set_image()
         */
        ScalingManager_shptr get_scaling_manager();

        /**
         * Print the layer.
         */
        void print(std::ostream& os = std::cout);

        /**
         * Notify the layer that a shape of a logic model object changed.
         * This will adjust the quadtree.
         *
         * It will use the old bounding box to remove the object and the actual one (the new one) to insert it back.
         *
         * @exception CollectionLookupException This exception is thrown if
         *    there is no object in the layer, that has this object ID.
         * @exception InvalidObjectIDException Is raised, if \p object_id
         *    has an invalid ID.
         *
         * @param object : the object.
         * @param old_bb : the old bounding box of the object.
         */
        void notify_shape_change(object_id_t object_id, const BoundingBox& old_bb);

        /**
         * Get an object at a specific position.
         * If multiple objects are placed at coordinate \p x, \p y, then if
         * there is a port it will be returned, if not and if there is a gate
         * it will be returned and if there is no port and no gate and an
         * annotation then it will be returned.
         * @param x The x-position.
         * @param y The y-position.
         * @param max_distance It is possible to check for objects, which are
         *   not directly placed in a way, that \p x and \p y are within the shape
         *   of an object. Therefore the parameter \p max_distance specifies an
         *   allowed distance to the object.
         * @param ignore_annotations If true no annotations will be considered.
         * @param ignore_gates If true no gates will be considered.
         * @param ignore_ports If true no ports will be considered.
         * @param ignore_emarkers If true no emarkers will be considered.
         * @param ignore_vias If true no vias will be considered.
         * @param ignore_wires If true no wires will be considered.
         * @return If there is an object at the position, a shared pointer to
         *   it is returned. If there is no object, then a nullptr pointer representation
         *   is returned.
         */
        PlacedLogicModelObject_shptr get_object_at_position(float x,
                                                            float y,
                                                            float max_distance = 0,
                                                            bool ignore_annotations = false,
                                                            bool ignore_gates = false,
                                                            bool ignore_ports = false,
                                                            bool ignore_emarkers = false,
                                                            bool ignore_vias = false,
                                                            bool ignore_wires = false);

        /**
         * Check for placed objects in a region of type given by template param.
         * @return Returns true, if there is a an object of the specified type in the region.
         *   Else it returns false.
         */
        template <typename LogicModelObjectType>
        bool exists_type_in_region(unsigned int min_x, unsigned int max_x,
                                   unsigned int min_y, unsigned int max_y)
        {
            for (Layer::qt_region_iterator iter = quadtree.region_iter_begin(min_x, max_x, min_y, max_y);
                 iter != quadtree.region_iter_end(); ++iter)
            {
                if (std::dynamic_pointer_cast<LogicModelObjectType>(*iter) != nullptr)
                {
                    return true;
                }
            }
            return false;
        }


        /**
         * Check for placed gates in a region and return the distance to
         * the boundary.
         * @return Returns the distance from \p x to the right boundary or
         *   from \p y to the bottom boundary depending on \p query_horizontal_distance.
         *   If there is no gate, this method returns 0.
         */
        unsigned int get_distance_to_gate_boundary(unsigned int x, unsigned int y,
                                                   bool query_horizontal_distance = true,
                                                   unsigned int width = 0,
                                                   unsigned int height = 0);


        /**
         * Enable a layer.
         */
        void set_enabled(bool state = true);

        /**
         * Check if a layer is enabled.
         * Enabled means, that the layer is visible to the user.
         */
        bool is_enabled() const;


        /**
         * Get layer description.
         */
        std::string get_description() const;


        /**
         * Set layer description.
         */
        void set_description(std::string const& description);


        /**
         * Set layer position.
         */
        void set_layer_pos(layer_position_t pos) { layer_pos = pos; }

        /**
         * Set the layer ID. The layer ID is not an object ID. The only requirement is
         * that each layer has a unique and position-independend ID.
         */
        virtual void set_layer_id(layer_id_t lid) { layer_id = lid; }

        /**
         * Get the layer ID.
         */
        virtual layer_id_t get_layer_id() const { return layer_id; }

        /**
         * Check if the layer has a valid layer ID.
         */
        virtual bool has_valid_layer_id() const { return layer_id != 0; }

        /**
         * Get project type.
         */
        inline ProjectType get_project_type() const { return project_type; };
    };
}

#endif
