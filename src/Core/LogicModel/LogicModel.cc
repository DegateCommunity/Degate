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

#include "Globals.h"
#include "Core/LogicModel/Layer.h"
#include "Core/LogicModel/Net.h"
#include "Core/LogicModel/ConnectedLogicModelObject.h"
#include "Core/LogicModel/Gate/GateLibrary.h"
#include "Core/LogicModel/LogicModel.h"

#include <algorithm>
#include <iterator>
#include <memory>


using namespace std;
using namespace degate;

std::shared_ptr<Layer> LogicModel::get_create_layer(layer_position_t pos)
{
    if (layers.size() <= pos || layers.at(pos) == nullptr)
    {
        add_layer(pos);
    }

    return layers[pos];
}

void LogicModel::print(std::ostream& os)
{
    os
        << endl
        << "--------------------------------[ Logic model ]--------------------------------" << endl;

    for (object_collection::iterator iter = objects.begin(); iter != objects.end(); ++iter)
    {
        os << "\t+ Object: "
            << (*iter).second->get_object_type_name() << " "
            << (*iter).second->get_object_id() << endl;

        // XXX dynamic cast and print
    }

    os << endl;

    os
        << endl
        << "--------------------------------[ Gate library ]--------------------------------" << endl;

    // print gate library
    if (gate_library) gate_library->print(os);

    os
        << endl
        << "--------------------------------[ Layers ]--------------------------------" << endl;

    // iterate over layers and print them

    for (layer_collection::iterator iter = layers.begin();
         iter != layers.end(); ++iter)
    {
        Layer_shptr layer = *iter;
        layer->print(os);
    }
}

bool LogicModel::exists_layer_id(layer_collection const& layers, layer_id_t lid) const
{
    for (auto l : layers)
    {
        if (l != nullptr && l->has_valid_layer_id() && l->get_layer_id() == lid)
            return true;
    }
    return false;
}

object_id_t LogicModel::get_new_object_id()
{
    object_id_t new_id = ++object_id_counter;
    while (objects.find(new_id) != objects.end() ||
        (gate_library != nullptr && (gate_library->exists_template(new_id) || gate_library->exists_template_port(new_id)))
        ||
        nets.find(new_id) != nets.end() ||
        exists_layer_id(layers, new_id))
    {
        new_id = ++object_id_counter;
    }
    return new_id;
}


LogicModel::LogicModel(unsigned int width, unsigned int height, ProjectType project_type, unsigned int layers) :
    bounding_box(static_cast<float>(width), static_cast<float>(height)),
    main_module(new Module("main_module", "", true)),
    object_id_counter(0),
    project_type(project_type)
{
    gate_library = std::make_shared<GateLibrary>();

    for (unsigned int i = 0; i < layers; i++)
        get_create_layer(i);

    if (layers > 0)
        set_current_layer(0);
}

LogicModel::~LogicModel()
{
}

DeepCopyable_shptr LogicModel::clone_shallow() const
{
    auto clone = std::make_shared<LogicModel>(*this);
    clone->layers.clear();
    clone->current_layer.reset();
    clone->gate_library.reset();
    clone->gates.clear();
    clone->wires.clear();
    clone->vias.clear();
    clone->emarkers.clear();
    clone->annotations.clear();
    clone->nets.clear();
    clone->objects.clear();
    clone->main_module.reset();
    return clone;
}

