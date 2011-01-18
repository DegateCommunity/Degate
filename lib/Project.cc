/*

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

#include "globals.h"
#include "Project.h"

#include <string>
#include <iostream>
#include <ctime>
#include <cstdlib>

using namespace std;
using namespace degate;

Project::Project(length_t width, length_t height) :
  bounding_box(width, height),
  logic_model(new LogicModel(width, height)),
  port_color_manager(new PortColorManager()) {
  init_default_values();
}


Project::Project(length_t width, length_t height, std::string const& _directory, unsigned int layers) :
  bounding_box(width, height),
  directory(_directory),
  logic_model(new LogicModel(width, height, layers)),
  port_color_manager(new PortColorManager()) {
  init_default_values();
}

Project::~Project() {
}

void Project::set_project_directory(std::string const& _directory) {
  directory = _directory;
}

std::string const& Project::get_project_directory() {
  return directory;
}

BoundingBox const& Project::get_bounding_box() const {
  return bounding_box;
}

unsigned int Project::get_width() const {
  return bounding_box.get_width();
}

unsigned int Project::get_height() const {
  return bounding_box.get_height();
}

LogicModel_shptr Project::get_logic_model() {
  return logic_model;
}


void Project::set_logic_model(LogicModel_shptr _logic_model) {
  logic_model = _logic_model;
}


void Project::set_name(std::string _name) {
  name = _name;
}

std::string const& Project::get_name() const {
  return name;
}

void Project::set_description(std::string _description) {
  description = _description;
}

std::string const& Project::get_description() const {
  return description;
}

void Project::set_degate_version(std::string version_str) {
  degate_version = version_str;
}

std::string const& Project::get_degate_version() {
  return degate_version;
}

void Project::set_lambda(length_t l) {
  lambda = l;
}

length_t Project::get_lambda() const {
  return lambda;
}

void Project::set_default_pin_diameter(diameter_t pin_diameter) {
  default_pin_diameter = pin_diameter;
}

diameter_t Project::get_default_pin_diameter() const {
  return default_pin_diameter;
}

void Project::set_default_wire_diameter(diameter_t wire_diameter) {
  default_wire_diameter = wire_diameter;
}

diameter_t Project::get_default_wire_diameter() const {
  return default_wire_diameter;
}

void Project::set_default_port_diameter(diameter_t port_diameter) {
  default_port_diameter = port_diameter;
}

diameter_t Project::get_default_port_diameter() const {
  return default_port_diameter;
}


void Project::set_changed(bool state) {
  changed = state;
  if(state == false)
    reset_last_saved_counter();
}

bool Project::is_changed() const {
  return changed;
}

time_t Project::get_time_since_last_save() const {
  return time(NULL) - last_persistent_version;
}

void Project::reset_last_saved_counter() {
  last_persistent_version = time(NULL);
}

RegularGrid_shptr Project::get_regular_horizontal_grid() {
  return regular_horizontal_grid;
}

RegularGrid_shptr Project::get_regular_vertical_grid() {
  return regular_vertical_grid;
}

IrregularGrid_shptr Project::get_irregular_horizontal_grid() {
  return irregular_horizontal_grid;
}

IrregularGrid_shptr Project::get_irregular_vertical_grid() {
  return irregular_vertical_grid;
}

PortColorManager_shptr Project::get_port_color_manager() {
  return port_color_manager;
}

void Project::print(std::ostream & os) {
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
    << "+ Default pin diameter: " << default_pin_diameter << endl
    << "+ Default port diameter: " << default_port_diameter << endl
    << "+ Min distance between electrically isolated objects in pixel (lambda value): " << lambda << endl
    << endl;

}

void Project::print_all(std::ostream & os) {
  print(os);
  if(logic_model == NULL) {
    os << "+ The project has no logic model." << endl
       << endl;
  }
  else
    logic_model->print(os);
}

void Project::init_default_values() {
  default_pin_diameter = 5;
  default_wire_diameter = 5;
  default_port_diameter = 5;
  lambda = 5;
  last_transaction_id = 0;

  pixel_per_um = 0;

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

  regular_horizontal_grid = RegularGrid_shptr(new RegularGrid(Grid::HORIZONTAL));
  regular_vertical_grid = RegularGrid_shptr(new RegularGrid(Grid::VERTICAL));
  irregular_horizontal_grid = IrregularGrid_shptr(new IrregularGrid(Grid::HORIZONTAL));
  irregular_vertical_grid = IrregularGrid_shptr(new IrregularGrid(Grid::VERTICAL));

  // Generate a server URL

  srand(time(NULL));
  std::string channel_ident;
  while(channel_ident.size() <= 20)
    channel_ident += (char)('a'+(int) (26.0*rand()/(RAND_MAX+1.0)));


  boost::format f(Configuration::get_instance().get_servers_uri_pattern());
  f % channel_ident;
  server_url = f.str();
}


void Project::set_server_url(std::string const& server_url) {
  this->server_url = server_url;
}

std::string Project::get_server_url() const {
  return server_url;
}

transaction_id_t Project::get_last_pulled_tid() const {
  return last_transaction_id;
}

void Project::set_last_pulled_tid(transaction_id_t tid) {
  last_transaction_id = tid;
}


void Project::set_default_color(ENTITY_COLOR e, color_t c) {
  default_colors[e] = c;
}

color_t Project::get_default_color(ENTITY_COLOR e) const {
  default_colors_t::const_iterator i = default_colors.find(e);
  if(i == default_colors.end()) return 0;
  else return i->second;
}

const default_colors_t Project::get_default_colors() const {
  return default_colors;
}

double Project::get_pixel_per_um() const {
  return pixel_per_um;
}

void Project::set_pixel_per_um(double pix_per_um) {
  pixel_per_um = pix_per_um;
}


int Project::get_template_dimension() const {
  return template_dimension;
}

void Project::set_template_dimension(int template_dimension) {
  this->template_dimension = template_dimension;
}

DRCBase::container_type & Project::get_drcv_blacklist() {
  return drcv_blacklist;
}
