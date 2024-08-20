/**
 * This file is part of the IC reverse engineering tool Degate.
 *
 * Copyright 2008, 2009, 2010 by Martin Schobert
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

#include "Core/LogicModel/LogicModelImporter.h"
#include "Core/LogicModel/Annotation/SubProjectAnnotation.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>
#include <memory>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <list>

#include <boost/format.hpp>
#include <boost/algorithm/string/case_conv.hpp>

using namespace std;
using namespace degate;

void LogicModelImporter::import_into(LogicModel_shptr lmodel,
                                     std::string const& filename)
{
    if (RET_IS_NOT_OK(check_file(filename)))
    {
        debug(TM, "Problem: file %s not found.", filename.c_str());
        throw InvalidPathException("Can't load logic model from file.");
    }

    try
    {
        QDomDocument parser;

        QFile file(QString::fromStdString(filename));
        if (!file.open(QIODevice::ReadOnly))
        {
            debug(TM, "Problem: can't open the file %s.", filename.c_str());
            throw InvalidFileFormatException(
                "The LogicModelImporter cannot load the project file. Can't open the file.");
        }

        if (!parser.setContent(&file))
        {
            debug(TM, "Problem: can't parse the file %s.", filename.c_str());
            throw InvalidXMLException("The LogicModelImporter cannot load the project file. Can't parse the file.");
        }
        file.close();

        const QDomElement root_elem = parser.documentElement();
        assert(!root_elem.isNull());

        lmodel->set_gate_library(gate_library);

        parse_logic_model_element(root_elem, lmodel);

        // check if the ports of placed standard cell are available and create them if necessary
        for (auto g : gates)
        {
            lmodel->update_ports(g);
        }
    }
    catch (const std::exception& ex)
    {
        std::cout << "Exception caught: " << ex.what() << std::endl;
        throw;
    }
}

LogicModel_shptr LogicModelImporter::import(std::string const& filename, ProjectType project_type)
{
    LogicModel_shptr lmodel(new LogicModel(width, height, project_type));
    assert(lmodel != nullptr);

    import_into(lmodel, filename);

    return lmodel;
}

void LogicModelImporter::parse_logic_model_element(QDomElement const lm_elem,
                                                   LogicModel_shptr lmodel)
{
    const QDomElement gates_elem = get_dom_twig(lm_elem, "gates");
    if (!gates_elem.isNull()) parse_gates_element(gates_elem, lmodel);

    const QDomElement vias_elem = get_dom_twig(lm_elem, "vias");
    if (!vias_elem.isNull()) parse_vias_element(vias_elem, lmodel);

    const QDomElement emarkers_elem = get_dom_twig(lm_elem, "emarkers");
    if (!emarkers_elem.isNull()) parse_emarkers_element(emarkers_elem, lmodel);

    const QDomElement wires_elem = get_dom_twig(lm_elem, "wires");
    if (!wires_elem.isNull()) parse_wires_element(wires_elem, lmodel);

    const QDomElement nets_elem = get_dom_twig(lm_elem, "nets");
    if (!nets_elem.isNull()) parse_nets_element(nets_elem, lmodel);

    const QDomElement annotations_elem = get_dom_twig(lm_elem, "annotations");
    if (!annotations_elem.isNull()) parse_annotations_element(annotations_elem, lmodel);

    const QDomElement modules_elem = get_dom_twig(lm_elem, "modules");
    if (!modules_elem.isNull())
    {
        std::list<Module_shptr> mods = parse_modules_element(modules_elem, lmodel);

        assert(mods.size() == 1);

        lmodel->set_main_module(mods.front());
    }
}

void LogicModelImporter::parse_nets_element(QDomElement const nets_element,
                                            LogicModel_shptr lmodel)
{
    if (nets_element.isNull() || lmodel == nullptr)
        throw InvalidPointerException("Got a nullptr pointer in  LogicModelImporter::parse_nets_element()");

    const QDomNodeList net_list = nets_element.elementsByTagName("net");
    for (int i = 0; i < net_list.count(); i++)
    {
        int object_counter = 0;

        QDomElement net_elem = net_list.at(i).toElement();
        if (!net_elem.isNull())
        {
            object_id_t net_id = parse_number<object_id_t>(net_elem, "id");

            Net_shptr net(new Net());
            net->set_object_id(net_id);

            const QDomNodeList connection_list = net_elem.elementsByTagName("connection");
            for (int j = 0; j < connection_list.count(); j++)
            {
                QDomElement conn_elem = connection_list.at(j).toElement();
                if (!conn_elem.isNull())
                {
                    object_id_t object_id = parse_number<object_id_t>(conn_elem, "object-id");

                    // add connection
                    try
                    {
                        PlacedLogicModelObject_shptr placed_object = lmodel->get_object(object_id);
                        if (placed_object == nullptr)
                        {
                            debug(TM,
                                  "Failed to lookup logic model object %llu. Can't connect it to net %llu.",
                                  object_id, net_id);
                        }
                        else
                        {
                            ConnectedLogicModelObject_shptr o =
                                std::dynamic_pointer_cast<ConnectedLogicModelObject>(placed_object);
                            if (o != nullptr)
                            {
                                o->set_net(net);
                            }
                            else
                            {
                                debug(TM, "Failed to dynamic_cast<> a logic model object with ID %llu", object_id);
                            }
                        }
                    }
                    catch (CollectionLookupException const&)
                    {
                        debug(TM,
                              "Failed to insert a connection for net %llu into the logic layer. "
                              "Can't lookup logic model object %llu that should be connected to that net.",
                              net_id, object_id);
                        throw; // rethrow
                    }
                } // end of if

                object_counter++;
            } // end of for

            if (object_counter < 2)
            {
                boost::format f("Net with ID %1% has only a single object. This should not occur.");
                f % net_id;
                std::cout << "WARNING: " << f.str() << std::endl;
                //throw DegateLogicException(f.str());
            }
            lmodel->add_net(net);
        }
    }
}

void LogicModelImporter::parse_wires_element(QDomElement const wires_element,
                                             LogicModel_shptr lmodel)
{
    if (wires_element.isNull() || lmodel == nullptr)
        throw InvalidPointerException("Null pointer in LogicModelImporter::parse_wires_element()");

    const QDomNodeList wire_list = wires_element.elementsByTagName("wire");
    for (int i = 0; i < wire_list.count(); i++)
    {
        QDomElement wire_elem = wire_list.at(i).toElement();
        if (!wire_elem.isNull())
        {
            // XXX PORT ID REPLACER ...

            object_id_t object_id = parse_number<object_id_t>(wire_elem, "id");
            float from_x = parse_number<float>(wire_elem, "from-x");
            float from_y = parse_number<float>(wire_elem, "from-y");
            float to_x = parse_number<float>(wire_elem, "to-x");
            float to_y = parse_number<float>(wire_elem, "to-y");
            int diameter = parse_number<int>(wire_elem, "diameter");
            int layer = parse_number<int>(wire_elem, "layer");
            int remote_id = parse_number<object_id_t>(wire_elem, "remote-id", 0);

            const std::string name(wire_elem.attribute("name").toStdString());
            const std::string description(wire_elem.attribute("description").toStdString());
            const std::string fill_color_str(wire_elem.attribute("fill-color").toStdString());
            const std::string frame_color_str(wire_elem.attribute("frame-color").toStdString());


            Wire_shptr wire(new Wire(from_x, from_y, to_x, to_y, diameter));
            wire->set_name(name.c_str());
            wire->set_description(description.c_str());
            wire->set_object_id(object_id);
            wire->set_fill_color(parse_color_string(fill_color_str));
            wire->set_frame_color(parse_color_string(frame_color_str));

            wire->set_remote_object_id(remote_id);
            lmodel->add_object(layer, wire);
        }
    }
}

void LogicModelImporter::parse_vias_element(QDomElement const vias_element,
                                            LogicModel_shptr lmodel)
{
    if (vias_element.isNull() || lmodel == nullptr) throw InvalidPointerException();

    const QDomNodeList via_list = vias_element.elementsByTagName("via");
    for (int i = 0; i < via_list.count(); i++)
    {
        QDomElement via_elem = via_list.at(i).toElement();
        if (!via_elem.isNull())
        {
            // XXX PORT ID REPLACER ...

            object_id_t object_id = parse_number<object_id_t>(via_elem, "id");
            float x = parse_number<float>(via_elem, "x");
            float y = parse_number<float>(via_elem, "y");
            int diameter = parse_number<int>(via_elem, "diameter");
            int layer = parse_number<int>(via_elem, "layer");
            int remote_id = parse_number<object_id_t>(via_elem, "remote-id", 0);

            const std::string name(via_elem.attribute("name").toStdString());
            const std::string description(via_elem.attribute("description").toStdString());
            const std::string fill_color_str(via_elem.attribute("fill-color").toStdString());
            const std::string frame_color_str(via_elem.attribute("frame-color").toStdString());
            const std::string direction_str(
                boost::algorithm::to_lower_copy(via_elem.attribute("direction").toStdString()));

            Via::DIRECTION direction;
            if (direction_str == "undefined") direction = Via::DIRECTION_UNDEFINED;
            else if (direction_str == "up") direction = Via::DIRECTION_UP;
            else if (direction_str == "down") direction = Via::DIRECTION_DOWN;
            else
            {
                boost::format f("Can't parse via direction type: %1%");
                f % direction_str;
                throw XMLAttributeParseException(f.str());
            }

            Via_shptr via(new Via(x, y, diameter, direction));
            via->set_name(name.c_str());
            via->set_description(description.c_str());
            via->set_object_id(object_id);
            via->set_fill_color(parse_color_string(fill_color_str));
            via->set_frame_color(parse_color_string(frame_color_str));

            via->set_remote_object_id(remote_id);

            lmodel->add_object(layer, via);
        }
    }
}

void LogicModelImporter::parse_emarkers_element(QDomElement const emarkers_element,
                                                LogicModel_shptr lmodel)
{
    if (emarkers_element.isNull() || lmodel == nullptr) throw InvalidPointerException();

    const QDomNodeList emarker_list = emarkers_element.elementsByTagName("emarker");
    for (int i = 0; i < emarker_list.count(); i++)
    {
        QDomElement emarker_elem = emarker_list.at(i).toElement();
        if (!emarker_elem.isNull())
        {
            // XXX PORT ID REPLACER ...

            object_id_t object_id = parse_number<object_id_t>(emarker_elem, "id");
            float x = parse_number<float>(emarker_elem, "x");
            float y = parse_number<float>(emarker_elem, "y");
            int diameter = parse_number<diameter_t>(emarker_elem, "diameter");
            int layer = parse_number<int>(emarker_elem, "layer");
            int remote_id = parse_number<object_id_t>(emarker_elem, "remote-id", 0);

            const std::string name(emarker_elem.attribute("name").toStdString());
            const std::string description(emarker_elem.attribute("description").toStdString());
            const bool is_module_port(emarker_elem.attribute("is-module-port", "0").toInt());
            const std::string fill_color_str(emarker_elem.attribute("fill-color").toStdString());
            const std::string frame_color_str(emarker_elem.attribute("frame-color").toStdString());
            const std::string direction_str(
                boost::algorithm::to_lower_copy(emarker_elem.attribute("direction").toStdString()));

            EMarker_shptr emarker(new EMarker(x, y, diameter, is_module_port));
            emarker->set_name(name.c_str());
            emarker->set_description(description.c_str());
            emarker->set_object_id(object_id);
            emarker->set_fill_color(parse_color_string(fill_color_str));
            emarker->set_frame_color(parse_color_string(frame_color_str));

            emarker->set_remote_object_id(remote_id);

            lmodel->add_object(layer, emarker);
        }
    }
}

void LogicModelImporter::parse_gates_element(QDomElement const gates_element,
                                             LogicModel_shptr lmodel)
{
    if (gates_element.isNull() || lmodel == nullptr) throw InvalidPointerException();


    const QDomNodeList gate_list = gates_element.elementsByTagName("gate");
    for (int i = 0; i < gate_list.count(); i++)
    {
        QDomElement gate_elem = gate_list.at(i).toElement();
        if (!gate_elem.isNull())
        {
            object_id_t object_id = parse_number<object_id_t>(gate_elem, "id");
            float min_x = parse_number<float>(gate_elem, "min-x");
            float min_y = parse_number<float>(gate_elem, "min-y");
            float max_x = parse_number<float>(gate_elem, "max-x");
            float max_y = parse_number<float>(gate_elem, "max-y");

            int layer = parse_number<int>(gate_elem, "layer");

            int gate_type_id = parse_number<int>(gate_elem, "type-id");
            const std::string name(gate_elem.attribute("name").toStdString());
            const std::string description(gate_elem.attribute("description").toStdString());
            const std::string orientation_str(
                boost::algorithm::to_lower_copy(gate_elem.attribute("orientation").toStdString()));
            const std::string frame_color_str(gate_elem.attribute("frame-color").toStdString());
            const std::string fill_color_str(gate_elem.attribute("fill-color").toStdString());

            Gate::ORIENTATION orientation;
            if (orientation_str == "undefined") orientation = Gate::ORIENTATION_UNDEFINED;
            else if (orientation_str == "normal") orientation = Gate::ORIENTATION_NORMAL;
            else if (orientation_str == "flipped-left-right") orientation = Gate::ORIENTATION_FLIPPED_LEFT_RIGHT;
            else if (orientation_str == "flipped-up-down") orientation = Gate::ORIENTATION_FLIPPED_UP_DOWN;
            else if (orientation_str == "flipped-both") orientation = Gate::ORIENTATION_FLIPPED_BOTH;
            else throw XMLAttributeParseException("Can't parse orientation type.");

            // create a new gate and add it into the logic model

            Gate_shptr gate(new Gate(min_x, max_x, min_y, max_y, orientation));
            gate->set_name(name.c_str());
            gate->set_description(description.c_str());
            gate->set_object_id(object_id);
            gate->set_template_type_id(gate_type_id);
            gate->set_fill_color(parse_color_string(fill_color_str));
            gate->set_frame_color(parse_color_string(frame_color_str));

            if (gate_library != nullptr && gate_type_id != 0)
            {
                GateTemplate_shptr tmpl = gate_library->get_template(gate_type_id);
                assert(tmpl != nullptr);
                gate->set_gate_template(tmpl);
            }

            // parse port instances
            const QDomNodeList port_list = gate_elem.elementsByTagName("port");
            for (int j = 0; j < port_list.count(); j++)
            {
                QDomElement port_elem = port_list.at(j).toElement();
                if (!port_elem.isNull())
                {
                    object_id_t template_port_id = parse_number<object_id_t>(port_elem, "type-id");

                    // create a new port
                    GatePort_shptr gate_port = std::make_shared<GatePort>(gate);
                    gate_port->set_object_id(parse_number<object_id_t>(port_elem, "id"));
                    gate_port->set_template_port_type_id(template_port_id);
                    gate_port->set_diameter(parse_number<diameter_t>(port_elem, "diameter", 5));

                    if (gate_library != nullptr)
                    {
                        GateTemplatePort_shptr tmpl_port = gate_library->get_template_port(template_port_id);
                        gate_port->set_template_port(tmpl_port);
                    }

                    gate->add_port(gate_port);
                }
            }

            lmodel->add_object(layer, gate);

            #if DEBUG_PROJECT_IMPORT
                gate->print();
            #endif

            // Collect placed standard cells in a first step.
            // Later we call lmodel->update_ports().
            gates.push_back(gate);
        }
    }
}


void LogicModelImporter::parse_annotations_element(QDomElement const annotations_element,
                                                   LogicModel_shptr lmodel)
{
    if (annotations_element.isNull() || lmodel == nullptr) throw InvalidPointerException();

    const QDomNodeList annotation_list = annotations_element.elementsByTagName("annotation");
    for (int i = 0; i < annotation_list.count(); i++)
    {
        QDomElement annotation_elem = annotation_list.at(i).toElement();
        if (!annotation_elem.isNull())
        {
            object_id_t object_id = parse_number<object_id_t>(annotation_elem, "id");

            float min_x = parse_number<float>(annotation_elem, "min-x");
            float min_y = parse_number<float>(annotation_elem, "min-y");
            float max_x = parse_number<float>(annotation_elem, "max-x");
            float max_y = parse_number<float>(annotation_elem, "max-y");

            int layer = parse_number<int>(annotation_elem, "layer");
            Annotation::class_id_t class_id = parse_number<Annotation::class_id_t>(annotation_elem, "class-id");

            const std::string name(annotation_elem.attribute("name").toStdString());
            const std::string description(annotation_elem.attribute("description").toStdString());
            const std::string fill_color_str(annotation_elem.attribute("fill-color").toStdString());
            const std::string frame_color_str(annotation_elem.attribute("frame-color").toStdString());


            Annotation_shptr annotation;

            if (class_id == Annotation::SUBPROJECT)
            {
                const std::string path = annotation_elem.attribute("subproject-directory").toStdString();
                annotation = std::make_shared<SubProjectAnnotation>(min_x, max_x, min_y, max_y, path);
            }
            else
                annotation = std::make_shared<Annotation>(min_x, max_x, min_y, max_y, class_id);

            annotation->set_name(name.c_str());
            annotation->set_description(description.c_str());
            annotation->set_object_id(object_id);
            annotation->set_fill_color(parse_color_string(fill_color_str));
            annotation->set_frame_color(parse_color_string(frame_color_str));

            lmodel->add_object(layer, annotation);
        }
    }
}

std::list<Module_shptr> LogicModelImporter::parse_modules_element(QDomElement const modules_element,
                                                                  LogicModel_shptr lmodel)
{
    if (modules_element.isNull() || lmodel == nullptr)
        throw InvalidPointerException("Got a nullptr pointer in LogicModelImporter::parse_modules_element()");

    std::list<Module_shptr> modules;

    QDomElement module_elem = modules_element.firstChildElement("module");

    while (!module_elem.isNull())
    {
        // parse module attributes
        object_id_t id = parse_number<object_id_t>(module_elem, "id");
        const std::string name(module_elem.attribute("name").toStdString());
        const std::string entity(module_elem.attribute("entity").toStdString());

        Module_shptr module(new Module(name, entity));
        module->set_object_id(id);

        // parse standard cell list
        const QDomElement cells_elem = module_elem.firstChildElement("cells");
        if (!cells_elem.isNull())
        {
            const QDomNodeList cell_list = cells_elem.elementsByTagName("cell");
            for (int j = 0; j < cell_list.count(); j++)
            {
                QDomElement cell_elem = cell_list.at(j).toElement();
                if (!cell_elem.isNull())
                {
                    object_id_t cell_id = parse_number<object_id_t>(cell_elem, "object-id");

                    // Lookup will throw an exception, if cell is not in the logic model. This is intended behaviour.
                    if (Gate_shptr gate = std::dynamic_pointer_cast<Gate>(lmodel->get_object(cell_id)))
                        module->add_gate(gate, /* autodetect module ports = */ false);
                }
            }
        }

        // parse module ports
        const QDomElement mports_elem = module_elem.firstChildElement("module-ports");
        if (!mports_elem.isNull())
        {
            const QDomNodeList mport_list = mports_elem.elementsByTagName("module-port");
            for (int k = 0; k < mport_list.count(); k++)
            {
                QDomElement mport_elem = mport_list.at(k).toElement();
                if (!mport_elem.isNull())
                {
                    const std::string port_name(mport_elem.attribute("name").toStdString());
                    object_id_t ref_id = parse_number<object_id_t>(mport_elem, "object-id");

                    // Lookup will throw an exception, if cell is not in the logic model. This is intended behaviour.
                    if (GatePort_shptr gport = std::dynamic_pointer_cast<GatePort>(lmodel->get_object(ref_id)))
                        module->add_module_port(port_name, gport);
                }
            }
        }


        // parse sub-modules
        const QDomElement sub_modules_elem = module_elem.firstChildElement("modules");
        if (!sub_modules_elem.isNull())
        {
            std::list<Module_shptr> sub_modules = parse_modules_element(sub_modules_elem, lmodel);
            for (auto submod : sub_modules)
            {
                module->add_module(submod);
            }
        }

        modules.push_back(module);

        module_elem = module_elem.nextSiblingElement("module");
    }

    return modules;
}