void LogicModel::clone_deep_into(DeepCopyable_shptr dest, oldnew_t* oldnew) const
{
    auto clone = std::dynamic_pointer_cast<LogicModel>(dest);

    // layers
    std::transform(layers.begin(), layers.end(), back_inserter(clone->layers), [&](const Layer_shptr& d)
    {
        Layer_shptr layer_cloned = std::dynamic_pointer_cast<Layer>(d->clone_deep(oldnew));
        if (d == current_layer)
        {
            clone->current_layer = layer_cloned;
        }
        return layer_cloned;
    });

    // gate_library
    clone->gate_library = std::dynamic_pointer_cast<GateLibrary>(gate_library->clone_deep(oldnew));

    // gates
    std::for_each(gates.begin(), gates.end(), [&](const gate_collection::value_type& v)
    {
        clone->gates[v.first] = std::dynamic_pointer_cast<Gate>(v.second->clone_deep(oldnew));
    });

    // wires
    std::for_each(wires.begin(), wires.end(), [&](const wire_collection::value_type& v)
    {
        clone->wires[v.first] = std::dynamic_pointer_cast<Wire>(v.second->clone_deep(oldnew));
    });

    // vias
    std::for_each(vias.begin(), vias.end(), [&](const via_collection::value_type& v)
    {
        clone->vias[v.first] = std::dynamic_pointer_cast<Via>(v.second->clone_deep(oldnew));
    });

    // emarkers
    std::for_each(emarkers.begin(), emarkers.end(), [&](const emarker_collection::value_type& v)
    {
        clone->emarkers[v.first] = std::dynamic_pointer_cast<EMarker>(v.second->clone_deep(oldnew));
    });

    // annotations
    std::for_each(annotations.begin(), annotations.end(), [&](const annotation_collection::value_type& v)
    {
        clone->annotations[v.first] = std::dynamic_pointer_cast<Annotation>(v.second->clone_deep(oldnew));
    });

    // nets
    std::for_each(nets.begin(), nets.end(), [&](const net_collection::value_type& v)
    {
        clone->nets[v.first] = std::dynamic_pointer_cast<Net>(v.second->clone_deep(oldnew));
    });

    // objects
    std::for_each(objects.begin(), objects.end(), [&](const object_collection::value_type& v)
    {
        clone->objects[v.first] = std::dynamic_pointer_cast<PlacedLogicModelObject>(v.second->clone_deep(oldnew));
    });

    // main_module
    clone->main_module = std::dynamic_pointer_cast<Module>(main_module->clone_deep(oldnew));
}

unsigned int LogicModel::get_width() const
{
    return static_cast<unsigned int>(bounding_box.get_width());
}

unsigned int LogicModel::get_height() const
{
    return static_cast<unsigned int>(bounding_box.get_height());
}

PlacedLogicModelObject_shptr LogicModel::get_object(object_id_t object_id)
{
    object_collection::iterator found = objects.find(object_id);

    if (found == objects.end())
    {
        std::ostringstream stm;
        stm << "Can't find object with id " << object_id << " in logic model.";
        throw CollectionLookupException(stm.str());
    }
    else
    {
        return found->second;
    }
}

void LogicModel::add_wire(int layer_pos, Wire_shptr o)
{
    if (o == nullptr) throw InvalidPointerException();
    if (!o->has_valid_object_id()) o->set_object_id(get_new_object_id());
    wires[o->get_object_id()] = o;
}

void LogicModel::add_via(int layer_pos, Via_shptr o)
{
    if (o == nullptr) throw InvalidPointerException(); //
    if (!o->has_valid_object_id()) o->set_object_id(get_new_object_id());
    vias[o->get_object_id()] = o;
}

void LogicModel::add_emarker(int layer_pos, EMarker_shptr o)
{
    if (o == nullptr) throw InvalidPointerException(); //
    if (!o->has_valid_object_id()) o->set_object_id(get_new_object_id());
    emarkers[o->get_object_id()] = o;
}

void LogicModel::add_annotation(int layer_pos, Annotation_shptr o)
{
    if (o == nullptr) throw InvalidPointerException();
    if (!o->has_valid_object_id()) o->set_object_id(get_new_object_id());
    annotations[o->get_object_id()] = o;
}

void LogicModel::add_gate(int layer_pos, Gate_shptr o)
{
    if (o == nullptr) throw InvalidPointerException();
    if (!o->has_valid_object_id()) o->set_object_id(get_new_object_id());
    gates[o->get_object_id()] = o;

    assert(main_module != nullptr);
    main_module->add_gate(o);

    // iterate over ports and add them into the lookup table
    for (Gate::port_iterator iter = o->ports_begin(); iter != o->ports_end(); ++iter)
    {
        assert(*iter != nullptr);
        assert((*iter)->has_valid_object_id() == true);

        add_object(layer_pos, std::dynamic_pointer_cast<PlacedLogicModelObject>(*iter));
    }
}

