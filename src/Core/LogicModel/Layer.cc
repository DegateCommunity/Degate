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

#include "Core/LogicModel/Layer.h"
#include "Core/LogicModel/Gate/Gate.h"
#include "Core/LogicModel/Via/Via.h"

#include <memory>

using namespace degate;

void Layer::add_object(std::shared_ptr<PlacedLogicModelObject> o)
{
    if (o->get_bounding_box() == BoundingBox(0, 0, 0, 0))
    {
        boost::format fmter("Error in add_object(): Object %1% with ID %2% has an "
            "undefined bounding box. Can't insert it into the quadtree");
        fmter % o->get_object_type_name() % o->get_object_id();
        throw DegateLogicException(fmter.str());
    }

    if (RET_IS_NOT_OK(quadtree.insert(o)))
    {
        debug(TM, "Failed to insert object into quadtree.");
        throw DegateRuntimeException("Failed to insert object into quadtree.");
    }
    objects[o->get_object_id()] = o;
}

void Layer::remove_object(std::shared_ptr<PlacedLogicModelObject> o)
{
    if (RET_IS_NOT_OK(quadtree.remove(o)))
    {
        debug(TM, "Failed to remove object from quadtree.");
        throw std::runtime_error("Failed to remove object from quadtree.");
    }

    objects.erase(o->get_object_id());
}

std::shared_ptr<PlacedLogicModelObject> Layer::get_object(object_id_t object_id)
{
    if (!object_id)
        throw InvalidObjectIDException("Invalid object ID in Layer::notify_shape_change()");

    auto iter = objects.find(object_id);
    if (iter == objects.end())
        throw CollectionLookupException("Error in Layer::notify_shape_change(): The object is not in the layer.");

    return (*iter).second;
}

Layer::Layer(BoundingBox const& bbox, ProjectType project_type, Layer::LAYER_TYPE layer_type) :
    quadtree(bbox, 100),
    layer_type(layer_type),
    layer_pos(0),
    enabled(true),
    layer_id(0),
    project_type(project_type)
{
}

Layer::Layer(BoundingBox const& bbox, ProjectType project_type, Layer::LAYER_TYPE layer_type,
             BackgroundImage_shptr img) :
    quadtree(bbox, 100),
    layer_type(layer_type),
    layer_pos(0),
    enabled(true),
    layer_id(0),
    project_type(project_type)
{
    set_image(img);
}


Layer::~Layer()
{
}

/**
 * @todo Check whether scaling_manager can really be reused by clones without trouble.
 */
DeepCopyable_shptr Layer::clone_shallow() const
{
    auto clone = std::make_shared<Layer>(quadtree.get_bounding_box(), project_type, layer_type);
    clone->layer_pos = layer_pos;
    clone->enabled = enabled;
    clone->description = description;
    clone->layer_id = layer_id;
    clone->scaling_manager = scaling_manager;
    return clone;
}

void Layer::clone_deep_into(DeepCopyable_shptr dest, oldnew_t* oldnew) const
{
    auto clone = std::dynamic_pointer_cast<Layer>(dest);

    // quadtree
    std::vector<quadtree_element_type> quadtree_elems;
    quadtree.get_all_elements(quadtree_elems);
    std::for_each(quadtree_elems.begin(), quadtree_elems.end(), [=,&clone](const quadtree_element_type& t)
    {
        clone->quadtree.insert(std::dynamic_pointer_cast<PlacedLogicModelObject>(t->clone_deep(oldnew)));
    });

    // objects
    std::for_each(objects.begin(), objects.end(), [&](object_collection::value_type v)
    {
        clone->objects[v.first] = std::dynamic_pointer_cast<PlacedLogicModelObject>(v.second->clone_deep(oldnew));
    });
}

unsigned int Layer::get_width() const
{
    return quadtree.get_width();
}

unsigned int Layer::get_height() const
{
    return quadtree.get_height();
}

BoundingBox const& Layer::get_bounding_box() const
{
    return quadtree.get_bounding_box();
}


const std::string Layer::get_layer_type_as_string() const
{
    return get_layer_type_as_string(layer_type);
}

