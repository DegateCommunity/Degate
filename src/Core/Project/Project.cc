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
#include "Project.h"
#include "Core/Version.h"

#include <algorithm>
#include <memory>

#include <string>
#include <iostream>
#include <ctime>
#include <cstdlib>

using namespace std;
using namespace degate;

Project::Project(length_t width, length_t height, ProjectType project_type) :
    bounding_box(width, height),
    degate_version(DEGATE_VERSION),
    logic_model(new LogicModel(width, height, project_type)),
    port_color_manager(new PortColorManager()),
    project_type(project_type)
{
    init_default_values();
}


Project::Project(length_t width, length_t height, std::string const& directory, ProjectType project_type, unsigned int layers) :
    bounding_box(width, height),
    degate_version(DEGATE_VERSION),
    directory(directory),
    logic_model(new LogicModel(width, height, project_type, layers)),
    port_color_manager(new PortColorManager()),
    project_type(project_type)
{
    init_default_values();
}

Project::~Project()
{
}

DeepCopyable_shptr Project::clone_shallow() const
{
    auto clone = std::make_shared<Project>(*this);
    clone->regular_horizontal_grid.reset();
    clone->regular_vertical_grid.reset();
    clone->irregular_horizontal_grid.reset();
    clone->irregular_vertical_grid.reset();
    clone->logic_model.reset();
    clone->port_color_manager.reset();
    return clone;
}

void Project::clone_deep_into(DeepCopyable_shptr destination, oldnew_t* oldnew) const
{
    auto clone = std::dynamic_pointer_cast<Project>(destination);

    clone->logic_model = std::dynamic_pointer_cast<LogicModel>(logic_model->clone_deep(oldnew));

    // For these members we use the default copy constructors.
    clone->regular_horizontal_grid = std::make_shared<RegularGrid>(*regular_horizontal_grid);
    clone->regular_vertical_grid = std::make_shared<RegularGrid>(*regular_vertical_grid);
    clone->irregular_horizontal_grid = std::make_shared<IrregularGrid>(*irregular_horizontal_grid);
    clone->irregular_vertical_grid = std::make_shared<IrregularGrid>(*irregular_vertical_grid);
    clone->port_color_manager = std::make_shared<PortColorManager>(*port_color_manager);
}

void Project::set_project_directory(std::string const& directory)
{
    this->directory = directory;
}

std::string const& Project::get_project_directory()
{
    return directory;
}

BoundingBox const& Project::get_bounding_box() const
{
    return bounding_box;
}

unsigned int Project::get_width() const
{
    return static_cast<unsigned int>(std::ceil(bounding_box.get_width()));
}

unsigned int Project::get_height() const
{
    return static_cast<unsigned int>(std::ceil(bounding_box.get_height()));
}

bool Project::update_size(unsigned int width, unsigned int height)
{
    bool res = false;

    if (width > static_cast<unsigned int>(std::ceil(bounding_box.get_width())))
    {
        bounding_box.set_max_x(static_cast<float>(width));
        res = true;
    }

    if (height > static_cast<unsigned int>(std::ceil(bounding_box.get_height())))
    {
        bounding_box.set_max_y(static_cast<float>(height));
        res = true;
    }

    return res;
}

LogicModel_shptr Project::get_logic_model()
{
    return logic_model;
}


void Project::set_logic_model(LogicModel_shptr logic_model)
{
    this->logic_model = logic_model;
}


void Project::set_name(const std::string& name)
{
    this->name = name;
}

std::string const& Project::get_name() const
{
    return name;
}

void Project::set_description(std::string description)
{
    this->description = description;
}

std::string const& Project::get_description() const
{
    return description;
}

void Project::set_degate_version(std::string version_str)
{
    degate_version = version_str;
}

std::string const& Project::get_degate_version()
{
    return degate_version;
}

void Project::set_lambda(length_t l)
{
    lambda = l;
}

length_t Project::get_lambda() const
{
    return lambda;
}

void Project::set_default_via_diameter(diameter_t via_diameter)
{
    default_via_diameter = via_diameter;
}

diameter_t Project::get_default_via_diameter() const
{
    return default_via_diameter;
}

void Project::set_default_wire_diameter(diameter_t wire_diameter)
{
    default_wire_diameter = wire_diameter;
}

diameter_t Project::get_default_wire_diameter() const
{
    return default_wire_diameter;
}

void Project::set_default_port_diameter(diameter_t port_diameter)
{
    default_port_diameter = port_diameter;
}

diameter_t Project::get_default_port_diameter() const
{
    return default_port_diameter;
}


void Project::set_changed(bool state)
{
    changed = state;
    if (state == false)
        reset_last_saved_counter();
}