void LogicModel::remove_gate_ports(Gate_shptr o)
{
    if (o == nullptr) throw InvalidPointerException();
    // iterate over ports and remove them from the lookup table
    for (Gate::port_iterator iter = o->ports_begin(); iter != o->ports_end(); ++iter)
    {
        object_id_t port_id = (*iter)->get_object_id();
        remove_object(get_object(port_id));
    }
}

void LogicModel::remove_gate(Gate_shptr o)
{
    if (o == nullptr) throw InvalidPointerException();
    remove_gate_ports(o);
    debug(TM, "remove gate");
    gates.erase(o->get_object_id());

    main_module->remove_gate(o);
}

void LogicModel::remove_wire(Wire_shptr o)
{
    if (o == nullptr) throw InvalidPointerException();
    wires.erase(o->get_object_id());
}

void LogicModel::remove_via(Via_shptr o)
{
    if (o == nullptr) throw InvalidPointerException();
    vias.erase(o->get_object_id());
    //removed_remote_oids.push_back(o->get_remote_object_id());
}

void LogicModel::remove_emarker(EMarker_shptr o)
{
    if (o == nullptr) throw InvalidPointerException();
    emarkers.erase(o->get_object_id());
}

void LogicModel::remove_annotation(Annotation_shptr o)
{
    if (o == nullptr) throw InvalidPointerException();
    annotations.erase(o->get_object_id());
}


void LogicModel::add_object(int layer_pos, PlacedLogicModelObject_shptr o)
{
    if (o == nullptr) throw InvalidPointerException();
    if (!o->has_valid_object_id()) o->set_object_id(get_new_object_id());
    object_id_t object_id = o->get_object_id();

    if (Gate_shptr gate = std::dynamic_pointer_cast<Gate>(o))
        add_gate(layer_pos, gate);
    else if (Wire_shptr wire = std::dynamic_pointer_cast<Wire>(o))
        add_wire(layer_pos, wire);
    else if (Via_shptr via = std::dynamic_pointer_cast<Via>(o))
        add_via(layer_pos, via);
    else if (EMarker_shptr via = std::dynamic_pointer_cast<EMarker>(o))
        add_emarker(layer_pos, via);
    else if (Annotation_shptr annotation = std::dynamic_pointer_cast<Annotation>(o))
        add_annotation(layer_pos, annotation);


    // if it is a RemoteObject, update remote-to-local-id mapping
    if (RemoteObject_shptr ro = std::dynamic_pointer_cast<RemoteObject>(o))
    {
        update_roid_mapping(ro->get_remote_object_id(), o->get_object_id());
    }

    if (objects.find(object_id) != objects.end())
    {
        std::ostringstream stm;
        stm << "Logic model object with id " << object_id << " is already stored in the logic model.";
        std::cout << stm.str() << std::endl;
        throw DegateLogicException(stm.str());
    }
    else
    {
        objects[object_id] = o;
        Layer_shptr layer = get_create_layer(layer_pos);
        assert(layer != nullptr);
        o->set_layer(layer);
        layer->add_object(o);
    }
    assert(objects.find(object_id) != objects.end());
}


void LogicModel::remove_remote_object(object_id_t remote_id)
{
    debug(TM, "Should remove object with remote ID %llu from lmodel.", remote_id);

    if (remote_id == 0)
        throw InvalidObjectIDException("Parameter passed to remove_remote_object() is invalid.");

    debug(TM, "Should remove object with remote ID %llu from lmodel - 2.", remote_id);

    for (const auto& p : objects)
    {
        PlacedLogicModelObject_shptr plo = p.second;
        RemoteObject_shptr ro;

        if (ro = std::dynamic_pointer_cast<RemoteObject>(plo))
        {
            object_id_t local_id = plo->get_object_id();

            debug(TM, "found remote object with remote ID %llu and local ID = %llu.", ro->get_remote_object_id(), local_id);

            if (ro->get_remote_object_id() == remote_id)
            {
                debug(TM, "Removed object with remote ID %llu and local ID = %llu from lmodel.", remote_id, local_id);
                remove_object(plo, false);

                object_collection::iterator found = objects.find(local_id);
                assert(found == objects.end());

                return;
            }
        }
    }
}

