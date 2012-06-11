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

#include <degate.h>
#include <Gate.h>

#include <boost/format.hpp>

using namespace degate;

Gate::Gate(int _min_x, int _max_x, int _min_y, int _max_y,
	   ORIENTATION _orientation) :
  Rectangle(_min_x, _max_x, _min_y, _max_y),
  orientation(_orientation),
  template_type_id(0) {
}

Gate::Gate(BoundingBox const& bounding_box,
	   ORIENTATION _orientation):
  Rectangle(bounding_box.get_min_x(), bounding_box.get_max_x(),
	    bounding_box.get_min_y(), bounding_box.get_max_y()),
  orientation(_orientation),
  template_type_id(0) {
}

Gate::~Gate() {
  debug(TM, "destroy gate");
  if(gate_template != NULL) remove_template();
}

void Gate::add_port(GatePort_shptr gate_port) {
  if(!gate_port->has_valid_object_id())
    throw InvalidObjectIDException("Error in Gate::add_port(). "
				   "The port has no valid object ID.");

  if(!gate_port->has_template_port())
    throw DegateLogicException("Error in Gate::add_port(). "
			       "The gate port has no template port");

  if(!has_orientation())
    throw DegateLogicException("Error in Gate::add_port(). "
			       "The gate has no defined orientation");

  gate_port->set_x(get_min_x() +
		   get_relative_x_position_within_gate
		     (gate_port->get_template_port()->get_x()));
  gate_port->set_y(get_min_y() +
		   get_relative_y_position_within_gate
		     (gate_port->get_template_port()->get_y()));

  gate_ports.insert(gate_port);
}


void Gate::remove_port(GatePort_shptr gate_port) {
  port_iterator found = gate_ports.find(gate_port);
  if(found != gate_ports.end()) {
    gate_ports.erase(found);
  }
  else throw CollectionLookupException();
}


GatePort_shptr Gate::get_port_by_template_port(GateTemplatePort_shptr template_port) {
  for(port_iterator piter = ports_begin(); piter != ports_end(); ++piter) {
    GatePort_shptr gate_port = *piter;
    GateTemplatePort_shptr tmpl_port = gate_port->get_template_port();
    if(tmpl_port == template_port)
      return gate_port;
  }
  throw CollectionLookupException();
}


void Gate::set_template_type_id(object_id_t template_type_id) {
  this->template_type_id = template_type_id;
}


object_id_t Gate::get_template_type_id() const {
  return template_type_id;
}



void Gate::set_gate_template(std::shared_ptr<GateTemplate> gate_template) {
  if(has_template()) {
    this->gate_template->decrement_reference_counter();
  }

  if(this->gate_template != gate_template) {
    this->gate_template = gate_template;
    set_template_type_id(gate_template->get_object_id());
    set_fill_color(gate_template->get_fill_color());
    set_frame_color(gate_template->get_frame_color());
    this->gate_template->increment_reference_counter();

    if((unsigned int)get_width() != gate_template->get_width() ||
       (unsigned int)get_height() != gate_template->get_height()) {
      set_max_x(get_min_x() + gate_template->get_width());
      set_max_y(get_min_y() + gate_template->get_height());
    }
  }
}


std::shared_ptr<GateTemplate> Gate::get_gate_template() const {
  return gate_template;
}

bool Gate::has_template() const {
  return gate_template != NULL;
}

void Gate::remove_template() {
  gate_ports.clear();
  orientation = ORIENTATION_UNDEFINED;
  template_type_id = 0;
  set_fill_color(0);
  set_frame_color(0);
  if(has_template()) {
    gate_template->decrement_reference_counter();
    gate_template.reset();
  }
}

bool Gate::has_template_port(GateTemplatePort_shptr template_port) const {
  for(port_iterator piter = ports_begin(); piter != ports_end(); ++piter) {
    GatePort_shptr gate_port = *piter;

    assert(gate_port->get_template_port()->has_valid_object_id());
    assert(template_port->has_valid_object_id());

    if(gate_port->get_template_port()->get_object_id() == template_port->get_object_id()) {

      // debugging
      if(gate_port->get_template_port() != template_port) {
	std::cout << "ERROR\n";
	gate_port->print();
      }
      // debugging
      //assert(gate_port->get_template_port() == template_port);

      return true;
    }

  }
  return false;
}