const std::string Layer::get_layer_type_as_string(LAYER_TYPE layer_type)
{
    switch (layer_type)
    {
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
{
    if (layer_type_str == "metal") return Layer::METAL;
    else if (layer_type_str == "logic") return Layer::LOGIC;
    else if (layer_type_str == "transistor") return Layer::TRANSISTOR;
    else if (layer_type_str == "undefined") return Layer::UNDEFINED;
    else throw DegateRuntimeException("Can't parse layer type.");
}


Layer::LAYER_TYPE Layer::get_layer_type() const
{
    return layer_type;
}

void Layer::set_layer_type(LAYER_TYPE layer_type)
{
    this->layer_type = layer_type;
}


bool Layer::is_empty() const
{
    return quadtree.is_empty();
}

layer_position_t Layer::get_layer_pos() const
{
    return layer_pos;
}

Layer::object_iterator Layer::objects_begin()
{
    return quadtree.region_iter_begin();
}

Layer::object_iterator Layer::objects_end()
{
    return quadtree.region_iter_end();
}

Layer::qt_region_iterator Layer::region_begin(int min_x, int max_x, int min_y, int max_y)
{
    return quadtree.region_iter_begin(min_x, max_x, min_y, max_y);
}

Layer::qt_region_iterator Layer::region_begin(BoundingBox const& bbox)
{
    return quadtree.region_iter_begin(bbox);
}

Layer::qt_region_iterator Layer::region_end()
{
    return quadtree.region_iter_end();
}

void Layer::set_image(BackgroundImage_shptr img)
{
    scaling_manager = std::make_shared<ScalingManager<BackgroundImage>>(img, img->get_path(), project_type);

    scaling_manager->create_scalings();
}

BackgroundImage_shptr Layer::get_image()
{
    if (scaling_manager != nullptr)
    {
        ScalingManager<BackgroundImage>::image_map_element p = scaling_manager->get_image(1);
        return p.second;
    }
    else throw DegateLogicException("You have to set the background image first.");
}

std::string Layer::get_image_filename() const
{
    if (scaling_manager == nullptr)
        throw DegateLogicException("There is no scaling manager.");
    else
    {
        const ScalingManager<BackgroundImage>::image_map_element p = scaling_manager->get_image(1);

        if (p.second != nullptr)
            return p.second->get_path();
        else
            throw DegateLogicException("The scaling manager failed to return an image pointer.");
    }
}

bool Layer::has_background_image() const
{
    return scaling_manager != nullptr;
}

void Layer::unset_image()
{
    if (!has_background_image())
        return;

    if (scaling_manager == nullptr) throw DegateLogicException("There is no scaling manager.");

    // Release (cache) memory
    auto images = scaling_manager->get_images();
    for (auto& image : images)
        image.second->release_memory();

    std::string img_dir = get_image_filename();
    scaling_manager.reset();

    debug(TM, "remove directory: %s", img_dir.c_str());
    remove_directory(img_dir);
}

ScalingManager_shptr Layer::get_scaling_manager()
{
    return scaling_manager;
}

void Layer::print(std::ostream& os)
{
    os
        << "Layer position       : " << get_layer_pos() << std::endl
        << "Width                : " << get_width() << std::endl
        << "Height               : " << get_height() << std::endl
        << "Layer type           : " << get_layer_type_as_string() << std::endl
        << "Has background image : " << (has_background_image() ? "true" : "false") << std::endl
        << "Background image     : " << (has_background_image() ? get_image_filename() : "none") << std::endl
        << std::endl;

    quadtree.print(os);
}

void Layer::notify_shape_change(object_id_t object_id, const BoundingBox& old_bb)
{
    quadtree.notify_shape_change(get_object(object_id), old_bb);
}


enum class PlacedLogicModelObjectType
{
    NONE,
    GATE_PORT,
    VIA,
    EMARKER,
    GATE,
    ANNOTATION,
    WIRE
};

PlacedLogicModelObject_shptr Layer::get_object_at_position(float x, float y, float max_distance, bool ignore_annotations, bool ignore_gates, bool ignore_ports, bool ignore_emarkers, bool ignore_vias, bool ignore_wires)
{
    PlacedLogicModelObject_shptr object = nullptr;
    auto type = PlacedLogicModelObjectType::NONE;

    for (qt_region_iterator iter = quadtree.region_iter_begin(static_cast<int>(std::floor(x - max_distance)),
                                                              static_cast<int>(std::ceil(x + max_distance)),
                                                              static_cast<int>(std::floor(y - max_distance)),
                                                              static_cast<int>(std::ceil(y + max_distance)));
         iter != quadtree.region_iter_end(); ++iter)
    {
        if ((*iter)->in_shape(x, y, max_distance))
        {
            if (std::dynamic_pointer_cast<GatePort>((*iter)) != nullptr && !ignore_ports)
            {
                object = (*iter);
                type = PlacedLogicModelObjectType::GATE_PORT;
            }
            else if (std::dynamic_pointer_cast<Via>((*iter)) != nullptr && !ignore_vias)
            {
                if (type == PlacedLogicModelObjectType::GATE_PORT)
                    continue;

                object = (*iter);
                type = PlacedLogicModelObjectType::VIA;
            }
            else if (std::dynamic_pointer_cast<EMarker>((*iter)) != nullptr && !ignore_emarkers)
            {
                if (type == PlacedLogicModelObjectType::GATE_PORT ||
                    type == PlacedLogicModelObjectType::VIA)
                    continue;

                object = (*iter);
                type = PlacedLogicModelObjectType::EMARKER;
            }
            else if (std::dynamic_pointer_cast<Gate>((*iter)) != nullptr && !ignore_gates)
            {
                if (type == PlacedLogicModelObjectType::GATE_PORT ||
                    type == PlacedLogicModelObjectType::VIA ||
                    type == PlacedLogicModelObjectType::EMARKER)
                    continue;

                object = (*iter);
                type = PlacedLogicModelObjectType::GATE;
            }
            else if (std::dynamic_pointer_cast<Annotation>((*iter)) != nullptr && !ignore_annotations)
            {
                if (type == PlacedLogicModelObjectType::GATE_PORT ||
                    type == PlacedLogicModelObjectType::VIA ||
                    type == PlacedLogicModelObjectType::EMARKER ||
                    type == PlacedLogicModelObjectType::GATE)
                    continue;

                object = (*iter);
                type = PlacedLogicModelObjectType::ANNOTATION;
            }
            else if (std::dynamic_pointer_cast<Wire>((*iter)) != nullptr && !ignore_wires)
            {
                if (type == PlacedLogicModelObjectType::GATE_PORT ||
                    type == PlacedLogicModelObjectType::VIA ||
                    type == PlacedLogicModelObjectType::EMARKER ||
                    type == PlacedLogicModelObjectType::GATE ||
                    type == PlacedLogicModelObjectType::ANNOTATION)
                    continue;

                object = (*iter);
                type = PlacedLogicModelObjectType::WIRE;
            }

        }
    }

    return object;
}

unsigned int Layer::get_distance_to_gate_boundary(unsigned int x, unsigned int y,
                                                  bool query_horizontal_distance,
                                                  unsigned int width,
                                                  unsigned int height)
{
    for (Layer::qt_region_iterator iter = quadtree.region_iter_begin(x, x + width, y, y + height);
         iter != quadtree.region_iter_end(); ++iter)
    {
        if (Gate_shptr gate = std::dynamic_pointer_cast<Gate>(*iter))
        {
            if (query_horizontal_distance)
            {
                assert(gate->get_max_x() >= (int)x);
                return static_cast<unsigned int>(gate->get_max_x()) - x;
            }
            else
            {
                assert(gate->get_max_y() >= (int)y);
                return static_cast<unsigned int>(gate->get_max_y()) - y;
            }
        }
    }

    return 0;
}


void Layer::set_enabled(bool state)
{
    enabled = state;
}

bool Layer::is_enabled() const
{
    return enabled;
}


std::string Layer::get_description() const
{
    return description;
}


void Layer::set_description(std::string const& description)
{
    this->description = description;
}
