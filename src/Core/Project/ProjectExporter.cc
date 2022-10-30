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

#include "Globals.h"
#include "Core/LogicModel/Layer.h"
#include "Core/Project/ProjectExporter.h"
#include "Core/Utils/ObjectIDRewriter.h"
#include "Core/LogicModel/LogicModelExporter.h"
#include "Core/LogicModel/Gate/GateLibraryExporter.h"
#include "Core/RuleCheck/RCVBlacklistExporter.h"
#include "Core/Version.h"

#include <cerrno>
#include <iostream>
#include <stdexcept>
#include <list>
#include <memory>

using namespace std;
using namespace degate;

void ProjectExporter::export_all(std::string const& project_directory, const Project_shptr& prj,
                                 bool enable_oid_rewrite,
                                 std::string const& project_file,
                                 std::string const& lmodel_file,
                                 std::string const& gatelib_file,
                                 std::string const& rcbl_file)
{
    if (!is_directory(project_directory))
    {
        throw InvalidPathException("The path where the project should be exported to is not a directory.");
    }
    else
    {
        ObjectIDRewriter_shptr oid_rewriter(new ObjectIDRewriter(enable_oid_rewrite));

        export_data(join_pathes(project_directory, project_file), prj);

        LogicModel_shptr lmodel = prj->get_logic_model();

        if (lmodel != nullptr)
        {
            LogicModelExporter lm_exporter(oid_rewriter);
            string lm_filename(join_pathes(project_directory, lmodel_file));
            lm_exporter.export_data(lm_filename, lmodel);


            RCVBlacklistExporter rcv_exporter(oid_rewriter);
            rcv_exporter.export_data(join_pathes(project_directory, rcbl_file), prj->get_rcv_blacklist());

            GateLibrary_shptr glib = lmodel->get_gate_library();
            if (glib != nullptr)
            {
                GateLibraryExporter gl_exporter(oid_rewriter);
                gl_exporter.export_data(join_pathes(project_directory, gatelib_file), glib);
            }
        }
    }
}

void ProjectExporter::export_data(std::string const& filename, const Project_shptr& prj)
{
    if (prj == nullptr) throw InvalidPointerException("Project pointer is nullptr.");

    try
    {
        QDomDocument doc;

        QDomProcessingInstruction head = doc.createProcessingInstruction("xml", XML_ENCODING);
        doc.appendChild(head);

        QDomElement root_elem = doc.createElement("project");
        assert(!root_elem.isNull());

        set_project_node_attributes(doc, root_elem, prj);

        add_layers(doc, root_elem, prj->get_logic_model(), prj->get_project_directory());
        add_grids(doc, root_elem, prj);
        add_colors(doc, root_elem, prj);
        add_port_colors(doc, root_elem, prj->get_port_color_manager());

        doc.appendChild(root_elem);

        QFile file(QString::fromStdString(filename));
        if (!file.open(QIODevice::WriteOnly))
        {
            throw InvalidPathException("Can't create export file.");
        }

        QTextStream stream(&file);
        stream.setEncoding(QStringConverter::Utf8);
        stream << doc.toString();

        file.close();
    }
    catch (const std::exception& ex)
    {
        std::cout << "Exception caught: " << ex.what() << std::endl;
        throw;
    }
}

void ProjectExporter::add_grids(QDomDocument& doc, QDomElement& prj_elem, const Project_shptr& prj)
{
    QDomElement grids_elem = doc.createElement("grids");

    if (grids_elem.isNull()) throw(std::runtime_error("Failed to create node."));

    add_regular_grid(doc, grids_elem, prj->get_regular_horizontal_grid(), "horizontal");
    add_regular_grid(doc, grids_elem, prj->get_regular_vertical_grid(), "vertical");

    add_irregular_grid(doc, grids_elem, prj->get_irregular_horizontal_grid(), "horizontal");
    add_irregular_grid(doc, grids_elem, prj->get_irregular_vertical_grid(), "vertical");

    prj_elem.appendChild(grids_elem);
}