bool Project::is_changed() const
{
    return changed;
}

time_t Project::get_time_since_last_save() const
{
    return time(nullptr) - last_persistent_version;
}

void Project::reset_last_saved_counter()
{
    last_persistent_version = time(nullptr);
}

RegularGrid_shptr Project::get_regular_horizontal_grid()
{
    return regular_horizontal_grid;
}

RegularGrid_shptr Project::get_regular_vertical_grid()
{
    return regular_vertical_grid;
}

IrregularGrid_shptr Project::get_irregular_horizontal_grid()
{
    return irregular_horizontal_grid;
}

IrregularGrid_shptr Project::get_irregular_vertical_grid()
{
    return irregular_vertical_grid;
}

PortColorManager_shptr Project::get_port_color_manager()
{
    return port_color_manager;
}

void Project::print(std::ostream& os)
{
    os
        << endl
        << "================================[ "
        << (name.empty() ? "Unnamed project" : name)
        << " ]=================================" << endl
        << "+ Project description: " << description << endl
        << "+ Degate version: " << degate_version << endl
        << "+ Directory: " << directory << endl
        << "+" << endl
        << "+ Changed: " << (changed ? "true" : "false") << endl
        << "+" << endl
        << "+ Default wire diameter: " << default_wire_diameter << endl
        << "+ Default pin diameter: " << default_via_diameter << endl
        << "+ Default port diameter: " << default_port_diameter << endl
        << "+ Min distance between electrically isolated objects in pixel (lambda value): " << lambda << endl
        << endl;
}

void Project::print_all(std::ostream& os)
{
    print(os);
    if (logic_model == nullptr)
    {
        os << "+ The project has no logic model." << endl
            << endl;
    }
    else
        logic_model->print(os);
}

void Project::init_default_values()
{
    default_via_diameter = 5;
    default_wire_diameter = 5;
    default_port_diameter = 5;
    template_dimension = 40;
    lambda = 5;
    last_transaction_id = 0;

    pixel_per_um = 0;
    font_size = 12;

    // A B G R
    default_colors[DEFAULT_COLOR_WIRE] = 0xff00a3fb;
    default_colors[DEFAULT_COLOR_VIA_UP] = 0xffff8900;
    default_colors[DEFAULT_COLOR_VIA_DOWN] = 0xff0023ff;
    default_colors[DEFAULT_COLOR_GRID] = 0xffff1200;
    default_colors[DEFAULT_COLOR_ANNOTATION] = 0xa0303030;
    default_colors[DEFAULT_COLOR_ANNOTATION_FRAME] = 0xa032d932;
    default_colors[DEFAULT_COLOR_GATE] = 0xa0303030;
    default_colors[DEFAULT_COLOR_GATE_FRAME] = 0xa032b0d9;
    default_colors[DEFAULT_COLOR_GATE_PORT] = 0xff0000ff;
    default_colors[DEFAULT_COLOR_TEXT] = 0xffffffff;
    default_colors[DEFAULT_COLOR_EMARKER] = 0xffa0a0ff;

    set_changed(false);

    reset_last_saved_counter();

    regular_horizontal_grid = std::make_shared<RegularGrid>(Grid::HORIZONTAL);
    regular_vertical_grid = std::make_shared<RegularGrid>(Grid::VERTICAL);
    irregular_horizontal_grid = std::make_shared<IrregularGrid>(Grid::HORIZONTAL);
    irregular_vertical_grid = std::make_shared<IrregularGrid>(Grid::VERTICAL);
}

void Project::set_default_color(ENTITY_COLOR e, color_t c)
{
    default_colors[e] = c;
}

color_t Project::get_default_color(ENTITY_COLOR e) const
{
    default_colors_t::const_iterator i = default_colors.find(e);
    if (i == default_colors.end()) return 0;
    else return i->second;
}

const default_colors_t Project::get_default_colors() const
{
    return default_colors;
}

double Project::get_pixel_per_um() const
{
    return pixel_per_um;
}

void Project::set_pixel_per_um(double pix_per_um)
{
    pixel_per_um = pix_per_um;
}


int Project::get_template_dimension() const
{
    return template_dimension;
}

void Project::set_template_dimension(int template_dimension)
{
    this->template_dimension = template_dimension;
}

void Project::set_font_size(unsigned int font_size)
{
    this->font_size = font_size;
}

unsigned int Project::get_font_size() const
{
    return font_size;
}


RCBase::container_type& Project::get_rcv_blacklist()
{
    return rcv_blacklist;
}

ProjectType Project::get_project_type() const
{
    return project_type;
}