void LogicModel::remove_object(PlacedLogicModelObject_shptr o, bool add_to_remove_list)
{
    if (o == nullptr) throw InvalidPointerException();
    Layer_shptr layer = o->get_layer();
    if (layer == nullptr)
    {
        debug(TM, "warning: object has no layer");
    }
    else
    {
        if (ConnectedLogicModelObject_shptr clmo =
            std::dynamic_pointer_cast<ConnectedLogicModelObject>(o))
        {
            Net_shptr net = clmo->get_net();
            clmo->remove_net();
            if (net != nullptr && net->size() == 0) remove_net(net);
        }

        if (Gate_shptr gate = std::dynamic_pointer_cast<Gate>(o))
            remove_gate(gate);
        else if (Wire_shptr wire = std::dynamic_pointer_cast<Wire>(o))
            remove_wire(wire);
        else if (Via_shptr via = std::dynamic_pointer_cast<Via>(o))
            remove_via(via);
        else if (EMarker_shptr emarker = std::dynamic_pointer_cast<EMarker>(o))
            remove_emarker(emarker);
        else if (Annotation_shptr annotation = std::dynamic_pointer_cast<Annotation>(o))
            remove_annotation(annotation);


        if (RemoteObject_shptr ro = std::dynamic_pointer_cast<RemoteObject>(o))
        {
            // remember to send a was-removed-message to the collaboration server
            if (add_to_remove_list) removed_remote_oids.push_back(ro->get_remote_object_id());

            // remove entry from remote-to-local-id mapping
            roid_mapping.erase(ro->get_remote_object_id());
        }

        layer->remove_object(o);
    }
    objects.erase(o->get_object_id());
}

void LogicModel::remove_object(PlacedLogicModelObject_shptr o)
{
    remove_object(o, true);
}


void LogicModel::add_gate_template(GateTemplate_shptr tmpl)
{
    if (gate_library != nullptr)
    {
        if (!tmpl->has_valid_object_id()) tmpl->set_object_id(get_new_object_id());
        gate_library->add_template(tmpl);
        //update_gate_ports(tmpl);

        // XXX iterate over gates and check tmpl-id -> update
    }
    else
    {
        throw DegateLogicException("You can't add a gate template, if there is no gate library.");
    }
}


void LogicModel::remove_gate_template(GateTemplate_shptr tmpl)
{
    if (gate_library == nullptr)
        throw DegateLogicException("You can't remove a gate template, if there is no gate library.");
    else
    {
        remove_gates_by_template_type(tmpl);
        gate_library->remove_template(tmpl);
    }
}

void LogicModel::remove_template_references(GateTemplate_shptr tmpl)
{
    if (gate_library == nullptr)
        throw DegateLogicException("You can't remove a gate template, if there is no gate library.");
    for (gate_collection::iterator iter = gates_begin();
         iter != gates.end(); ++iter)
    {
        Gate_shptr gate = (*iter).second;
        if (gate->get_gate_template() == tmpl)
        {
            remove_gate_ports(gate);
            gate->remove_template();
        }
    }
}


void LogicModel::remove_gates_by_template_type(GateTemplate_shptr tmpl)
{
    if (tmpl == nullptr) throw InvalidPointerException("The gate template pointer is invalid.");

    std::list<Gate_shptr> gates_to_remove;

    for (gate_collection::iterator iter = gates_begin();
         iter != gates_end(); ++iter)
    {
        Gate_shptr gate = (*iter).second;

        if (gate->get_gate_template() == tmpl)
            gates_to_remove.push_back(gate);
    }

    while (!gates_to_remove.empty())
    {
        remove_object(gates_to_remove.front());
        gates_to_remove.pop_front();
    }
}

void LogicModel::add_template_port_to_gate_template(GateTemplate_shptr gate_template,
                                                    GateTemplatePort_shptr template_port)
{
    gate_template->add_template_port(template_port);
    update_ports(gate_template);
}