void ProjectExporter::add_regular_grid(QDomDocument& doc,
                                       QDomElement& grids_elem,
                                       const RegularGrid_shptr& grid,
                                       std::string const& grid_orientation)
{
    QDomElement grid_elem = doc.createElement("regular-grid");
    if (grid_elem.isNull()) throw(std::runtime_error("Failed to create node."));

    grid_elem.setAttribute("enabled", grid->is_enabled() ? "true" : "false");
    grid_elem.setAttribute("distance", QString::fromStdString(number_to_string<double>(grid->get_distance())));
    grid_elem.setAttribute("offset", QString::fromStdString(number_to_string<int>(grid->get_min())));
    grid_elem.setAttribute("orientation", QString::fromStdString(grid_orientation));

    grids_elem.appendChild(grid_elem);
}

void ProjectExporter::add_irregular_grid(QDomDocument& doc,
                                         QDomElement& grids_elem,
                                         const IrregularGrid_shptr& grid,
                                         std::string const& grid_orientation)
{
    QDomElement grid_elem = doc.createElement("irregular-grid");
    if (grid_elem.isNull()) throw(std::runtime_error("Failed to create node."));

    grid_elem.setAttribute("enabled", grid->is_enabled() ? "true" : "false");
    grid_elem.setAttribute("orientation", QString::fromStdString(grid_orientation));

    QDomElement offsets_elem = doc.createElement("offsets");
    if (offsets_elem.isNull()) throw(std::runtime_error("Failed to create node."));

    for (auto offset : *grid)
    {
        QDomElement offset_elem = doc.createElement("offset-entry");
        if (offset_elem.isNull()) throw(std::runtime_error("Failed to create node."));

        offset_elem.setAttribute("offset", QString::fromStdString(number_to_string<int>(offset)));

        offsets_elem.appendChild(offset_elem);
    }

    grid_elem.appendChild(offsets_elem);

    grids_elem.appendChild(grid_elem);
}


void ProjectExporter::set_project_node_attributes(QDomDocument& doc,
                                                  QDomElement& prj_elem,
                                                  const Project_shptr& prj)
{
    prj_elem.setAttribute("degate-version", DEGATE_VERSION);
    prj_elem.setAttribute("name", QString::fromStdString(prj->get_name()));
    prj_elem.setAttribute("description", QString::fromStdString(prj->get_description()));
    prj_elem.setAttribute("width", QString::fromStdString(number_to_string<int>(prj->get_width())));
    prj_elem.setAttribute("height", QString::fromStdString(number_to_string<int>(prj->get_height())));

    prj_elem.setAttribute("lambda", QString::fromStdString(number_to_string<length_t>(prj->get_lambda())));
    prj_elem.setAttribute("pin-diameter",
                          QString::fromStdString(number_to_string<length_t>(prj->get_default_via_diameter())));
    prj_elem.setAttribute("wire-diameter",
                          QString::fromStdString(number_to_string<length_t>(prj->get_default_wire_diameter())));
    prj_elem.setAttribute("port-diameter",
                          QString::fromStdString(number_to_string<length_t>(prj->get_default_port_diameter())));

    prj_elem.setAttribute("pixel-per-um", QString::fromStdString(number_to_string<double>(prj->get_pixel_per_um())));
    prj_elem.setAttribute("template-dimension",
                          QString::fromStdString(number_to_string<int>(prj->get_template_dimension())));
    prj_elem.setAttribute("font-size", QString::fromStdString(number_to_string<unsigned int>(prj->get_font_size())));
}