void Gate::set_orientation(ORIENTATION _orientation) {
  orientation = _orientation;
}

Gate::ORIENTATION Gate::get_orientation() const {
  return orientation;
}

bool Gate::has_orientation() const {
  return orientation != ORIENTATION_UNDEFINED;
}

std::string Gate::get_orienation_type_as_string() const {
  switch(orientation) {
  case ORIENTATION_NORMAL: return std::string("normal");
  case ORIENTATION_FLIPPED_UP_DOWN: return std::string("flipped-up-down");
  case ORIENTATION_FLIPPED_LEFT_RIGHT: return std::string("flipped-left-right");
  case ORIENTATION_FLIPPED_BOTH: return std::string("flipped-both");
  case ORIENTATION_UNDEFINED:
  default: return std::string("undefined");
  }
}


Gate::port_iterator Gate::ports_begin() {
  return gate_ports.begin();
}

Gate::port_const_iterator Gate::ports_begin() const {
  return gate_ports.begin();
}

Gate::port_iterator Gate::ports_end() {
  return gate_ports.end();
}

Gate::port_const_iterator Gate::ports_end() const {
  return gate_ports.end();
}


unsigned int Gate::get_relative_x_position_within_gate(int rel_x) const {
  switch(orientation) {
  case ORIENTATION_NORMAL:
  case ORIENTATION_FLIPPED_UP_DOWN:
    return rel_x;
  case ORIENTATION_FLIPPED_LEFT_RIGHT:
  case ORIENTATION_FLIPPED_BOTH:
    return get_width() - rel_x;
  case ORIENTATION_UNDEFINED:
  default:
    assert(orientation != ORIENTATION_UNDEFINED);
    throw DegateRuntimeException("Can't calculate a position for an undefined orientation");
  }
}


unsigned int Gate::get_relative_y_position_within_gate(int rel_y) const {
  switch(orientation) {
  case ORIENTATION_NORMAL:
  case ORIENTATION_FLIPPED_LEFT_RIGHT:
    return rel_y;
  case ORIENTATION_FLIPPED_UP_DOWN:
  case ORIENTATION_FLIPPED_BOTH:
    return get_height() - rel_y;
  case ORIENTATION_UNDEFINED:
  default:
    assert(orientation != ORIENTATION_UNDEFINED);
    throw DegateRuntimeException("Can't calculate a position for an undefined orientation");
  }
}


const std::string Gate::get_descriptive_identifier() const {

  if(has_template()) {
    if(has_name()) {
      boost::format fmter("%1% : %2%");
      fmter % get_name() % get_gate_template()->get_name();
      return fmter.str();
    }
    else {
      boost::format fmter("%1% (%2%)");
      fmter % get_gate_template()->get_name() % get_object_id();
      return fmter.str();
    }
  }
  else {
    if(!has_name()) {
      boost::format fmter("gate (%1%)");
      fmter % get_object_id();
      return fmter.str();
    }
    else {
      boost::format fmter("gate %1% (%2%)");
      fmter % get_name() % get_object_id();
      return fmter.str();
    }
  }

}


const std::string Gate::get_object_type_name() const {
  return std::string("Gate");
}

void Gate::print(std::ostream & os, int n_tabs) const {

  os
    << gen_tabs(n_tabs) << "Gate name        : " << get_name() << std::endl
    << gen_tabs(n_tabs) << "Gate description : " << get_description() << std::endl
    << gen_tabs(n_tabs) << "Object ID        : " << get_object_id() << std::endl
    << gen_tabs(n_tabs) << "Bounding box     : " << Rectangle::get_bounding_box().to_string() << std::endl
    << std::endl;

  for(port_const_iterator pi = ports_begin(); pi != ports_end(); ++pi) {
    const GatePort_shptr gate_port = *pi;
    gate_port->print(os, n_tabs + 1);
    os << std::endl;
  }

  os << std::endl;
}