void LogicModel::remove_template_port_from_gate_template(GateTemplate_shptr gate_template,
                                                         GateTemplatePort_shptr template_port)
{
    gate_template->remove_template_port(template_port);
    update_ports(gate_template);
}

void LogicModel::update_ports(Gate_shptr gate)
{
    if (gate == nullptr)
        throw InvalidPointerException("Invalid parameter for update_ports()");

    GateTemplate_shptr gate_template = gate->get_gate_template();

    debug(TM, "update ports on gate %llu", gate->get_object_id());

    // in a first iteration over all template ports from the corresponding template
    // we check if there are gate ports to add
    if (gate->has_template())
    {
        // iterate over template ports from the corresponding template

        debug(TM, "compare ports for gate with oid=%llu with corresponding template (oid=%llu)", gate->get_object_id(),
              gate_template->get_object_id());

        for (GateTemplate::port_iterator tmpl_port_iter = gate_template->ports_begin();
             tmpl_port_iter != gate_template->ports_end(); ++tmpl_port_iter)
        {
            GateTemplatePort_shptr tmpl_port = *tmpl_port_iter;
            assert(tmpl_port != nullptr);

            if (!gate->has_template_port(tmpl_port) && gate->has_orientation())
            {
                debug(TM, "adding a new port to gate, because the gate has no reference to the gate port template %llu.",
                      tmpl_port->get_object_id());
                GatePort_shptr new_gate_port(new GatePort(gate, tmpl_port, port_diameter));
                new_gate_port->set_object_id(get_new_object_id());
                gate->add_port(new_gate_port); // will set coordinates, too

                assert(gate->get_layer() != nullptr);
                add_object(gate->get_layer()->get_layer_pos(), new_gate_port);
            }
        }
    }

    std::list<GatePort_shptr> ports_to_remove;


    // iterate over gate ports
    for (Gate::port_iterator port_iter = gate->ports_begin();
         port_iter != gate->ports_end(); ++port_iter)
    {
        //debug(TM, "iterating over ports");
        GatePort_shptr gate_port = *port_iter;
        assert(gate_port != nullptr);

        if (gate->has_template())
        {
            GateTemplate_shptr tmpl = gate->get_gate_template();
            assert(tmpl != nullptr);

            GateTemplatePort_shptr tmpl_port = gate_port->get_template_port();
            assert(tmpl_port != nullptr);

            bool has_template_port = tmpl->has_template_port(tmpl_port->get_object_id());

            if (has_template_port)
            {
                GateTemplatePort_shptr tmpl_port = gate_port->get_template_port();
                // reset port coordinates
                if (gate->has_orientation())
                {
                    float x, y;
                    x = gate->get_relative_x_position_within_gate(tmpl_port->get_x());
                    y = gate->get_relative_y_position_within_gate(tmpl_port->get_y());
                    gate_port->set_x(x + gate->get_min_x());
                    gate_port->set_y(y + gate->get_min_y());
                    gate_port->set_name(tmpl_port->get_name());
                }
            }
                // unset port coordinates
            else
            {
                debug(TM, "should remove port with oid=%llu from gate with oid %llu", gate_port->get_object_id(),
                      gate->get_object_id());
                ports_to_remove.push_back(gate_port);
            }
        }
        else
        {
            debug(TM, "should remove port with oid=%llu from gate with oid=%llu, because gate has no template",
                  gate_port->get_object_id(), gate->get_object_id());
            ports_to_remove.push_back(gate_port);
        }
    }

    for (std::list<GatePort_shptr>::iterator iter = ports_to_remove.begin();
         iter != ports_to_remove.end(); ++iter)
    {
        debug(TM, "remove real port:");
        (*iter)->print();
        gate->remove_port(*iter);
        remove_object(*iter);
    }
}