void ProjectExporter::add_layers(QDomDocument& doc,
                                 QDomElement& prj_elem,
                                 const LogicModel_shptr& lmodel,
                                 std::string const& project_dir)
{
    if (lmodel == nullptr) throw InvalidPointerException();

    QDomElement layers_elem = doc.createElement("layers");
    if (layers_elem.isNull()) throw(std::runtime_error("Failed to create node."));

    for (auto layer_iter = lmodel->layers_begin();
         layer_iter != lmodel->layers_end(); ++layer_iter)
    {
        QDomElement layer_elem = doc.createElement("layer");
        if (layer_elem.isNull()) throw(std::runtime_error("Failed to create node."));

        Layer_shptr layer = *layer_iter;
        assert(layer->has_valid_layer_id());

        layer_elem.setAttribute("position",
                                QString::fromStdString(number_to_string<layer_position_t>(layer->get_layer_pos())));
        layer_elem.setAttribute("id", QString::fromStdString(number_to_string<layer_id_t>(layer->get_layer_id())));
        layer_elem.setAttribute("type", QString::fromStdString(layer->get_layer_type_as_string()));
        layer_elem.setAttribute("description", QString::fromStdString(layer->get_description()));
        layer_elem.setAttribute("enabled", QString::fromStdString(layer->is_enabled() ? "true" : "false"));

        if (layer->has_background_image())
            layer_elem.setAttribute("image-filename",
                                    QString::fromStdString(
                                        get_relative_path(layer->get_image_filename(), project_dir)));

        layers_elem.appendChild(layer_elem);
    }

    prj_elem.appendChild(layers_elem);
}


void ProjectExporter::add_port_colors(QDomDocument& doc,
                                      QDomElement& prj_elem,
                                      const PortColorManager_shptr& port_color_manager)
{
    if (port_color_manager == nullptr) throw InvalidPointerException();

    QDomElement port_colors_elem = doc.createElement("port-colors");
    if (port_colors_elem.isNull()) throw(std::runtime_error("Failed to create node."));

    for (auto iter = port_color_manager->begin();
         iter != port_color_manager->end(); ++iter)
    {
        const std::string port_name = (*iter).first;
        const color_t frame_color = port_color_manager->get_frame_color(port_name);
        const color_t fill_color = port_color_manager->get_fill_color(port_name);

        QDomElement color_elem = doc.createElement("port-color");
        if (color_elem.isNull()) throw(std::runtime_error("Failed to create node."));

        color_elem.setAttribute("fill-color", QString::fromStdString(to_color_string(fill_color)));
        color_elem.setAttribute("frame-color", QString::fromStdString(to_color_string(frame_color)));

        port_colors_elem.appendChild(color_elem);
    }

    prj_elem.appendChild(port_colors_elem);
}

void ProjectExporter::add_colors(QDomDocument& doc, QDomElement& prj_elem, const Project_shptr& prj)
{
    if (prj == nullptr) throw InvalidPointerException();

    QDomElement colors_elem = doc.createElement("default-colors");
    if (colors_elem.isNull()) throw(std::runtime_error("Failed to create node."));

    default_colors_t default_colors = prj->get_default_colors();
    for(const auto& p : default_colors)
    {
        QDomElement color_elem = doc.createElement("color");
        if (color_elem.isNull()) throw(std::runtime_error("Failed to create node."));

        std::string o;
        switch (p.first)
        {
        case DEFAULT_COLOR_WIRE: o = "wire";
            break;
        case DEFAULT_COLOR_VIA_UP: o = "via-up";
            break;
        case DEFAULT_COLOR_VIA_DOWN: o = "via-down";
            break;
        case DEFAULT_COLOR_EMARKER: o = "emarker";
            break;
        case DEFAULT_COLOR_GRID: o = "grid";
            break;
        case DEFAULT_COLOR_ANNOTATION: o = "annotation";
            break;
        case DEFAULT_COLOR_ANNOTATION_FRAME: o = "annotation-frame";
            break;
        case DEFAULT_COLOR_GATE: o = "gate";
            break;
        case DEFAULT_COLOR_GATE_FRAME: o = "gate-frame";
            break;
        case DEFAULT_COLOR_GATE_PORT: o = "gate-port";
            break;
        case DEFAULT_COLOR_TEXT: o = "text";
            break;
        default:
            throw std::runtime_error("Invalid object type.");
        }

        color_elem.setAttribute("object", QString::fromStdString(o));
        color_elem.setAttribute("color", QString::fromStdString(to_color_string(p.second)));

        colors_elem.appendChild(color_elem);
    }

    prj_elem.appendChild(colors_elem);
}
