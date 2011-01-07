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

#include "Via.h"
#include <XmlRpc.h>
#include <boost/format.hpp>

using namespace degate;

Via::Via(int _x, int _y, diameter_t _diameter, Via::DIRECTION _direction) :
  Circle(_x, _y, _diameter),
  direction(_direction) {
}

Via::~Via() {}

Via::DIRECTION Via::get_direction() const {
  return direction;
}

void Via::set_direction(Via::DIRECTION dir) {
  direction = dir;
}

const std::string Via::get_direction_as_string() const {
  switch(direction) {
  case DIRECTION_UP: return std::string("up");
  case DIRECTION_DOWN: return std::string("down");
  case DIRECTION_UNDEFINED:
  default: return std::string("undefined");
  }
}

Via::DIRECTION Via::get_via_direction_from_string(std::string const& via_direction_str) throw() {

  if(via_direction_str == "up") return Via::DIRECTION_UP;
  else if(via_direction_str == "down") return Via::DIRECTION_DOWN;
  else if(via_direction_str == "undefined") return Via::DIRECTION_UNDEFINED;
  else throw DegateRuntimeException("Can't parse via direction type.");
}

const std::string Via::get_descriptive_identifier() const {
  if(has_name()) {
    boost::format fmter("%1% (%2%)");
    fmter % get_name() % get_object_id();
    return fmter.str();
  }
  else {
    boost::format fmter("via (%1%)");
    fmter % get_object_id();
    return fmter.str();
  }
}

const std::string Via::get_object_type_name() const {
  return std::string("Via");
}


void Via::print(std::ostream & os, int n_tabs) const {

  os
    << gen_tabs(n_tabs) << "Via name          : " << get_name() << std::endl
    << gen_tabs(n_tabs) << "Object ID         : " << get_object_id() << std::endl
    << gen_tabs(n_tabs) << "Via position      : " << get_x() << " / " << get_y() << std::endl
    << gen_tabs(n_tabs) << "Bounding box      : " << Circle::get_bounding_box().to_string() << std::endl
    << std::endl;
    ;

}

//bool Via::in_shape(int x, int y) const {
//  return Circle::in_shape(x,y);
//}

void Via::shift_x(int delta_x) {
  Circle::shift_x(delta_x);
  notify_shape_change();
}

void Via::shift_y(int delta_y) {
  Circle::shift_y(delta_y);
  notify_shape_change();
}

void Via::set_x(int x) {
  Circle::set_x(x);
  notify_shape_change();
}

void Via::set_y(int y) {
  Circle::set_y(y);
  notify_shape_change();
}

void Via::set_diameter(unsigned int diameter) {
  Circle::set_diameter(diameter);
  notify_shape_change();
}


object_id_t Via::push_object_to_server(std::string const& server_url) {

  try {

    xmlrpc_c::paramList params;
    params.add(xmlrpc_c::value_string("add"));
    params.add(xmlrpc_c::value_string("via"));

    Layer_shptr layer = get_layer();
    assert(layer != NULL);
    params.add(xmlrpc_c::value_int(layer->get_layer_id()));

    params.add(xmlrpc_c::value_int(get_x()));
    params.add(xmlrpc_c::value_int(get_y()));
    params.add(xmlrpc_c::value_int(get_diameter()));
    params.add(xmlrpc_c::value_string(get_direction_as_string()));

    int const transaction_id =
      xmlrpc_c::value_int(remote_method_call(server_url, "degate.push", params));

    set_remote_object_id(transaction_id);

    std::cout << "Pushed via to server. remote id is: " << transaction_id << std::endl;
    return transaction_id;
  }
  catch(std::exception const& e) {
    std::cerr << "Client threw error: " << e.what() << std::endl;
    throw XMLRPCException(e.what());
  }
  catch(...) {
    std::cerr << "Client threw unexpected error." << std::endl;
    throw XMLRPCException("Client threw unexpected error.");
  }

}