void LogicModel::update_ports(GateTemplate_shptr gate_template)
{
    if (gate_template == nullptr)
        throw InvalidPointerException("Invalid parameter for update_ports()");

    // iterate over all gates ...
    for (gate_collection::iterator g_iter = gates.begin();
         g_iter != gates.end(); ++g_iter)
    {
        Gate_shptr gate = (*g_iter).second;
        if (gate->get_gate_template() == gate_template)
        {
            debug(TM, "update ports on gate with id %llu", gate->get_object_id());
            update_ports(gate);
        }
    }
}


layer_id_t LogicModel::get_new_layer_id()
{
    return get_new_object_id();
}

void LogicModel::add_layer(layer_position_t pos, Layer_shptr new_layer)
{
    if (layers.size() <= pos) layers.resize(pos + 1);

    if (layers[pos] != nullptr)
        throw DegateLogicException("There is already a layer for this layer number.");
    else
    {
        if (!new_layer->is_empty()) throw DegateLogicException("You must add an empty layer.");
        if (!new_layer->has_valid_layer_id()) new_layer->set_layer_id(get_new_layer_id());
        layers[pos] = new_layer;
        new_layer->set_layer_pos(pos);
    }

    if (current_layer == nullptr) current_layer = get_layer(0);
    if (current_layer == nullptr) current_layer = new_layer;
}


void LogicModel::add_layer(layer_position_t pos)
{
    Layer_shptr new_layer(new Layer(bounding_box, project_type));
    add_layer(pos, new_layer);
}

Layer_shptr LogicModel::get_layer(layer_position_t pos)
{
    return layers.at(pos);
}

Layer_shptr LogicModel::get_layer_by_id(layer_id_t lid)
{
    for (auto l : layers)
    {
        if (l->has_valid_layer_id() && l->get_layer_id() == lid)
            return l;
    }

    throw CollectionLookupException("Can't find a matching layer.");
}

void LogicModel::set_layers(layer_collection layers)
{
    std::list<Layer_shptr> layers_to_remove;


    if (this->layers.size() > 0)
    {
        /*
          We have a vector of old layers and should set a vector with new layers.
          Therefore we need to get a list of layers to remove.
        */

        // iterate over present (old) layers
        for (layer_collection::const_iterator i = this->layers.begin(); i != this->layers.end(); ++i)
        {
            bool found_in_new = false;
            for (layer_collection::const_iterator i2 = layers.begin(); i2 != layers.end(); ++i2)
            {
                if ((*i2)->get_layer_id() == (*i)->get_layer_id()) found_in_new = true;
            }

            if (!found_in_new) layers_to_remove.push_back(*i);
        }
    }

    for (auto l : layers_to_remove)
    {
        remove_layer(l);
    }

    // set new layers
    this->layers = layers;
}

void LogicModel::remove_layer(layer_position_t pos)
{
    remove_layer(layers.at(pos));
}

void LogicModel::remove_layer(Layer_shptr layer)
{
    // Iterate over layer objects and place them in a remove list.
    std::list<PlacedLogicModelObject_shptr> remove_list;

    for (Layer::object_iterator i = layer->objects_begin();
         i != layer->objects_end(); ++i)
        remove_list.push_back(*i);

    // Remove objects from logic model.
    for (auto o : remove_list)
    {
        remove_object(o);
    }

    // Unset background image. It will remove the image files, too.
    layer->unset_image();

    // Remove layer container.
    layers.erase(remove(layers.begin(), layers.end(), layer),
                 layers.end());
}

void LogicModel::set_current_layer(layer_position_t pos)
{
    current_layer = layers[pos];
}

Layer_shptr LogicModel::get_current_layer()
{
    return current_layer;
}

GateLibrary_shptr LogicModel::get_gate_library()
{
    return gate_library;
}

void LogicModel::set_gate_library(GateLibrary_shptr new_gate_lib)
{
    if (gate_library != nullptr)
    {
        // XXX
    }
    gate_library = new_gate_lib;
}

void LogicModel::add_net(Net_shptr net)
{
    if (net == nullptr) throw InvalidPointerException();

    if (!net->has_valid_object_id()) net->set_object_id(get_new_object_id());
    if (nets.find(net->get_object_id()) != nets.end())
    {
        boost::format f("Error in add_net(). Net with ID %1% already exists");
        f % net->get_object_id();
        throw DegateRuntimeException(f.str());
    }
    nets[net->get_object_id()] = net;
}


Net_shptr LogicModel::get_net(object_id_t net_id)
{
    if (nets.find(net_id) == nets.end())
    {
        boost::format f("Failed to get net with OID %1%, because it is not registered in the set of nets.");
        f % net_id;
        throw CollectionLookupException(f.str());
    }
    return nets[net_id];
}

void LogicModel::remove_net(Net_shptr net)
{
    if (!net->has_valid_object_id())
        throw InvalidObjectIDException("The net object has no object ID.");
    else if (nets.find(net->get_object_id()) == nets.end())
    {
        boost::format f("Failed to remove net with OID %1%, because it is not registered in the set of nets.");
        f % net->get_object_id();
        throw CollectionLookupException(f.str());
    }
    else
    {
        while (net->size() > 0)
        {
            // get an object ID from the net
            object_id_t oid = *(net->begin());

            // logic check: this object should be known
            if (objects.find(oid) == objects.end()) throw CollectionLookupException();

            // the logic model object should be connectable
            if (ConnectedLogicModelObject_shptr o =
                std::dynamic_pointer_cast<ConnectedLogicModelObject>(objects[oid]))
            {
                // unconnect object from net and net from object
                o->remove_net();
            }
            else
                throw DegateLogicException("Can't dynamic cast to a shared ptr of "
                    "ConnectedLogicModelObject, but the object "
                    "must be of that type, because it is "
                    "referenced from a net.");
        }

        // remove the net
        //nets[net->get_object_id()].reset();
        size_t n = nets.erase(net->get_object_id());
        assert(n == 1);
    }
}

LogicModel::object_collection::iterator LogicModel::objects_begin()
{
    return objects.begin();
}

LogicModel::object_collection::iterator LogicModel::objects_end()
{
    return objects.end();
}

LogicModel::gate_collection::iterator LogicModel::gates_begin()
{
    return gates.begin();
}

LogicModel::gate_collection::iterator LogicModel::gates_end()
{
    return gates.end();
}

LogicModel::via_collection::iterator LogicModel::vias_begin()
{
    return vias.begin();
}

LogicModel::via_collection::iterator LogicModel::vias_end()
{
    return vias.end();
}

LogicModel::layer_collection::iterator LogicModel::layers_begin()
{
    return layers.begin();
}

LogicModel::layer_collection::iterator LogicModel::layers_end()
{
    return layers.end();
}

LogicModel::net_collection::iterator LogicModel::nets_begin()
{
    return nets.begin();
}

LogicModel::net_collection::iterator LogicModel::nets_end()
{
    return nets.end();
}

LogicModel::annotation_collection::iterator LogicModel::annotations_begin()
{
    return annotations.begin();
}

LogicModel::annotation_collection::iterator LogicModel::annotations_end()
{
    return annotations.end();
}


unsigned int LogicModel::get_num_layers() const
{
    return static_cast<unsigned int>(layers.size());
}

Module_shptr LogicModel::get_main_module() const
{
    return main_module;
}

void LogicModel::set_main_module(Module_shptr main_module)
{
    this->main_module = main_module;
    main_module->set_main_module(); // set the root-node-state
}

void LogicModel::reset_removed_remote_objetcs_list()
{
    removed_remote_oids.clear();
}

std::list<object_id_t> const& LogicModel::get_removed_remote_objetcs_list()
{
    return removed_remote_oids;
}

void LogicModel::update_roid_mapping(object_id_t remote_oid, object_id_t local_oid)
{
    roid_mapping[remote_oid] = local_oid;
}

object_id_t LogicModel::get_local_oid_for_roid(object_id_t remote_oid)
{
    roid_mapping_t::const_iterator found = roid_mapping.find(remote_oid);
    if (found == roid_mapping.end())
        return 0;
    else
    {
        assert(found->second != 0);
        return found->second;
    }
}

void LogicModel::set_default_gate_port_diameter(diameter_t port_diameter)
{
    this->port_diameter = port_diameter;
}
